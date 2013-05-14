#include <string>
#include <vector>
#include <deque>
#include <sstream>
#include <iostream>

#include <joined_q_interval.h>
#include <joined_q_int_manager.h>
#include <util.h>
#include <types.h>
#include <BWTReader.h>

#include "search.h"
#include "edge_joined_interval.h"

using std::vector;
using std::string;
using std::deque;

int main ( int argc, char** argv )
{
  // buildgraph BWTFilenamePrefix revBWTFilenamePrefix gsaFilename
  //            argv[ 1 ]         argv[ 2 ]            argv[ 3 ]

  // TODO: manage non positional args
  vector< string > BWTInputFilenames;
  vector< string > revBWTInputFilenames;
  vector< string > qIntFilenames;
  vector< string > revqIntFilenames;

  string gsaInputFileName ( argv[ 3 ] );

  // create filenames and add them to the previous vectors
  for ( int i( 0 ); i < ALPHABET_SIZE; ++i )
    {
      std::ostringstream partialBWTname, partialrevBWTname, qIntFilename, 
	revqIntFilename;
      partialBWTname << argv[ 1 ] << i;
      partialrevBWTname << argv[ 2 ] << i;
      qIntFilename << ".QINT-" << i;
      revqIntFilename << ".revQINT-" << i;

      BWTInputFilenames.push_back( partialBWTname.str( ) );
      revBWTInputFilenames.push_back( partialrevBWTname.str( ) );
      qIntFilenames.push_back( qIntFilename.str( ) );
      revqIntFilenames.push_back( revqIntFilename.str( ) );
    }
  
  JoinedQIntervalManager imgr( qIntFilenames );
  EdgeJoinedQIntervalManager revimgr( revqIntFilenames );
  
  BWTReader br( BWTInputFilenames );
  BWTReader revbr( revBWTInputFilenames );

  vector< NucleoCounter >* c = br.get_C( );
  vector< NucleoCounter >* rev_c = revbr.get_C( ); // Actually there's no need
						   // for this. Maybe we can
						   // simply check if c and
						   // rev_c are equal.
  
  std::cout << "C size: " << c->size() << std::endl;
  std::cout << "C contains : " << std::endl;

  for (int nucl( 0 ); (unsigned int) nucl < ALPHABET_SIZE; ++nucl)
    {
      std::cout << ntoc( (Nucleotide) nucl ) << ": " << c->at( nucl ) << std::endl;
      JoinedQInterval jint ( c->at( nucl ), c->at( nucl+1 ),
			     c->at( nucl ), c->at( nucl+1 ) );
      imgr.add_interval ( jint, (Nucleotide) nucl );
    }

  imgr.swap_files();

  build_tau_intervals( br, imgr, *c, TAU);

  for( int i( 0 ); i < 10; ++i )
    {
      deque< EdgeInterval* >* LT;
      deque< EdgeInterval* >* RT;

      std::cout << "Left search step #" << i+1 << std::endl;
      br.reset( );
      revbr.reset( );
      LT = search_step_left( br, imgr, *c );
      imgr.swap_files( );
      std::cout << "Right search step #" << i+1 << std::endl;
      RT = search_step_right( revbr, revimgr, *rev_c, LT );
      revimgr.swap_files( );
 
      for( deque< EdgeInterval* >::iterator it = (*LT).begin( );
	   it != (*LT).end( ); ++it )
	{
	  if( (*it) != NULL )
	    delete *it;
	}

     bool rt_int_in_GSA$ = true;

     for( deque< EdgeInterval* >::iterator it = (*RT).begin( );
	   it != (*RT).end( ); ++it )
	{
#ifdef DEBUG_VERBOSE_OUTPUT
	  std::cout << "Confronto tra intervallo [" << (*it)->get_first_interval( ).get_begin( )
		    << ", " << (*it)->get_first_interval( ).get_end( ) << ") e intervallo ["
		    << (*it)->get_second_interval( ).get_begin( ) << ", "
		    << (*it)->get_second_interval( ).get_end( ) << ")" << std::endl;
#endif
	  if( (*it)->get_second_interval( ).get_end( ) > (*c)[ BASE_A ] ||
	      (*it)->get_first_interval( ).get_end( ) > (*c)[ BASE_A ] )
	    rt_int_in_GSA$ = false;
	  if( (*it) != NULL )
	    delete *it;
	}
      
     rt_int_in_GSA$ ?
       std::cout << "All RT intervals are in GSA[$]." << std::endl :
       std::cout << "Not all RT intervals are in GSA[$] (THAT'S NOT GOOD)." << std::endl ;

     delete LT;	   
     delete RT;
    }

  delete c;
  delete rev_c;

  return 0;
}
