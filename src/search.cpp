#include "search.h"

void build_tau_intervals( BWTReader& b, JoinedQIntervalManager& jqmgr, 
			  vector< NucleoCounter >& C, int T)
{
  JoinedQInterval* jqin;
  for( int i( 0 ); i < T; ++i)
    {
      std::cerr << "Building tau intervals - " << i+1 << "/" << TAU << std::endl;
      unsigned long int nwintc =0; // new intervals
      unsigned long int uniquebkwe =0; // unique backward extension
      while( (jqin = jqmgr.get_next_interval()) )
	{
	  // std::cerr << jqin->get_interval( ).get_begin( ) << std::endl
	  // 	    << jqin->get_interval( ).get_end( ) << std::endl;
	  // Get occ at the beginning and end of the interval
	  BWTPosition begin = jqin->get_interval().get_begin();
	  BWTPosition end = jqin->get_interval().get_end();
	  b.move_to( begin );
	  vector< NucleoCounter > occ_begin( b.get_Pi( ) ); 
	  b.move_to( end );
	  vector< NucleoCounter > occ_end( b.get_Pi( ) );

	  for( int base( 1 ); base < ALPHABET_SIZE; ++base )
	    {
	      // Extend the interval with every possibile backward extension
	      BWTPosition new_rev_begin =  jqin->get_reverse_interval().get_begin() +
		(OccLT(occ_end, (Nucleotide) base) - OccLT(occ_begin, (Nucleotide) base));
	      BWTPosition new_rev_end =	new_rev_begin +	(occ_end[ base ] - occ_begin[ base ] );
	      BWTPosition new_begin = C[ base ] + occ_begin[ base ];
	      BWTPosition new_end = C[ base ] + occ_end[ base ];

	      if( new_end - new_begin >= 2 )
		{
		  // Shared substring
		  ++nwintc;
		  JoinedQInterval new_interval ( new_begin, new_end, new_rev_begin, new_rev_end );
		  jqmgr.add_interval( new_interval, (Nucleotide) base );
		}
	      else if ( new_end == new_begin + 1 )
		{
		  // unique backward extension
		  ++uniquebkwe;
		}
	    } // ~for
	  // delete jqin;
	} // ~while
      b.reset();
      jqmgr.swap_files();
#ifdef DEBUG
      std::cerr << "--> Generated " << nwintc << " new intervals." << std::endl;
      std::cerr << "--> Rejected " << uniquebkwe << " unique backward extension"
		<< std::endl;
#endif
    } // ~for
}


