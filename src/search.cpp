#include "search.h"

#include <stack>
#include <vector>
#include <ostream>

void build_tau_intervals( BWTReader& b, QIntervalManager& qmgr, GSAReader& grdr,
                          vector< NucleoCounter >& C, int T )
{
  QInterval* qint;
  for( int i( 1 ); i < T; ++i )
    {
      unsigned long int nwintc    =0;
      unsigned long int rejintc   =0;

      std::cerr << "@ " << now( "%I:%M:%S %p %Z" ) << std::endl;
      std::cerr << "--> Building tau intervals - " << i+1 << "/" << T << std::endl;
      std::cerr << "--> Testing suffixes of length " << i << std::endl;
      while( (qint = qmgr.get_next_interval()) )
        {
          BWTPosition begin = qint->get_begin( );
          BWTPosition end = qint->get_end( );

          if( !(grdr.move_to( begin )) )
            {
              std::cerr << "ERROR IN: seach.cpp:build_tau_interval" << std::endl;
              std::cerr << "Can't move to " << begin << std::endl;
              std::cerr << "Aborting..." << std::endl;
              std::exit( -1 );
            }

          b.move_to( begin );
          GSAEntry beginGSA = grdr.get_v( );

          if( beginGSA.sa == (SequenceLength) i )
            {
              vector< NucleoCounter > occ_begin( b.get_Pi( ) );
              b.move_to( end );
              vector< NucleoCounter > occ_end( b.get_Pi( ) );
              for( int base( BASE_A ); base < ALPHABET_SIZE; ++base )
                {
                  BWTPosition new_begin = C[ base ] + occ_begin[ base ];
                  BWTPosition new_end   = C[ base ] + occ_end[ base ];
                  if( new_end - new_begin >= 2 )
                    {
                      // Shared substring
                      ++nwintc;
                      QInterval new_interval ( new_begin, new_end );
                      qmgr.add_interval( new_interval, (Nucleotide) base );
                    }
                }
            }
          else
            {
              ++rejintc;
            }
        }
      b.reset( );
      grdr.reset( );
      qmgr.swap_files( );
      
      std::cerr << "--> Generated " << nwintc << " new intervals." << std::endl;
      std::cerr << "--> Rejected " << rejintc << " intervals (string is not suffix)." << std::endl;
    }
}

