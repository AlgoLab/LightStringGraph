#include "search.h"

BWTPosition OccLT( vector< NucleoCounter >& occ, Nucleotide base )
{
  BWTPosition p =0;
  for(unsigned int i( 0 ); i < (unsigned int) base; ++i )
    {
      p += (BWTPosition) occ[ i ];
    }
  return p;
}

void search_step_left( BWTReader& b, JoinedQIntervalManager& jqmgr, vector< NucleoCounter >& C )
{
  JoinedQInterval* jqin; // = jqmgr.get_next_interval();
  unsigned long nwintc =0; // new intervals counter
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
	  if( new_end - new_begin > 2 )
	    {
#ifdef DEBUG_VERBOSE
	      std::cout << "   EXTENDS WITH: " << ntoc( (Nucleotide) base ) << std::endl;
	      std::cout << "  NEW DIMENSION: " << new_end - new_begin << std::endl;
#endif
	      ++nwintc;
	      JoinedQInterval new_interval ( new_begin, new_end, new_rev_begin, new_rev_end );
	      jqmgr.add_q_interval( new_interval, (Nucleotide) base );
	    }
	}
    }
#ifdef DEBUG
  std::cout << "Generated " << nwintc << " new intervals." << std::endl;
#endif
}