vector< EdgeInterval* >* search_step_left( BWTReader& b, JoinedQIntervalManager& jqmgr,
					  vector< NucleoCounter >& C )
{
  JoinedQInterval* jqin;
  unsigned long int nwintc =0; // new intervals counter
  unsigned long int nwltc =0; // new left terminated intervals
  unsigned long int uniquebkwe =0; // unique backward extension
  vector< EdgeInterval* >* LT = new vector< EdgeInterval* >(); // Left-terminated
							     // intervals
  while( (jqin = jqmgr.get_next_interval()) )
    {
      // Get occ at the beginning and end of the interval
      BWTPosition begin = jqin->get_interval().get_begin();
      BWTPosition end = jqin->get_interval().get_end();
      b.move_to( begin );
      vector< NucleoCounter > occ_begin( b.get_Pi( ) );
      b.move_to( end );
      vector< NucleoCounter > occ_end( b.get_Pi( ) );

      // Extend the interval with every possibile backward extension
      for( int base( 0 ); base < ALPHABET_SIZE; ++base )
	{
	  BWTPosition new_rev_begin = jqin->get_reverse_interval().get_begin() +
	    (OccLT(occ_end, (Nucleotide) base) - OccLT(occ_begin, (Nucleotide) base));
	  BWTPosition new_rev_end = new_rev_begin + (occ_end[ base ] - occ_begin[ base ] );
	  BWTPosition new_begin = C[ base ] + occ_begin[ base ];
	  BWTPosition new_end = C[ base ] + occ_end[ base ];

	  if( ( base != BASE_$ ) && ( new_end - new_begin >= 2 ) )
	    {
	      // New shared substring
	      ++nwintc;
	      JoinedQInterval new_interval ( new_begin, new_end, new_rev_begin, new_rev_end );
	      jqmgr.add_interval( new_interval, (Nucleotide) base );
	    }

	  else if ( ( base == BASE_$ ) && ( new_rev_end - new_rev_begin >= 1 ) )
	    {
	      // New left-terminated interval
	      ++nwltc;
	      EdgeInterval* new_interval =
		new EdgeInterval ( new_rev_begin, new_rev_end, 
				   jqin->get_reverse_interval( ).get_begin( ),
				   jqin->get_reverse_interval( ).get_end( ), 
				   0 );
	      LT->push_back( new_interval );
	    }

	  else if ( new_end - new_begin == 1 )
	    {
	      // Unique backward extension (don't care)
	      ++uniquebkwe;
	    }
	} // ~for
      // delete jqin;
    } // ~while

#ifdef DEBUG
  std::cerr << "--> Generated " << nwintc << " new intervals." << std::endl;
  std::cerr << "--> Generated " << nwltc << " new left-terminated intervals."
	    << std::endl;
  std::cerr << "--> Rejected " << uniquebkwe << " unique backward extension"
	    << std::endl;
#endif

  std::cerr << "--> Sorting LT" << std::endl;
  std::sort( LT->begin(), LT->end(), CompareEdgeIntervalReverse );
  return LT;
}