void left_step( BWTReader& b, QIntervalManager& qmgr, GSAReader& grdr,
                vector< NucleoCounter >& C, int iteration )
{
  QInterval* qint     =NULL;
  unsigned long int nwintc  =0;
  unsigned long int rejintc   =0;
  unsigned long int nwedgc  =0;
  unsigned long int notsuffc  =0;

  SeqNExtVect   rsuff;
  SeqNExtVect   rpref;
  BWTPExtVect   rprefixpos;

  std::cerr << "--> Testing suffixes of length " << iteration << std::endl;
  std::cerr << "--> Producing suffixes of length " << iteration +1 << std::endl;

  while( (qint = qmgr.get_next_interval()) )
    {
      BWTPosition begin   = qint->get_begin();
      BWTPosition end   = qint->get_end();

      if( !(grdr.move_to( begin )) )
        {
          std::cerr << "ERROR IN: search.cpp:left_step" << std::endl;
          std::cerr << "Can't move to " << begin << std::endl;
          std::cerr << "Current position is " << grdr.get_position() << std::endl;
          std::cerr << "Aborting..." << std::endl;
          std::exit(-1);
        }
      GSAEntry gsafent    = grdr.get_v(); // GSA entry @ begin

      b.move_to( begin );

      if( gsafent.sa == (SequenceLength) iteration )
        {
          vector< NucleoCounter > occ_begin   = b.get_Pi();
          b.move_to_storing_sent( end, rprefixpos );
          vector< NucleoCounter > occ_end   = b.get_Pi();
          // Output edges if they exists
          if(rprefixpos.size() > 0)
            {
              grdr.get_seq_sent( iteration , rsuff, end );
              grdr.get_seq_at_pos( rprefixpos, rpref, end );

              size_t rsize = rsuff.size();
              size_t psize = rpref.size();

              if(rsize * psize > 0)
                {
                  nwedgc += rsize * psize;

                  for( SeqNExtVect::iterator rsi = rsuff.begin(); rsi != rsuff.end(); ++rsi)
                    for( SeqNExtVect::iterator psi = rpref.begin(); psi != rpref.end(); ++psi)
                      std::cout << *rsi << "," << *psi << "," << iteration << std::endl;
                  // std::cout << "-->FROM NODES" << std::endl;
                  // for(SeqNExtVect::bufreader_type sufreader(rsuff); !sufreader.empty(); ++sufreader)
                  //   {
                  //     std::cout << *sufreader << std::endl;
                  //   }
                  // std::cout << "-->TO NODES" << std::endl;
                  // for(SeqNExtVect::bufreader_type prefreader(rpref); !prefreader.empty(); ++prefreader)
                  //   {
                  //     std::cout << *prefreader << std::endl;
                  //   }
                }
            }
          rprefixpos.clear();
          rsuff.clear();
          rpref.clear();

          // Extend interval
          for( int base( BASE_A ); base < ALPHABET_SIZE; ++base )
            {
              BWTPosition new_begin   = C[ base ] + occ_begin[ base ];
              BWTPosition new_end   = C[ base ] + occ_end[ base ];
              if( new_end - new_begin >= 2 )
                {
                  QInterval new_interval( new_begin, new_end );
                  qmgr.add_interval( new_interval, (Nucleotide) base );
                  ++nwintc;
                }
              else
                {
                  ++rejintc;
                }
            }
        }
      else
        {
          ++notsuffc;
        }
    }
  std::cerr << "--> Generated " << nwintc << " new intervals" << std::endl;
  std::cerr << "--> Rejected " << rejintc << " unique backward extensions" << std::endl;
  std::cerr << "--> Rejected " << notsuffc << " intervals (pattern is not suffix)" << std::endl;
  std::cerr << "--> Produced " << nwedgc << " new edges" << std::endl;
}

BWTPosition OccLT( vector< NucleoCounter >& occ, Nucleotide base )
{
  // Count occurrences of bases lexicographically less than base given occ
  BWTPosition p =0;

  for(unsigned int i( 0 ); i < (unsigned int) base; ++i )
    p += (BWTPosition) occ[ i ];

  return p;
}

// void build_tau_intervals( BWTReader& b, JoinedQIntervalManager& jqmgr,
//                           vector< NucleoCounter >& C, int T)
// {
//   JoinedQInterval* jqin;
//   for( int i( 0 ); i < T; ++i)
//     {
//       std::cerr << "Building tau intervals - " << i+1 << "/" << T << std::endl;
//       unsigned long int nwintc =0; // new intervals
//       unsigned long int uniquebkwe =0; // unique backward extension
//       while( (jqin = jqmgr.get_next_interval()) )
//         {
//           // std::cerr << jqin->get_interval( ).get_begin( ) << std::endl
//           //       << jqin->get_interval( ).get_end( ) << std::endl;
//           // Get occ at the beginning and end of the interval
//           BWTPosition begin = jqin->get_interval().get_begin();
//           BWTPosition end = jqin->get_interval().get_end();
//           b.move_to( begin );
//           vector< NucleoCounter > occ_begin( b.get_Pi( ) );
//           b.move_to( end );
//           vector< NucleoCounter > occ_end( b.get_Pi( ) );

//           for( int base( 1 ); base < ALPHABET_SIZE; ++base )
//             {
//               // Extend the interval with every possibile backward extension
//               BWTPosition new_rev_begin =  jqin->get_reverse_interval().get_begin() +
//                 (OccLT(occ_end, (Nucleotide) base) - OccLT(occ_begin, (Nucleotide) base));
//               BWTPosition new_rev_end =  new_rev_begin +   (occ_end[ base ] - occ_begin[ base ] );
//               BWTPosition new_begin = C[ base ] + occ_begin[ base ];
//               BWTPosition new_end = C[ base ] + occ_end[ base ];

