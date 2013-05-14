#include "search.h"

void build_tau_intervals( BWTReader& b, JoinedQIntervalManager& jqmgr, 
			  vector< NucleoCounter >& C, int T)
{
  JoinedQInterval* jqin;
  // TODO: Refactor
  for( int i( 0 ); i < T; ++i)
    {
      std::cout << "Build tau intervals - " << i+1 << "/" << TAU << std::endl;
      unsigned long int nwintc =0; // new intervals
      unsigned long int uniquebkwe =0;
      while( (jqin = jqmgr.get_next_interval()) )
	{
	  BWTPosition begin = jqin->get_interval().get_begin();
	  b.move_to( begin );
	  vector< NucleoCounter > occ_begin( b.get_Pi( ) ); 
	  BWTPosition end = jqin->get_interval().get_end();
	  b.move_to( end );
	  vector< NucleoCounter > occ_end( b.get_Pi( ) );
#ifdef DEBUG_VERBOSE
	  std::cout << "Q-Interval [ " << begin << " , " << " ) " << std::endl;
#endif
	  for( int base( 1 ); base < ALPHABET_SIZE; ++base )
	    {
	      BWTPosition new_rev_begin = 
		jqin->get_reverse_interval().get_begin() +
		(OccLT(occ_end, (Nucleotide) base) - OccLT(occ_begin, (Nucleotide) base));
	      BWTPosition new_rev_end =
		new_rev_begin +
		(occ_end[ base ] - occ_begin[ base ] );
	      BWTPosition new_begin =
		C[ base ] + occ_begin[ base ];
	      BWTPosition new_end =
		C[ base ] + occ_end[ base ];
	      if( new_end - new_begin >= 2 )
		{
		  ++nwintc;
#ifdef DEBUG_VERBOSE
		  std::cout << "   EXTENDS WITH: " << ntoc( (Nucleotide) base ) << std::endl;
		  std::cout << "  NEW DIMENSION: " << new_end - new_begin << std::endl;
#endif
		  JoinedQInterval new_interval ( new_begin, new_end, new_rev_begin, new_rev_end );
		  jqmgr.add_interval( new_interval, (Nucleotide) base );
		}
	      else if ( new_end == new_begin + 1 )
		{
		  ++uniquebkwe;
		}
	    }
	}
      b.reset();
      jqmgr.swap_files();
#ifdef DEBUG
      std::cout << "Generated " << nwintc << " new intervals." << std::endl;
      std::cout << uniquebkwe << " unique backward extension intervals (rejected)" << std::endl;
#endif
    }
}


BWTPosition OccLT( vector< NucleoCounter >& occ, Nucleotide base )
{
  BWTPosition p =0;
  for(unsigned int i( 0 ); i < (unsigned int) base; ++i )
    {
      p += (BWTPosition) occ[ i ];
    }
  return p;
}

deque< EdgeInterval* >* search_step_left( BWTReader& b, JoinedQIntervalManager& jqmgr,
					  vector< NucleoCounter >& C )
{
  JoinedQInterval* jqin; // = jqmgr.get_next_interval();
  unsigned long int nwintc =0; // new intervals counter
  unsigned long int nwltc =0; // new left terminated intervals
  unsigned long int uniquebkwe =0;
  deque< EdgeInterval* >* LT = new deque< EdgeInterval* >();
  while( (jqin = jqmgr.get_next_interval()) )
    {
      BWTPosition begin = jqin->get_interval().get_begin();
      b.move_to( begin );
      vector< NucleoCounter > occ_begin( b.get_Pi( ) );
      BWTPosition end = jqin->get_interval().get_end();
      b.move_to( end );
      vector< NucleoCounter > occ_end( b.get_Pi( ) );
#ifdef DEBUG_VERBOSE
      std::cout << "Q-Interval [ " << begin << " , " << " ) " << std::endl;
#endif
      for( int base( 0 ); base < ALPHABET_SIZE; ++base )
	{
	  BWTPosition new_rev_begin =
	    jqin->get_reverse_interval().get_begin() +
	    (OccLT(occ_end, (Nucleotide) base) - OccLT(occ_begin, (Nucleotide) base));
	  BWTPosition new_rev_end =
	    new_rev_begin +
	    (occ_end[ base ] - occ_begin[ base ] );
	  BWTPosition new_begin =
	    C[ base ] + occ_begin[ base ];
	  BWTPosition new_end =
	    C[ base ] + occ_end[ base ];
	  if( base != BASE_$ && new_end - new_begin >= 2 )
	    {
#ifdef DEBUG_VERBOSE
	      std::cout << "   EXTENDS WITH: " << ntoc( (Nucleotide) base ) << std::endl;
	      std::cout << "  NEW DIMENSION: " << new_end - new_begin << std::endl;
#endif
	      ++nwintc;
	      JoinedQInterval new_interval ( new_begin, new_end, new_rev_begin, new_rev_end );
	      jqmgr.add_interval( new_interval, (Nucleotide) base );
	    }
	  else if ( base == BASE_$ && new_end - new_begin >= 1 )
	    {
	      // TODO: Left-terminal
	      ++nwltc;
#ifdef DEBUG
	      EdgeInterval* new_interval =
		new EdgeInterval ( new_begin, new_end, 
				   jqin->get_reverse_interval().get_begin(),
				   jqin->get_reverse_interval().get_end(), 0 );
	      LT->push_back( new_interval );
#endif
	    }
	  else if ( new_end - new_begin == 1 )
	    {
	      ++uniquebkwe;
	    }
	}
    }
#ifdef DEBUG
  std::cout << "Generated " << nwintc << " new intervals." << std::endl;
  std::cout << "Generated " << nwltc << " new left-terminated intervals."
	    << std::endl;
  std::cout << uniquebkwe << " unique backward extension (rejected)" << std::endl;
#endif
  std::cout << "Sorting LT" << std::endl;
  std::sort( LT->begin(), LT->end(), CompareEdgeInterval );
  return LT;
}