vector< EdgeInterval* >* search_step_right( BWTReader& b, EdgeJoinedQIntervalManager& imgr, 
					   vector< NucleoCounter >& C, vector< EdgeInterval* >* LT )
{
  // LETS TRY IT AGAIN DAMMIT

  vector< EdgeInterval* >* edges_to_test = new vector< EdgeInterval* >( );
  EdgeInterval* i_l = NULL; // interval from LT
  EdgeInterval* i_m = imgr.get_next_interval( ); // interval from imgr

  unsigned long int nwintc =0; // new interval counter
  unsigned long int rejintc =0; // rejected LT interval counter
  unsigned long int mrgintc =0; // intervals from imgr merged with the ones from LT

  // read first interval from LT if it exists
  if( LT->size( ) > 0 )
    {
      i_l = LT->back( );
      LT->pop_back( );
    }

  while( ( i_l != NULL ) || ( i_m != NULL ) )
    {
      bool merged = false;
      while( ( i_l != NULL ) && ( i_m != NULL ) && EqualFirstEdgeInterval( i_l, i_m ) )
      	{
      	  // Need to merge i_lt and i_mgr because the first interval (the one we
      	  // need to extend) is the same. Note that we don't know if i_lt is
      	  // right-terminal as for now
      	  ++mrgintc;
	  merged = true;
      	  for( unsigned int i( 0 ); i < i_m->get_second_interval( ).size( ); ++i )
      	    {
	      i_l->add_suffix_interval( i_m->get_second_interval( )[ i ],
					i_m->get_len( )[ i ] );
      	    }
      	  // delete i_m;
      	  i_m = imgr.get_next_interval( );
      	} // ~while merge

      if( ( i_l != NULL ) && ( ( i_m == NULL ) || CompareEdgeInterval( i_l, i_m ) ) )
	{
	  // Check if the new interval that comes from the left search is right
	  // terminated. If it is extend and save to imgr, if it isn't check if
	  // we have merged elements and extend them rejecting the new interval.
	  BWTPosition begin_f_i = i_l->get_first_interval( ).get_begin( );    // begin of Q'$-interval (same as begin Q'-interval)
	  BWTPosition end_f_i = i_l->get_first_interval( ).get_end( );        // end of Q'$-interval
	  BWTPosition end_s_i = i_l->get_second_interval( )[ 0 ]->get_end( ); // end of Q'-interval

	  b.move_to( begin_f_i );
	  vector< NucleoCounter > occ_b( b.get_Pi( ) );
	  b.move_to( end_f_i );
	  vector< NucleoCounter > occ_e_f( b.get_Pi( ) );
	  b.move_to( end_s_i );
	  vector< NucleoCounter > occ_e( b.get_Pi( ) );

	  bool new_interval_is_right_terminal = ( occ_e[ BASE_$ ] > occ_e_f[ BASE_$ ] ) ? true : false ;

	  if( !new_interval_is_right_terminal )
	      ++rejintc;
	  if( new_interval_is_right_terminal || merged )
	    {
	      for( int base( BASE_$ ); base < ALPHABET_SIZE; ++base )
		{
		  BWTPosition new_int_begin = C[ base ] + occ_b[ base ];
		  BWTPosition new_int_end = C[ base ] + occ_e_f[ base ];
		  if( new_int_end > new_int_begin )
		    {
		      BWTPosition first_suffix_begin = new_interval_is_right_terminal ? 
			occ_e_f[ BASE_$ ] : i_l->get_second_interval( )[ 1 ]->get_begin( );
		      BWTPosition first_suffix_end = new_interval_is_right_terminal ?
			occ_e[ BASE_$ ] : i_l->get_second_interval( )[ 1 ]->get_end( );
		      EdgeInterval* new_interval =
			new EdgeInterval( new_int_begin, new_int_end, first_suffix_begin,
					  first_suffix_end, new_interval_is_right_terminal ? 
					  1 : i_l->get_len( )[ 1 ]+1 );
		      // Add other suffix if they exists
		      for( unsigned int i( new_interval_is_right_terminal ? 1 : 2 ); 
			   i < i_l->get_second_interval( ).size( ); ++i )
			new_interval->add_suffix_interval( i_l->get_second_interval( )[ i ],
							   i_l->get_len( )[ i ] +1 );
		      if( base == BASE_$ )
			{
			  edges_to_test->push_back( new_interval );
			}
		      else
			{
			  ++nwintc;
			  imgr.add_interval( *new_interval, (Nucleotide) base );
			  delete new_interval;
			}
		    }
		  else
		    {
		      // Non existent intervals, don't care
		    } // ~if new_int_end > new_int_begin
		} // ~for base
	    }

	  delete i_l;
	  if( LT->size( ) > 0 )
	    {
	      i_l = LT->back( );
	      LT->pop_back( );
	    }
	  else
	    i_l = NULL;
	} // ~if i_l comes first than i_m
      else if( ( i_m != NULL ) && ( ( i_l == NULL ) || CompareEdgeInterval( i_m, i_l ) ) )
	{
	  // Extend the interval that comes from the interval manager. If it
	  // extend with $ append it to edges_to_test
	  BWTPosition begin_f = i_m->get_first_interval( ).get_begin( );  // begin of Q'$-interval (same as begin Q'-interval)
	  BWTPosition end_f = i_m->get_first_interval( ).get_end( );      // end of Q'$-interval

	  b.move_to( begin_f );
	  vector< NucleoCounter > occ_b( b.get_Pi( ) );
	  b.move_to( end_f );
	  vector< NucleoCounter > occ_e( b.get_Pi( ) );

	  if( occ_e[ BASE_$ ] > occ_b[ BASE_$ ] )
	    {
	      BWTPosition new_int_begin = occ_b[ BASE_$ ];
	      BWTPosition new_int_end = occ_e[ BASE_$ ];
	      EdgeInterval* new_interval =
		new EdgeInterval( new_int_begin, new_int_end, i_m->get_second_interval( )[ 0 ]->get_begin( ),
				  i_m->get_second_interval( )[ 0 ]->get_end( ), i_m->get_len( )[ 0 ] );
	      for( unsigned int i( 1 ); i < i_m->get_second_interval( ).size( ); ++i )
		{
		  new_interval->add_suffix_interval( i_m->get_second_interval( )[ i ],
						     i_m->get_len( )[ i ] );
		}
	      edges_to_test->push_back( new_interval );
	    }
	  else
	    {
	      for( int base( BASE_A ); base < ALPHABET_SIZE; ++base )
		{
		  BWTPosition new_int_begin = C[ base ] + occ_b[ base ];
		  BWTPosition new_int_end = C[ base ] + occ_e[ base ];
		  if( new_int_end > new_int_begin )
		    {
		      EdgeInterval* new_interval =
			new EdgeInterval( new_int_begin, new_int_end, i_m->get_second_interval( )[ 0 ]->get_begin( ),
					  i_m->get_second_interval( )[ 0 ]->get_end( ), i_m->get_len( )[ 0 ] +1 );
		      for( unsigned int i( 1 ); i < i_m->get_second_interval( ).size( ); ++i )
			{
			  new_interval->add_suffix_interval( i_m->get_second_interval( )[ i ],
							     i_m->get_len( )[ i ] +1 );
			}
		      ++nwintc;
		      imgr.add_interval( *new_interval, (Nucleotide) base );
		      delete new_interval;
		    } // ~if new_int_end > new_int_begin
		} // ~for
	    }	  
	  // delete i_m;
	  i_m = imgr.get_next_interval( );
	} // ~if i_m comes first than i_l
      else
	{
	  std::cerr << "ERROR: This shouldn't happen. " << std::endl;
	}
    } // ~while interval exists

  std::cerr << "--> Sorting RT" << std::endl;
  std::sort( edges_to_test->begin( ), edges_to_test->end( ), CompareEdgeIntervalReverse );
#ifdef DEBUG
  std::cerr << "--> Generated " << nwintc << " new intervals on BWT'" << std::endl;
  std::cerr << "--> Merged " << mrgintc << " intervals from lt and from interval manager" << std::endl;
  std::cerr << "--> Rejected " << rejintc << " intervals from LT " << std::endl;
  std::cerr << "--> Got " << edges_to_test->size( ) << " new intervals on GSA to test." 
	    << std::endl;
#endif

  return edges_to_test;
}