//               if( new_end - new_begin >= 2 )
//                 {
//                   // Shared substring
//                   ++nwintc;
//                   JoinedQInterval new_interval ( new_begin, new_end, new_rev_begin, new_rev_end );
//                   jqmgr.add_interval( new_interval, (Nucleotide) base );
//                 }
//               else if ( new_end == new_begin + 1 )
//                 {
//                   // unique backward extension
//                   ++uniquebkwe;
//                 }
//             } // ~for
//           // delete jqin;
//         } // ~while
//       b.reset();
//       jqmgr.swap_files();
// #ifdef DEBUG
//       std::cerr << "--> Generated " << nwintc << " new intervals." << std::endl;
//       std::cerr << "--> Rejected " << uniquebkwe << " unique backward extension"
//                 << std::endl;
// #endif
//     } // ~for
// }


// size_t search_step_left( BWTReader& b, JoinedQIntervalManager& jqmgr,
//         vector< NucleoCounter >& C, stxxl::vector<PODEdgeInterval> &v )
// {
//   JoinedQInterval* jqin;
//   unsigned long int nwintc =0; // new intervals counter
//   size_t nwltc =0; // new left terminated intervals
//   unsigned long int uniquebkwe =0; // unique backward extension
//   // vector< EdgeInterval* >* LT = new vector< EdgeInterval* >(); // Left-terminated
//                     // intervals
//   // std::ofstream outputLT( outputfn.c_str(), std::ios::binary );
//   while( (jqin = jqmgr.get_next_interval()) )
//     {
//       // Get occ at the beginning and end of the interval
//       BWTPosition begin = jqin->get_interval().get_begin();
//       BWTPosition end = jqin->get_interval().get_end();
//       b.move_to( begin );
//       vector< NucleoCounter > occ_begin( b.get_Pi( ) );
//       b.move_to( end );
//       vector< NucleoCounter > occ_end( b.get_Pi( ) );

//       // Extend the interval with every possibile backward extension
//       for( int base( 0 ); base < ALPHABET_SIZE; ++base )
//    {
//      BWTPosition new_rev_begin = jqin->get_reverse_interval().get_begin() +
//        (OccLT(occ_end, (Nucleotide) base) - OccLT(occ_begin, (Nucleotide) base));
//      BWTPosition new_rev_end = new_rev_begin + (occ_end[ base ] - occ_begin[ base ] );
//      BWTPosition new_begin = C[ base ] + occ_begin[ base ];
//      BWTPosition new_end = C[ base ] + occ_end[ base ];

//      if( ( base != BASE_$ ) && ( new_end - new_begin >= 2 ) )
//        {
//          // New shared substring
//          ++nwintc;
//          JoinedQInterval new_interval ( new_begin, new_end, new_rev_begin, new_rev_end );
//          jqmgr.add_interval( new_interval, (Nucleotide) base );
//        }

//      else if ( ( base == BASE_$ ) && ( new_rev_end - new_rev_begin >= 1 ) )
//        {
//          // New left-terminated interval
//          ++nwltc;
//          PODEdgeInterval new_interval;
//          new_interval.b1 = new_rev_begin;
//          new_interval.e1 = new_rev_end;
//          new_interval.b2 = jqin->get_reverse_interval().get_begin();
//          new_interval.e2 = jqin->get_reverse_interval().get_end();
//          v.push_back(new_interval);
//          // EdgeInterval* new_interval =
//          //    new EdgeInterval ( new_rev_begin, new_rev_end,
//          //           jqin->get_reverse_interval( ).get_begin( ),
//          //           jqin->get_reverse_interval( ).get_end( ),
//          //           0 );
//          // outputLT << *new_interval;
//          // delete new_interval;
//          //LT->push_back( new_interval );
//        }

//      else if ( new_end - new_begin == 1 )
//        {
//          // Unique backward extension (don't care)
//          ++uniquebkwe;
//        }
//    } // ~for

//     } // ~while

// #ifdef DEBUG
//   std::cerr << "--> Generated " << nwintc << " new intervals." << std::endl;
//   std::cerr << "--> Generated " << nwltc << " new left-terminated intervals."
//        << std::endl;
//   std::cerr << "--> Rejected " << uniquebkwe << " unique backward extension"
//        << std::endl;
// #endif

//   return nwltc;
// }

