/**
 *
 *                     LightStringGraph
 *
 *            Lightweight String Graph Construction.
 *
 * Copyright (C) 2013, 2014 Stefano Beretta, Yuri Pirola, Marco Previtali
 *
 * Distributed under the terms of the GNU General Public License (or the Lesser
 * GPL).
 *
 * This file is part of LightStringGraph.
 *
 * LighStringGraph is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * LightStringGraph is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/
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
              _MY_FAIL;
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
          _MY_FAIL;
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
  DEBUG_LOG("  pos: " << p									\
				<< "  sigma: "	<< NuclConv::ntoc((Nucleotide)Ci)		\
            << "  c_lcp: " << lcur											\
				<< "  n_lcp: " << lnext											\
				<< "  bwt: " << (use_bwt?*bwt:'.')							\
				<< "  gsa: (" << (*gsa).sa << ","							\
				              << (*gsa).numSeq << ")"						\
				)


static void next_record(BWTIterator& bwt,
								LCPIterator& lcp,
								GSAIterator& gsa,
								BWTPosition& p,
								LCPValue& lcur,
								LCPValue& lnext,
								SequenceLength& max_len,
								const vector< NucleoCounter >& C,
								vector< NucleoCounter >::size_type& Ci,
								const bool use_bwt
								) {
  ++p;    // Position
  ++gsa;  // Generalized Suffix Array
  if (gsa==GSAIterator::end())
	 return;
  max_len= std::max(max_len, (*gsa).sa);
  if (use_bwt) ++bwt;  // BWT (if used)

  // LCP (current and next)
  lcur= *lcp;
  ++lcp;
  lnext= (lcp == LCPIterator::end()) ? 0 : *lcp;

  // starting symbol
  while ((Ci + 1 < C.size()) && (p >= C[Ci+1])) ++Ci;

  // print to log
  _LOG_RECORD;
}

struct stack_e_elem_t {
  BWTPosition b;
  BWTPosition e;
  LCPValue k;
  PrefixManager::position_t pos_on_pref_mgr;
  PrefixManager::offset_t size_of_pref_mgr;

  explicit stack_e_elem_t(const BWTPosition b_,
								  const BWTPosition e_,
								  const LCPValue k_,
								  const PrefixManager::position_t pos_on_pref_mgr_,
								  const PrefixManager::offset_t size_of_pref_mgr_)
		:b(b_), e(e_), k(k_), pos_on_pref_mgr(pos_on_pref_mgr_),
		 size_of_pref_mgr(size_of_pref_mgr_)
  {
  }

};

std::ostream& operator<<(std::ostream& os, const stack_e_elem_t& el) {
  os << "( [" << el.b << ", " << el.e << "), " << el.k << ", " << el.pos_on_pref_mgr << ")";
  return os;
}


SequenceLength build_basic_arc_intervals( BWTIterator& bwt,
                                          LCPIterator& lcp,
                                          GSAIterator& gsa,
                                          PrefixManager& pref_mgr,
                                          const SequenceLength& read_length,
                                          const SequenceLength& tau,
                                          const vector< NucleoCounter >& C,
                                          BasicArcIntervalManager& baimgr)
{
  typedef std::stack<stack_e_elem_t, std::vector<stack_e_elem_t> > stack_e_t;

  const bool use_bwt= (read_length == 0);

  stack_e_t stack_e;

  BWTPosition p= 0;  // current position (on LCP/BWT/...)

  LCPValue lcur= *lcp;
  ++lcp;
  LCPValue lnext= (lcp == LCPIterator::end()) ? 0 : *lcp;

  vector< NucleoCounter >::size_type Ci= 0;

  SequenceLength max_len = 0; // Max length found

  _LOG_RECORD;

  bool opening_block= false;
  BWTPosition ob_b= 0, ob_e= 0;
  LCPValue suff_len= 0;
  PrefixManager::position_t pos_on_pref_mgr= 0;
  PrefixManager::offset_t size_of_pref_mgr= 0;
  while (gsa != GSAIterator::end()) {
    if (opening_block && (((*gsa).sa != suff_len) || (lcur != suff_len))) {
      opening_block= false;
      ob_e= p;
      _FAIL_IF(ob_b >= ob_e);
      stack_e.push(stack_e_elem_t(ob_b, ob_e, suff_len, pos_on_pref_mgr, size_of_pref_mgr));
      DEBUG_LOG("  Added element " << stack_e.top() << " to stack_e(" << stack_e.size() << ").");
    }

// Closing some superblocks
	 while(!stack_e.empty() && (stack_e.top().k > lcur || (lcp == LCPIterator::end()))) {
		DEBUG_LOG("Closing a " << stack_e.top().k << "-superblock.");
		if (stack_e.top().size_of_pref_mgr < pref_mgr.size()) {
		  DEBUG_LOG("Adding the basic_arc_interval "
						<< "( [ " << stack_e.top().b << ", " << stack_e.top().e << "), "
						<< "[ " << stack_e.top().size_of_pref_mgr << ", " << pref_mgr.size() << "), "
						<< "0) to file "
						<< "E^0( " << NuclConv::ntoc((Nucleotide)Ci) << ", " << (stack_e.top().k+1) << ").");
// Add the basic_arc_interval to the interval manager with sigma=Ci and Length=stack_e.top().k+1
// (Note: '+1' is because '$' was not considered.)
		  baimgr[stack_e.top().k+1].													\
			 add_interval(ArcInterval(QInterval(stack_e.top().b, stack_e.top().e),
											  0,
											  SeedInterval(stack_e.top().pos_on_pref_mgr,
																pref_mgr.size()-stack_e.top().size_of_pref_mgr)),
							  (Nucleotide)Ci);
		}
		stack_e.pop();
	 }

	 // Found a new prefix
    if (//NOTE: condition '!stack-e.empty()' has been removed since we want that
        //the "prefix"-file contain the lexicographical order of ALL the reads, and not only
        //the ones "involved" in some superblocks.
        //(!stack_e.empty()) &&         // some supeblock is active AND
        ((use_bwt && *bwt == '$') ||  // ( using bwt and the bwt symbol is $  OR
         ((*gsa).sa == read_length) ) //   elem of GSA is a "complete read" )
        ) {
      pref_mgr.push(PrefixManager::elem_t((*gsa).numSeq));
		DEBUG_LOG("  Read " << (*gsa).numSeq << " is the " << pref_mgr.size()
					 << "-th read of the lexicographic order.");
    }


// Opening a superblock
	 if ((lcur<lnext) && (lnext >= tau) && (*gsa).sa == lnext) {
		DEBUG_LOG("Possibly opening a " << lnext << "-superblock at position " << p << ".");
		opening_block= true;
		ob_b= p;
		suff_len= lnext;
		pos_on_pref_mgr= pref_mgr.position();
		size_of_pref_mgr= pref_mgr.size();
	 }

	 next_record(bwt, lcp, gsa, p, lcur, lnext, max_len, C, Ci, use_bwt);
  }
  baimgr.swap_all_files();
  pref_mgr.flush();

  return max_len;
}

#undef _LOG_RECORD

bool equalFirstInterval( const QInterval& a, const QInterval& b )
{
  return (a.get_begin() == b.get_begin() &&
          a.get_end() == b.get_end() );
}

void extend_arc_intervals( const int length,
                           const vector< NucleoCounter >& C,
                           BWTReader& br,
                           GSAIterator& gsait,
                           SameLengthArcIntervalManager& qmgr,
                           SameLengthArcIntervalManager& newqmgr,
                           ExtendSymbolPile& extsym_p,
                           EdgeLabelIntervalManager& edlblmgr,
                           PrefixManager& pref_mgr,
                           OutputMultiFileManager& arcsOut )
{
  unsigned long int nwintc =0;     // New produced intervals
  unsigned long int nwarcs  =0;    // New arcs
  unsigned long int nwintmgr=0;    // Intrvals read from qmgr
  unsigned long int nwintnwmgr =0; // Intervals from neqmgr

  ArcInterval *qint, *newqint;
  qint = qmgr.get_next_interval( );
  newqint = newqmgr.get_next_interval( );

  QInterval lastInterval(0,0);

  vector< NucleoCounter > PI;
  vector< NucleoCounter > pi;

  ReadSet dst_reads;

  while((qint != NULL) || (newqint != NULL))
    {
      ArcInterval         *currentInterval;
      bool                 is_new_interval = false;
      bool                 from_qmgr = true;
      vector< Nucleotide > extendsymbols;

      if((newqint == NULL && qint != NULL) ||
         (qint != NULL && qint->es_interval.get_begin() < newqint->es_interval.get_begin()))
        {
          currentInterval = qint;
          from_qmgr = true;
          ++nwintmgr;
        }
      else
        {
          currentInterval = newqint;
          is_new_interval = true;
          from_qmgr = false;
          ++nwintnwmgr;
        }

      DEBUG_LOG("Read Interval [" << currentInterval->es_interval.get_begin()
                << "," << currentInterval->es_interval.get_end() << ") from_qmgr?" << from_qmgr);

      if(currentInterval->es_interval != lastInterval)
        {
          // New qinterval
          lastInterval = currentInterval->es_interval;
          dst_reads.clear();
          for(BWTPosition i(br.get_position()); i <= currentInterval->es_interval.get_begin(); ++i)
              br.move_to( i );

          PI = vector< NucleoCounter >( br.get_Pi() );

          while(gsait.get_position() < currentInterval->es_interval.get_begin())
              ++gsait;

          for(BWTPosition currentPosition=currentInterval->es_interval.get_begin();
              currentPosition < currentInterval->es_interval.get_end();
              ++currentPosition)
            {
              br.move_to(currentPosition);
              if(br.get_current_nucleotide() == BASE_$)
                dst_reads.push_back((*gsait).numSeq);
              if(gsait != GSAIterator::end()) ++gsait;
            }
          br.move_to(currentInterval->es_interval.get_end());
        }

      pi = vector< NucleoCounter >( br.get_Pi() );

      if(!dst_reads.empty() && currentInterval->ext_len>0)
        {
          // TODO: Output Prefix and Suffix in a more meaningful way
          BWTPosition begin$pos = PI[BASE_$], end$pos = pi[BASE_$];

          arcsOut[currentInterval->ext_len].write(reinterpret_cast<char*>(&begin$pos),
                                                  sizeof(BWTPosition));
          arcsOut[currentInterval->ext_len].write(reinterpret_cast<char*>(&end$pos),
                                                  sizeof(BWTPosition));
          arcsOut[currentInterval->ext_len].write(reinterpret_cast<const char*>(&(currentInterval->seed_int.begin)),
                                                  sizeof(SequenceNumber));
          SequenceNumber seedend = currentInterval->seed_int.begin + currentInterval->seed_int.end;
          arcsOut[currentInterval->ext_len].write(reinterpret_cast<const char*>(&(seedend)),
                                                  sizeof(SequenceNumber));
          // std::cout << "EXT_LEN " << currentInterval->ext_len << "\t";
          // for(ReadSet::const_iterator read = dst_reads.begin();
          //     read != dst_reads.end(); ++read)
          //     std::cout << *read << " ";
          // std::cout << "-> ";
          // std::vector< PrefixManager::elem_t > seqOriginID;
          // pref_mgr.get_elems( currentInterval->seed_int.begin,
          //                     currentInterval->seed_int.end,
          //                     seqOriginID );
          // for(std::vector< PrefixManager::elem_t >::iterator it = seqOriginID.begin();
          //     it != seqOriginID.end(); ++it)
          //   {
          //     std::cout << *it << " ";
          //   }
          // std::cout << std::endl;
          ++nwarcs;
          DEBUG_LOG("Add BASE_$");
          extendsymbols.push_back( BASE_$ );
        }
      for(int base(BASE_A); base < ALPHABET_SIZE; ++base)
        {
          BWTPosition new_begin = C[ base ] + PI[ base ];
          BWTPosition new_end   = C[ base ] + pi[ base ];
          if(new_end > new_begin)
            {
              ++nwintc;
              DEBUG_LOG("Add BASE_" << NuclConv::ntoc((Nucleotide)base));
              extendsymbols.push_back( (Nucleotide) base );
              if(is_new_interval)
                {
                  // Add intervals of length 0 in a meaninfull way
                  QInterval whole_bwt(0, br.size( ));
                  EdgeLabelInterval void_interval(whole_bwt, whole_bwt);
                  // BASE_A acts as a void base if we add every possible intervals to that file
                  edlblmgr.add_edge_interval( void_interval, 0, BASE_A );
                  // Add the interval only once
                  is_new_interval = false;
                  DEBUG_LOG("Add interval [0," << br.size() << ") to edgelabelmanager");
                }
              QInterval new_q_interval(new_begin, new_end);
              ArcInterval new_arc_interval(new_q_interval,
                                           currentInterval->ext_len +1,
                                           currentInterval->seed_int);
              DEBUG_LOG("Add interval [" << new_begin << "," << new_end
                        << ") to qmgr with ext_len = "
                        << currentInterval->ext_len +1);
              qmgr.add_interval(new_arc_interval, (Nucleotide)base);
            }
        }

      if(extendsymbols.size() > 0)
          extsym_p.add_extend_symbol(extendsymbols, currentInterval->ext_len);

      from_qmgr ? qint = qmgr.get_next_interval() : newqint = newqmgr.get_next_interval();
    }
  DEBUG_LOG( "--> Got " << nwintmgr << " new intervals from qmgr" );
  DEBUG_LOG( "--> Got " << nwintnwmgr << " new intervals from nwqmgr" );
  std::cerr << "--> Generated " << nwintc << " new intervals" << std::endl;
  std::cerr << "--> Produced " << nwarcs << " new edges (still not labeled)" << std::endl;
}

void extend_arc_labels( EdgeLabelIntervalManager& edgemgr,
                        ExtendSymbolPile& extsym_p,
                        const vector< NucleoCounter >& C,
                        BWTReader& br,
                        GSAIterator& gsait,
                        LCPIterator& lcp,
                        const SequenceLength max_len,
                        OutputMultiFileManager& edgeOut )
{
  struct EPI_t EPI(max_len +1);
  vector< int > EPI_p(max_len +1, -1);
  EPI_p[0] = 0;
  // vector< vector< NucleoCounter > > EPI(max_len+1, vector< NucleoCounter >(ALPHABET_SIZE));
  struct EdgeLabelEntry currentEdge;
  LCPValue lcur = (lcp == LCPIterator::end()) ? 0 : *lcp;
  ++lcp;
  LCPValue lnext = (lcp == LCPIterator::end()) ? 0 : *lcp;
  unsigned long nwtermlbc =0;
  unsigned long nwlbc =0;
  vector< NucleoCounter > pi(ALPHABET_SIZE);

  br.move_to(0);
  edgemgr.get_next_interval( currentEdge );

  while(true) // TODO: find a better condition
    {
      DEBUG_LOG("position       : " << lcp.get_position()-1);
      DEBUG_LOG("lcur           : " << lcur);
      DEBUG_LOG("lnext          : " << lnext);
      DEBUG_LOG("currentInterval: " << currentEdge._interval.get_label().get_begin()
                << ", " << currentEdge._interval.get_label().get_end());
      if(lcur<lnext)                         // potential open
        {
          std::copy(br.get_Pi().begin(), br.get_Pi().end(), EPI._occs[EPI._next].begin());
          for(LCPValue l=lnext; l>lcur; --l)
            {
              DEBUG_LOG("Build EPI[" << l << "]");
              EPI_p[l] = EPI._next;
              // std::copy(br.get_Pi().begin(), br.get_Pi().end(), EPI[l].begin());
            }
          ++EPI._next;
        }
      br.move_to(lcp.get_position());

      while(lcp.get_position() == currentEdge._interval.get_label().get_end())
        {
          // Close and extend intervals
          DEBUG_LOG("Current edgeLabelInterval is ["
                    << currentEdge._interval.get_label().get_begin() << ","
                    << currentEdge._interval.get_label().get_end() << ")"
                    << " and the reverse interval is ["
                    << currentEdge._interval.get_reverse_label().get_begin() << ","
                    << currentEdge._interval.get_reverse_label().get_end() << ")");
          const vector< Nucleotide >& extend_symbols = extsym_p.get_next_symbol(currentEdge._len);
          std::copy(br.get_Pi().begin(), br.get_Pi().end(), pi.begin());
          const bool special_interval = (currentEdge._interval.get_label().get_size() == 1);

          for(vector< Nucleotide >::const_iterator nucl_i =extend_symbols.begin();
              nucl_i != extend_symbols.end(); ++nucl_i)
            {
              DEBUG_LOG("Extending with " << *nucl_i << NuclConv::ntoc(*nucl_i));
              _FAIL_IF(*nucl_i > BASE_T);
              Nucleotide extension = *nucl_i;
              // Output if finished arc
              if(extension == BASE_$)
                {
                  // Compiler does not accept a function as a argument for '&' ?
                  BWTPosition labbegin = currentEdge._interval.get_reverse_label().get_begin();
                  BWTPosition labend   = currentEdge._interval.get_reverse_label().get_end();
                  edgeOut[currentEdge._len].write(reinterpret_cast<char*>(&labbegin),
                                                  sizeof(BWTPosition));
                  edgeOut[currentEdge._len].write(reinterpret_cast<char*>(&labend),
                                                  sizeof(BWTPosition));
                  ++nwtermlbc;
                }
              // Extend otherwise
              else
                {
                  BWTPosition new_begin;
                  if(special_interval) new_begin = C[ extension ] + pi[ extension ] -1;
                  else new_begin = C[extension] + EPI._occs[EPI_p[currentEdge._len]][extension];

                  BWTPosition new_end = C[ extension ] + pi[ extension ];
                  BWTPosition old_rev_begin = currentEdge._interval.get_reverse_label().get_begin();
                  BWTPosition new_rev_begin, new_rev_end;
                  if(currentEdge._len == 0)
                    {
                      new_rev_begin = C[extension];
                      new_rev_end = C[extension+1];
                    }
                  else
                    {
                      if(special_interval)
                        {
                          new_rev_begin = old_rev_begin;
                          new_rev_end = old_rev_begin+1;
                        }
                      else
                        {
                          new_rev_begin = old_rev_begin + OccLT(pi, extension) - OccLT(EPI._occs[EPI_p[currentEdge._len]], extension);
                          new_rev_end = new_rev_begin + pi[extension] - EPI._occs[EPI_p[currentEdge._len]][extension];
                        }
                    }
                  EdgeLabelInterval new_interval(QInterval(new_begin, new_end), QInterval(new_rev_begin, new_rev_end));
                  ++nwlbc;

                  DEBUG_LOG("Extended to [" << new_begin << "," << new_end << ")" << " and the reverse interval is ["
                            << new_rev_begin << "," << new_rev_end << ")");

                  edgemgr.add_edge_interval(new_interval, currentEdge._len +1, extension);
                }
            }
          if(!edgemgr.get_next_interval( currentEdge ))
            break;
        }// ~while interval ends in this position

      if(lnext<lcur)                          // potential close
        {
          for(LCPValue l=lcur; l>lnext; --l)
            {
              DEBUG_LOG("Clear EPI[" << l << "]");
              EPI_p[l] = -1;
              // EPI[l].clear();
            }
          EPI._next = EPI_p[lnext]+1;
        }
      lcur = lnext;
      if(lcp == LCPIterator::end())
        break;
      ++lcp;
      lnext = (lcp == LCPIterator::end()) ? 0 : *lcp;
    } // ~while true

  std::cerr << "--> Generated " << nwlbc << " new labels for arcs." << std::endl;
  std::cerr << "--> Labeled " << nwtermlbc << " new arcs." << std::endl;
}

// static void extend_arc_labels_old( EdgeLabelIntervalManager& edgemgr,
//                         ExtendSymbolPile& extsym_p,
//                         const vector< NucleoCounter >& C,
//                         BWTReader& br,
//                         GSAIterator& gsait,
//                         LCPIterator& lcp,
//                         const SequenceLength max_len,
//                         OutputMultiFileManager& edgeOut )
// {
//   vector< vector< NucleoCounter > > EPI(max_len+1, vector< NucleoCounter >(ALPHABET_SIZE));
//   struct EdgeLabelEntry currentEdge;
//   BWTPosition currentPosition=0;
//   LCPValue lcur = 0;
//   LCPValue lnext = 0;
//   EdgeLabelInterval lastInterval( QInterval(0,0), QInterval(0,0) );
//   unsigned long nwtermlbc =0;
//   unsigned long nwlbc =0;

//   vector< NucleoCounter > pi(ALPHABET_SIZE);
//   while(edgemgr.get_next_interval( currentEdge ))
//     {
//       DEBUG_LOG("Current edgeLabelInterval is ["
//                 << currentEdge._interval.get_label().get_begin() << ","
//                 << currentEdge._interval.get_label().get_end() << ")"
//                 << " and the reverse interval is ["
//                 << currentEdge._interval.get_reverse_label().get_begin() << ","
//                 << currentEdge._interval.get_reverse_label().get_end() << ")");
//       const vector< Nucleotide >& extend_symbols = extsym_p.get_next_symbol(currentEdge._len);
//       // Move to begin updating EPI
//       for(; currentPosition <= currentEdge._interval.get_label().get_begin(); ++currentPosition)
//         {
//           lcur = *lcp;
//           br.move_to(currentPosition);
//           ++lcp;
//           lnext = (lcp == LCPIterator::end()) ? 0 : *lcp;
//           if(lcur<lnext)
//             for(LCPValue l=lnext; l>lcur; --l)
//               {
//                 DEBUG_LOG("Build EPI[" << l << "]");
// 					 std::copy(br.get_Pi().begin(), br.get_Pi().end(), EPI[l].begin());
//               }
//           else if(lnext<lcur)
//             for(LCPValue l=lcur; l>lnext; --l)
//               {
//                 DEBUG_LOG("Clear EPI[" << l << "]");
// //                EPI[l].clear();
//               }
//         }
//       // moved to begin
//       const bool special_interval = (currentEdge._interval.get_reverse_label().get_size() == 1);

//       // intervals of size 1 wont withstand to LCP rule
//       if(lastInterval != currentEdge._interval)
//         {
//           for(; currentPosition <= currentEdge._interval.get_label().get_end(); ++currentPosition)
//             {
//               br.move_to(currentPosition);
//               if(lcp != LCPIterator::end())
//                 {
//                   lcur = *lcp;
//                   ++lcp;
//                 }
//               lnext = (lcp == LCPIterator::end()) ? 0 : *lcp;
//             }
//         }

// 		std::copy(br.get_Pi().begin(), br.get_Pi().end(), pi.begin());

//       for(vector< Nucleotide >::const_iterator nucl_i =extend_symbols.begin();
//           nucl_i != extend_symbols.end(); ++nucl_i)
//         {
//           DEBUG_LOG("Extending with " << *nucl_i);
//           Nucleotide extension = *nucl_i;
//           // Output if finished arc
//           if(extension == BASE_$)
//             {
//               // Compiler does not accept a function as a argument for '&' ?
//               BWTPosition labbegin = currentEdge._interval.get_reverse_label().get_begin();
//               BWTPosition labend   = currentEdge._interval.get_reverse_label().get_end();
//               edgeOut[currentEdge._len].write(reinterpret_cast<char*>(&labbegin),
//                                               sizeof(BWTPosition));
//               edgeOut[currentEdge._len].write(reinterpret_cast<char*>(&labend),
//                                               sizeof(BWTPosition));
//               ++nwtermlbc;
//               // std::cout << "EXT_LEN " << currentEdge._len << "\t"
//               //           << currentEdge._interval.get_reverse_label().get_begin()
//               //           << ","
//               //           << currentEdge._interval.get_reverse_label().get_end()
//               //           << std::endl;
//             }
//           // Extend otherwise
//           else
//             {
//               BWTPosition new_begin;
//               if(special_interval) new_begin = C[ extension ] + pi[ extension ] -1;
//               else new_begin = C[extension] + EPI[currentEdge._len][extension];

//               BWTPosition new_end = C[ extension ] + pi[ extension ];
//               BWTPosition old_rev_begin = currentEdge._interval.get_reverse_label().get_begin();
//               BWTPosition new_rev_begin, new_rev_end;
//               if(currentEdge._len == 0)
//                 {
//                   new_rev_begin = C[extension];
//                   new_rev_end = C[extension+1];
//                 }
//               else
//                 {
//                   if(special_interval)
//                     {
//                       new_rev_begin = old_rev_begin;
//                       new_rev_end = old_rev_begin +1;
//                     }
//                   else
//                     {
//                       new_rev_begin = old_rev_begin + OccLT(pi, extension) - OccLT(EPI[currentEdge._len], extension);
//                       new_rev_end = new_rev_begin + pi[extension] - EPI[currentEdge._len][extension];
//                     }
//                 }
//               EdgeLabelInterval new_interval(QInterval(new_begin, new_end), QInterval(new_rev_begin, new_rev_end));
//               ++nwlbc;
//               DEBUG_LOG("Extended to ["
//                         << new_begin << ","
//                         << new_end << ")"
//                         << " and the reverse interval is ["
//                         << new_rev_begin << ","
//                         << new_rev_end << ")");
//               edgemgr.add_edge_interval(new_interval, currentEdge._len +1, extension);
//             }
//         }
//       lastInterval = currentEdge._interval;
//     } // ~while get next interval
//   std::cerr << "--> Generated " << nwlbc << " new labels for arcs." << std::endl;
//   std::cerr << "--> Labeled " << nwtermlbc << " new arcs." << std::endl;
// }
