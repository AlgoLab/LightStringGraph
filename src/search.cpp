#include "search.h"

void build_tau_intervals( BWTReader& b, JoinedQIntervalManager& jqmgr, 
			  vector< NucleoCounter >& C, int T)
{
  JoinedQInterval* jqin;
  for( int i( 0 ); i < T; ++i)
    {
      std::cout << "Building tau intervals - " << i+1 << "/" << TAU << std::endl;
      unsigned long int nwintc =0; // new intervals
      unsigned long int uniquebkwe =0; // unique backward extension
      while( (jqin = jqmgr.get_next_interval()) )
	{
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
	} // ~while
      b.reset();
      jqmgr.swap_files();
#ifdef DEBUG
      std::cout << "--> Generated " << nwintc << " new intervals." << std::endl;
      std::cout << "--> Rejected " << uniquebkwe << " unique backward extension"
		<< std::endl;
#endif
    } // ~for
}


deque< EdgeInterval* >* search_step_left( BWTReader& b, JoinedQIntervalManager& jqmgr,
					  vector< NucleoCounter >& C )
{
  JoinedQInterval* jqin;
  unsigned long int nwintc =0; // new intervals counter
  unsigned long int nwltc =0; // new left terminated intervals
  unsigned long int uniquebkwe =0; // unique backward extension
  deque< EdgeInterval* >* LT = new deque< EdgeInterval* >(); // Left-terminated
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
    } // ~while

#ifdef DEBUG
  std::cout << "--> Generated " << nwintc << " new intervals." << std::endl;
  std::cout << "--> Generated " << nwltc << " new left-terminated intervals."
	    << std::endl;
  std::cout << "--> Rejected " << uniquebkwe << " unique backward extension"
	    << std::endl;
#endif

  std::cout << "--> Sorting LT" << std::endl;
  std::sort( LT->begin(), LT->end(), CompareEdgeInterval );
  return LT;
}