// //vector< EdgeInterval* >*
// void search_step_right( BWTReader& b, EdgeJoinedQIntervalManager& imgr,
//        vector< NucleoCounter >& C, stxxl::vector<PODEdgeInterval> &v )
// {

//   // vector< EdgeInterval* >* edges_to_test = new vector< EdgeInterval* >( );
//   EdgeInterval* i_l = NULL; // interval from LT
//   EdgeInterval* i_m = imgr.get_next_interval( ); // interval from imgr

//   unsigned long int nwintc  =0; // new interval counter
//   unsigned long int rejintc =0; // rejected LT interval counter
//   unsigned long int mrgintc =0; // intervals from imgr merged with the ones from LT
//   unsigned long int nwett   =0; // new edges to test
//   unsigned long int v_pos   =0; // Position reached in v

//   // read first interval from LT if it exists
//   if(v_pos < v.size())
//     {
//       PODEdgeInterval x = v[v_pos];
//       i_l = new EdgeInterval(x.b1, x.e1, x.b2, x.e2, 0);
//       ++v_pos;
//     }
//   else
//     {
//       i_l = NULL;
//     }

//   // if(! ((*LT) >> &i_l) )
//   //   i_l = NULL;

//   while( ( i_l != NULL ) || ( i_m != NULL ) )
//     {
//       bool merged = false;
//       while( ( i_l != NULL ) && ( i_m != NULL ) && EqualFirstEdgeInterval( i_l, i_m ) )
//    {
//      // Need to merge i_lt and i_mgr because the first interval (the one we
//      // need to extend) is the same. Note that we don't know if i_lt is
//      // right-terminal as for now
//      ++mrgintc;
//      merged = true;
//      for( unsigned int i( 0 ); i < i_m->get_second_interval( ).size( ); ++i )
//        {
//          i_l->add_suffix_interval( i_m->get_second_interval( )[ i ],
//            i_m->get_len( )[ i ] );
//        }
//      // delete i_m;
//      i_m = imgr.get_next_interval( );
//    } // ~while merge

//       if( ( i_l != NULL ) && ( ( i_m == NULL ) || CompareEdgeInterval( i_l, i_m ) ) )
//    {
//      // Check if the new interval that comes from the left search is right
//      // terminated. If it is extend and save to imgr, if it isn't check if
//      // we have merged elements and extend them rejecting the new interval.
//      BWTPosition begin_f_i = i_l->get_first_interval( ).get_begin( );    // begin of Q'$-interval (same as begin Q'-interval)
//      BWTPosition end_f_i = i_l->get_first_interval( ).get_end( );        // end of Q'$-interval
//      BWTPosition end_s_i = i_l->get_second_interval( )[ 0 ]->get_end( ); // end of Q'-interval

// #ifdef DEBUG_VERBOSE
//      std::cerr << "In search_step_left : first section" << std::endl;
//      std::cerr << "Moving to position :" << begin_f_i << std::endl;
// #endif
//      b.move_to( begin_f_i );
//      vector< NucleoCounter > occ_b( b.get_Pi( ) );
// #ifdef DEBUG_VERBOSE
//      std::cerr << "Moving to position :" << end_s_i << std::endl;
// #endif
//      b.move_to( end_f_i );
//      vector< NucleoCounter > occ_e_f( b.get_Pi( ) );
//      b.move_to( end_s_i );
//      vector< NucleoCounter > occ_e( b.get_Pi( ) );

//      bool new_interval_is_right_terminal = ( occ_e[ BASE_$ ] > occ_e_f[ BASE_$ ] ) ? true : false ;

