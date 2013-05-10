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
		  jqmgr.add_q_interval( new_interval, (Nucleotide) base );
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
      std::cerr << nwintc << "," << uniquebkwe << ",0"<< std::endl;
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

vector< JoinedQInterval* >* search_step_left( BWTReader& b, JoinedQIntervalManager& jqmgr, vector< NucleoCounter >& C )
{
  JoinedQInterval* jqin; // = jqmgr.get_next_interval();
  unsigned long int nwintc =0; // new intervals counter
  unsigned long int nwltc =0; // new left terminated intervals
  unsigned long int uniquebkwe =0;
  vector< JoinedQInterval* >* LT = new vector< JoinedQInterval* >();
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
	      jqmgr.add_q_interval( new_interval, (Nucleotide) base );
	    }
	  else if ( base == BASE_$ && new_end - new_begin >= 1 )
	    {
	      // TODO: Left-terminal
	      ++nwltc;
#ifdef DEBUG
	      JoinedQInterval* new_interval =
		new JoinedQInterval ( jqin->get_reverse_interval().get_begin(),
		jqin->get_reverse_interval().get_end(),
		new_begin,
		new_end);
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
  std::cerr << nwintc << "," << uniquebkwe << "," << LT->size() << std::endl;
  std::cout << "Sorting LT" << std::endl;
  std::sort( LT->begin(), LT->end(), CompareJoinedQInterval );
#endif
  return LT;
}

bool CompareJoinedQInterval( JoinedQInterval* a, JoinedQInterval* b)
{
  return ( a->get_reverse_interval().get_begin() < b->get_reverse_interval().get_begin() );
}