BWTPosition OccLT( vector< NucleoCounter >& occ, Nucleotide base )
{
  // Count occurrences of bases lexicographically less than base given occ
  BWTPosition p =0;

  for(unsigned int i( 0 ); i < (unsigned int) base; ++i )
    p += (BWTPosition) occ[ i ];

  return p;
}

bool CompareJoinedQInterval( JoinedQInterval* a, JoinedQInterval* b)
{
  return ( a->get_reverse_interval().get_begin() < b->get_reverse_interval().get_begin() );
}

bool CompareEdgeInterval( EdgeInterval* a, EdgeInterval* b )
{
  return (a->get_first_interval( ).get_begin( ) < b->get_first_interval( ).get_begin( ) );
}

bool CompareEdgeIntervalReverse( EdgeInterval* a, EdgeInterval* b )
{
  return (!CompareEdgeInterval( a, b ));
}

bool EqualSecondEdgeInterval( EdgeInterval* a, EdgeInterval* b )
{
  // TODO: second_interval is now a vector
  return false;
}

bool EqualFirstEdgeInterval( EdgeInterval* a, EdgeInterval* b )
{
  return (a->get_first_interval( ).get_begin( ) == b->get_first_interval( ).get_begin( ) &&
	  a->get_first_interval( ).get_end( ) == b->get_first_interval( ).get_end( ) );
}

bool int_overlap( const QInterval& a, const QInterval& b )
{
  return ( ( b.get_begin( ) > a.get_begin( ) && b.get_begin( ) < a.get_end( ) ) ||
	   ( a.get_begin( ) > b.get_begin( ) && a.get_begin( ) < b.get_end( ) ) );
}