//      if( !new_interval_is_right_terminal )
//          ++rejintc;
//      if( new_interval_is_right_terminal || merged )
//        {
//          for( int base( BASE_$ ); base < ALPHABET_SIZE; ++base )
//      {
//        BWTPosition new_int_begin = C[ base ] + occ_b[ base ];
//        BWTPosition new_int_end = C[ base ] + occ_e_f[ base ];
//        if( new_int_end > new_int_begin )
//          {
//            BWTPosition first_suffix_begin = new_interval_is_right_terminal ?
//        occ_e_f[ BASE_$ ] : i_l->get_second_interval( )[ 1 ]->get_begin( );
//            BWTPosition first_suffix_end = new_interval_is_right_terminal ?
//        occ_e[ BASE_$ ] : i_l->get_second_interval( )[ 1 ]->get_end( );
//            EdgeInterval* new_interval =
//        new EdgeInterval( new_int_begin, new_int_end, first_suffix_begin,
//              first_suffix_end, new_interval_is_right_terminal ?
//              1 : i_l->get_len( )[ 1 ]+1 );
//            // Add other suffix if they exists
//            for( unsigned int i( new_interval_is_right_terminal ? 1 : 2 );
//           i < i_l->get_second_interval( ).size( ); ++i )
//        new_interval->add_suffix_interval( i_l->get_second_interval( )[ i ],
//                   i_l->get_len( )[ i ] +1 );
//            if( base == BASE_$ )
//        {
//          std::vector< EdgeLength >::iterator eit = new_interval->get_len().begin();
//          std::vector< QInterval* >::iterator qit = new_interval->get_second_interval().begin();
//          for(; qit != new_interval->get_second_interval().end(); ++qit, ++eit)
//            {
//              std::cout << "(" << new_interval->get_first_interval().get_begin()
//            << "," << new_interval->get_first_interval().get_end() << ")";
//              std::cout << "(" << (*qit)->get_begin()
//            << "," << (*qit)->get_end() << ")";
//              std::cout << *eit << "\n";
//            }
//          // edges_to_test->push_back( new_interval );
//          delete new_interval;
//        }
//            else
//        {
//          ++nwintc;
//          imgr.add_interval( *new_interval, (Nucleotide) base );
//          delete new_interval;
//        }
//          }
//        else
//          {
//            // Non existent intervals, don't care
//          } // ~if new_int_end > new_int_begin
//      } // ~for base
//        }

//      delete i_l;
//      if(v_pos < v.size())
//        {
//          PODEdgeInterval x = v[v_pos];
//          i_l = new EdgeInterval(x.b1, x.e1, x.b2, x.e2, 0);
//          ++v_pos;
//        }
//      else
//        {
//          i_l = NULL;
//        }
//
//      // if(!( (*LT) >> &i_l ))
//      //   i_l = NULL;
//    } // ~if i_l comes before i_m
//       else if( ( i_m != NULL ) && ( ( i_l == NULL ) || CompareEdgeInterval( i_m, i_l ) ) )
//    {
//      // Extend the interval that comes from the interval manager. If it
//      // extend with $ append it to edges_to_test
//      BWTPosition begin_f = i_m->get_first_interval( ).get_begin( );  // begin of Q'$-interval (same as begin Q'-interval)
//      BWTPosition end_f = i_m->get_first_interval( ).get_end( );      // end of Q'$-interval

// #ifdef DEBUG_VERBOSE
//      std::cerr << "In search_step_left : second section" << std::endl;
//      std::cerr << "Moving to position :" << begin_f << std::endl;
// #endif
//      b.move_to( begin_f );
//      vector< NucleoCounter > occ_b( b.get_Pi( ) );
// #ifdef DEBUG_VERBOSE
//      std::cerr << "Moving to position :" << end_f << std::endl;
// #endif
//      b.move_to( end_f );
//      vector< NucleoCounter > occ_e( b.get_Pi( ) );

//      if( occ_e[ BASE_$ ] > occ_b[ BASE_$ ] )
//        {
//          BWTPosition new_int_begin = occ_b[ BASE_$ ];
//          BWTPosition new_int_end = occ_e[ BASE_$ ];
//          EdgeInterval* new_interval =
//      new EdgeInterval( new_int_begin, new_int_end, i_m->get_second_interval( )[ 0 ]->get_begin( ),
//            i_m->get_second_interval( )[ 0 ]->get_end( ), i_m->get_len( )[ 0 ] );
//          for( unsigned int i( 1 ); i < i_m->get_second_interval( ).size( ); ++i )
//      {
//        new_interval->add_suffix_interval( i_m->get_second_interval( )[ i ],
//                   i_m->get_len( )[ i ] );
//      }