deque< EdgeInterval* >* search_step_right( BWTReader& b, EdgeJoinedQIntervalManager& imgr, 
					   vector< NucleoCounter >& C, deque< EdgeInterval* >* LT )
{
  deque< EdgeInterval* >* suffix_prefix_comp = new deque< EdgeInterval* >( );
  EdgeInterval* interval_from_mgr = imgr.get_next_interval( );
  EdgeInterval* interval_from_lt = NULL;
  unsigned long int nwintc =0; // new intervals counter
  unsigned long int rejintc =0; // Intervals from LT not right-terminal
  unsigned long int mrgintc =0; // Merged intervals

  if( LT->size( ) > 0 )
    {
      interval_from_lt = LT->front( );
      LT->pop_front( );
    }

  while( ( interval_from_mgr != NULL ) || ( interval_from_lt != NULL ) )
    {
      while( interval_from_lt != NULL && interval_from_mgr != NULL &&
	     EqualSecondEdgeInterval( interval_from_lt, interval_from_mgr ) )
	{
	  // TODO: Merge first intervals from interval_from_lt and
	  // interval_from_mgr (the ones related to Q as suffix)
	  interval_from_mgr = imgr.get_next_interval( );
	  ++mrgintc;
	}

      if( interval_from_lt != NULL && 
	  ( ( interval_from_mgr == NULL)  ||  
	    CompareEdgeInterval( interval_from_lt, interval_from_mgr ) ) )
	{
	  // interval_from_lt comes first than interval_from_mgr or interval
	  // manager can't provide any other new joined intervals
	  BWTPosition begin_first_interval = interval_from_lt->get_first_interval( ).get_begin( );
	  b.move_to( begin_first_interval );
	  BWTPosition begin_first_interval_in_$ = b.get_Pi( )[ BASE_$ ];
	  
	  BWTPosition begin = interval_from_lt->get_second_interval( ).get_begin( );
	  b.move_to( begin );
	  vector< NucleoCounter > occ_begin( b.get_Pi( ) );
	  
	  BWTPosition end_first_interval_in_$ = occ_begin[ BASE_$ ];

	  BWTPosition end = interval_from_lt->get_second_interval( ).get_end( );
	  b.move_to( end );
	  vector< NucleoCounter > occ_end( b.get_Pi( ) );

	  if( occ_end[ BASE_$ ] > occ_begin[ BASE_$ ] )
	    {
	      // Q also is a suffix
	      for( int base( 1 ); base < ALPHABET_SIZE; ++base )
		{
		  BWTPosition new_int_begin = C[ base ] + occ_begin[ base ];
		  BWTPosition new_int_end   = C[ base ] + occ_end[ base ];
		  if( new_int_end > new_int_begin )
		    {
		      ++nwintc;
		      EdgeInterval eInt( begin_first_interval_in_$, end_first_interval_in_$,
					// interval_from_lt->get_first_interval( ).get_begin( ),
					// interval_from_lt->get_first_interval( ).get_end( ),
					 new_int_begin, new_int_end, 1 );
		      imgr.add_interval( eInt, (Nucleotide) base );
		    } // ~if
		} // ~for
	    } // ~if $ in interval
	  else
	    {
	      // else Q is not a suffix, don't care
	      ++rejintc;
	    }

	  delete interval_from_lt;
	  
	  // get next interval from LT, if it exists
	  if( LT->size( ) > 0 ) 
	    {
	      interval_from_lt = LT->front( );
	      LT->pop_front( );
	    }
	  else
	    interval_from_lt = NULL;
	}
      else if( interval_from_mgr != NULL && 
	       ( ( interval_from_lt == NULL)  ||  
		 CompareEdgeInterval( interval_from_mgr, interval_from_lt ) ) )
	{
	  // interval_from_mgr
	  BWTPosition begin = interval_from_mgr->get_second_interval( ).get_begin( );
	  b.move_to( begin );
	  vector< NucleoCounter > occ_begin( b.get_Pi( ) );
	  BWTPosition end = interval_from_mgr->get_second_interval( ).get_end( );
	  b.move_to( end );
	  vector< NucleoCounter > occ_end( b.get_Pi( ) );
	  if( occ_end[ BASE_$ ] > occ_begin[ BASE_$ ] )
	    {
	      // we can now save intervals to compare
	      BWTPosition begin_$ = occ_begin[ BASE_$ ];
	      BWTPosition end_$ = occ_end[ BASE_$ ];
	      EdgeInterval* i = 
		new EdgeInterval( interval_from_mgr->get_first_interval( ).get_begin( ),
				  interval_from_mgr->get_first_interval( ).get_end( ),
				  begin_$, end_$, interval_from_mgr->get_len( ) +1 );
	      suffix_prefix_comp->push_back( i );
	    }
	  else
	    {
	      for( int base( 1 ); base < ALPHABET_SIZE; ++base )
		{
		  BWTPosition new_begin = C[ base ] + occ_begin[ base ];
		  BWTPosition new_end   = C[ base ] + occ_end[ base ];
		  if( new_end > new_begin )
		    {
		      ++nwintc;
		      EdgeInterval i( interval_from_mgr->get_first_interval( ).get_begin( ),
				      interval_from_mgr->get_first_interval( ).get_end( ),
				      new_begin, new_end, interval_from_mgr->get_len( ) +1 );
		      imgr.add_interval( i, (Nucleotide) base );
		    } // ~if
		} // ~for
	    } // ~if BASE_$ in interval
	  interval_from_mgr = imgr.get_next_interval( );
	} // ~if interval from LT
    } // ~while exits interval

  std::cout << "--> Sorting RT" << std::endl;
  std::sort( suffix_prefix_comp->begin( ), suffix_prefix_comp->end( ), CompareEdgeInterval );
#ifdef DEBUG
  std::cout << "--> Generated " << nwintc << " new intervals on BWT'" << std::endl;
  std::cout << "--> Merged " << mrgintc << " intervals from lt and from interval manager" << std::endl;
  std::cout << "--> Rejected " << rejintc << " intervals from LT " << std::endl;
  std::cout << "--> Got " << suffix_prefix_comp->size( ) << " new intervals on GSA to test." 
	    << std::endl;
#endif

  return suffix_prefix_comp;
}

BWTPosition OccLT( vector< NucleoCounter >& occ, Nucleotide base )
{
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
  return (a->get_second_interval( ).get_begin( ) < b->get_second_interval( ).get_begin( ) );
}

bool EqualSecondEdgeInterval( EdgeInterval* a, EdgeInterval* b )
{
  return (a->get_second_interval( ).get_begin( ) == b->get_second_interval( ).get_begin( ) &&
	  a->get_second_interval( ).get_end( ) == b->get_second_interval( ).get_end( ) );
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