deque< EdgeInterval* >* search_step_right( BWTReader& b, EdgeJoinedQIntervalManager& imgr, 
					   vector< NucleoCounter >& C, deque< EdgeInterval* >* LT )
{
  deque< EdgeInterval* >* suffix_prefix_comp = new deque< EdgeInterval* >( );
  EdgeInterval* interval_from_mgr = imgr.get_next_interval( );
  EdgeInterval* interval_from_lt = NULL;
  if( LT->size( ) > 0 )
    {
      interval_from_lt = LT->front( );
      LT->pop_front( );
    }

  while( interval_from_mgr != NULL || interval_from_lt != NULL )
    {
      if( interval_from_lt != NULL &&
	  ( interval_from_mgr == NULL ||  
	    CompareEdgeInterval( interval_from_lt, interval_from_mgr ) ) )
	{
	  // interval_from_lt comes first than interval_from_mgr
	  BWTPosition begin = interval_from_lt->get_first_interval( ).get_end( ) +1;
	  b.move_to( begin );
	  vector< NucleoCounter > occ_begin( b.get_Pi( ) );
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
		      EdgeInterval eInt( interval_from_lt->get_first_interval( ).get_begin( ),
					 interval_from_lt->get_first_interval( ).get_end( ),
					 new_int_begin, new_int_end, 1 );
		      imgr.add_interval( eInt, (Nucleotide) base );
		    } // if
		} // for
	    } // if $ in interval
	  // Else Q is not a suffix, don't care
	  delete interval_from_lt;
	  if( LT->size( ) > 0 ) 
	    {
	      interval_from_lt = LT->front( );
	      LT->pop_front( );
	    }
	  else
	    interval_from_lt = NULL;
	}
      else
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
	      //	      std::cout << "2ELSE" << std::endl;
	      for( int base( 1 ); base < ALPHABET_SIZE; ++base )
		{
		  BWTPosition new_begin = C[ base ] + occ_begin[ base ];
		  BWTPosition new_end   = C[ base ] + occ_end[ base ];
		  EdgeInterval i( interval_from_mgr->get_first_interval( ).get_begin( ),
				  interval_from_mgr->get_first_interval( ).get_end( ),
				  new_begin, new_end, interval_from_mgr->get_len( ) +1 );
		  imgr.add_interval( i, (Nucleotide) base );
		}
	    }
	  interval_from_mgr = imgr.get_next_interval( );
	} // If
    } // While
  std::cout << "SORTING RT" << std::endl;
  std::sort( suffix_prefix_comp->begin( ), suffix_prefix_comp->end( ), CompareEdgeInterval );
  std::cout << "We got " << suffix_prefix_comp->size( ) << " new intervals on GSA to compare!" 
	    << std::endl;
  return suffix_prefix_comp;
}

bool CompareJoinedQInterval( JoinedQInterval* a, JoinedQInterval* b)
{
  return ( a->get_reverse_interval().get_begin() < b->get_reverse_interval().get_begin() );
}

bool CompareEdgeInterval( EdgeInterval* a, EdgeInterval* b )
{
  return (a->get_second_interval( ).get_begin( ) < b->get_second_interval( ).get_begin( ) );
}