//          std::vector< EdgeLength >::iterator eit = new_interval->get_len().begin();
//          std::vector< QInterval* >::iterator qit = new_interval->get_second_interval().begin();
//          for(; qit != new_interval->get_second_interval().end(); ++qit, ++eit)
//      {
//        std::cout << "(" << new_interval->get_first_interval().get_begin()
//            << "," << new_interval->get_first_interval().get_end() << ")";
//        std::cout << "(" << (*qit)->get_begin()
//            << "," << (*qit)->get_end() << ")";
//        std::cout << *eit << "\n";
//      }
//          delete new_interval;
//          // edges_to_test->push_back( new_interval );
//        }
//      else
//        {
//          for( int base( BASE_A ); base < ALPHABET_SIZE; ++base )
//      {
//        BWTPosition new_int_begin = C[ base ] + occ_b[ base ];
//        BWTPosition new_int_end = C[ base ] + occ_e[ base ];
//        if( new_int_end > new_int_begin )
//          {
//            EdgeInterval* new_interval =
//        new EdgeInterval( new_int_begin, new_int_end, i_m->get_second_interval( )[ 0 ]->get_begin( ),
//              i_m->get_second_interval( )[ 0 ]->get_end( ), i_m->get_len( )[ 0 ] +1 );
//            for( unsigned int i( 1 ); i < i_m->get_second_interval( ).size( ); ++i )
//        {
//          new_interval->add_suffix_interval( i_m->get_second_interval( )[ i ],
//                     i_m->get_len( )[ i ] +1 );
//        }
//            ++nwintc;
//            imgr.add_interval( *new_interval, (Nucleotide) base );
//            delete new_interval;
//          } // ~if new_int_end > new_int_begin
//      } // ~for
//        }
//      // delete i_m;
//      i_m = imgr.get_next_interval( );
//    } // ~if i_m comes first than i_l
//       else
//    {
//    }
//     } // ~while interval exists

//   // std::cerr << "--> Sorting RT" << std::endl;
//   // std::sort( edges_to_test->begin( ), edges_to_test->end( ), CompareEdgeIntervalReverse );
// #ifdef DEBUG
//   std::cerr << "--> Generated " << nwintc << " new intervals on BWT'" << std::endl;
//   std::cerr << "--> Merged " << mrgintc << " intervals from lt and from interval manager" << std::endl;
//   std::cerr << "--> Rejected " << rejintc << " intervals from LT " << std::endl;
//   std::cerr << "--> Got " << nwett << " new edges to test."  << std::endl;
// #endif

// }

// bool CompareJoinedQInterval( JoinedQInterval* a, JoinedQInterval* b)
// {
//   return ( a->get_reverse_interval().get_begin() < b->get_reverse_interval().get_begin() );
// }

// bool CompareEdgeInterval( EdgeInterval* a, EdgeInterval* b )
// {
//   return (a->get_first_interval( ).get_begin( ) < b->get_first_interval( ).get_begin( ) );
// }

// bool CompareEdgeIntervalReverse( EdgeInterval* a, EdgeInterval* b )
// {
//   return (!CompareEdgeInterval( a, b ));
// }

// bool EqualSecondEdgeInterval( EdgeInterval* a, EdgeInterval* b )
// {
//   // TODO: second_interval is now a vector
//   return false;
// }

// bool EqualFirstEdgeInterval( EdgeInterval* a, EdgeInterval* b )
// {
//   return (a->get_first_interval( ).get_begin( ) == b->get_first_interval( ).get_begin( ) &&
//           a->get_first_interval( ).get_end( ) == b->get_first_interval( ).get_end( ) );
// }

// bool int_overlap( const QInterval& a, const QInterval& b )
// {
//   return ( ( b.get_begin( ) > a.get_begin( ) && b.get_begin( ) < a.get_end( ) ) ||
//            ( a.get_begin( ) > b.get_begin( ) && a.get_begin( ) < b.get_end( ) ) );
// }

#define _LOG_RECORD											\
  DEBUG_LOG("pos: " << p									\
            << "  c_lcp: " << lcur											\
				<< "  n_lcp: " << *lcp											\
				<< "  gsa: (" << (*gsa).sa << ","							\
				              << (*gsa).numSeq << ")"						\
				)

static void next_record(BWTReader& bwt,
								LCPIterator& lcp,
								GSAIterator& gsa,
								BWTPosition& p,
								LCPValue& lcur,
								LCPValue& lnext
								) {
  ++p;
  lcur= *lcp;
  ++lcp;
  lnext= (lcp == LCPIterator::end()) ? 0 : *lcp;
  ++gsa;
  _LOG_RECORD;
}

struct stack_e_elem_t {
  SequenceNumber numSeq;
  LCPValue k;
  size_t idx_on_stack_$;

  explicit stack_e_elem_t(const SequenceNumber numSeq_,
								  const LCPValue k_,
								  const size_t idx_on_stack_$_)
		:numSeq(numSeq_), k(k_), idx_on_stack_$(idx_on_stack_$_)
  {
  }

};

std::ostream& operator<<(std::ostream& os, const stack_e_elem_t& el) {
  os << "(" << el.numSeq << ", " << el.k << ", " << el.idx_on_stack_$ << ")";
  return os;
}


typedef SequenceNumber stack_$_elem_t;

void build_basic_arc_intervals( BWTReader& bwt,
										  LCPIterator& lcp,
										  GSAIterator& gsa,
										  const SequenceLength& read_length,
										  const SequenceLength& tau,
										  const vector< NucleoCounter >& C,
										  vector< QIntervalManager >& qmgr)
{
  typedef std::stack<stack_e_elem_t, std::vector<stack_e_elem_t> > stack_e_t;
  typedef std::vector<stack_$_elem_t> stack_$_t;

  stack_e_t stack_e;
  stack_$_t stack_$;

  BWTPosition p= 0;  // current position (on LCP/BWT/...)

  LCPValue lcur= *lcp;
  ++lcp;
  LCPValue lnext= (lcp == LCPIterator::end()) ? 0 : *lcp;

  _LOG_RECORD;

  while (lcp != LCPIterator::end()) {
	 DEBUG_LOG("Starting again...");

	 if ((lcur<lnext) && (lnext >= tau)) {
		DEBUG_LOG("Opening a " << lnext << "-superblock.");
		while (gsa != GSAIterator::end() && (*gsa).sa == lnext) {
		  DEBUG_LOG("A " << lnext << "-long suffix of read " << (*gsa).numSeq
						<< " has been found.");
		  stack_e.push(stack_e_elem_t((*gsa).numSeq, lnext, stack_$.size()));
		  DEBUG_LOG("Added element " << stack_e.top() << " to stack_e(" << stack_e.size() << ").");
		  next_record(bwt, lcp, gsa, p, lcur, lnext);
		}
	 }
	 if (((*gsa).sa == read_length) && (!stack_e.empty())) {
		DEBUG_LOG("Found read " << (*gsa).numSeq << " inside a "
					 << stack_e.top().k << "-superblock.");
		stack_$.push_back(stack_$_elem_t((*gsa).numSeq));
		DEBUG_LOG("Added element " << stack_$.back() << " to stack_$(" << stack_$.size() << ").");
	 }
	 if ((lnext<lcur) && (lcur >= tau)) {
		LCPValue mink= std::max(lnext+1, tau);
		DEBUG_LOG("Closing k-superblocks with k in [" << mink << ", " << lcur << "].");
		while(stack_e.top().k >= mink) {
		  DEBUG_LOG("Adding a basic_arc_interval from read " << stack_e.top().numSeq << "...");
		  stack_$_t::const_reverse_iterator end= stack_$.rend()-stack_e.top().idx_on_stack_$;
		  for (stack_$_t::const_reverse_iterator it= stack_$.rbegin();
				 it != end;
				 ++it) {
			 DEBUG_LOG("   ...to read " << *it);
		  }
		  stack_e.pop();
		}
		if (stack_e.empty()) {
		  DEBUG_LOG("Stack_e is empty ==> clearing stack_$.");
		  stack_$.clear();
		}
		next_record(bwt, lcp, gsa, p, lcur, lnext);
	 } else {
		next_record(bwt, lcp, gsa, p, lcur, lnext);
	 }
  }
}

#undef _LOG_RECORD
