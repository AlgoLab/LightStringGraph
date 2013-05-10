#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#include <joined_q_interval.h>
#include <joined_q_int_manager.h>
#include <util.h>
#include <types.h>
#include <BWTReader.h>

#include "search.h"

using std::vector;
using std::string;

int main ( int argc, char** argv )
{
  // buildgraph BWTFilenamePrefix revBWTFilenamePrefix gsaFilename
  //            argv[ 1 ]         argv[ 2 ]            argv[ 3 ]

  // TODO: manage non positional args
  vector< string > BWTInputFilenames;
  vector< string > revBWTInputFilenames;
  vector< string > qIntFilenames;

  string gsaInputFileName ( argv[ 3 ] );

  // create filename for partial bwt
  for ( int i( 0 ); i < ALPHABET_SIZE; ++i )
    {
      std::ostringstream partialBWTname, partialrevBWTname, qIntFilename;
      partialBWTname << argv[ 1 ] << i;
      partialrevBWTname << argv[ 2 ] << i;
      qIntFilename << ".QINT-" << i;

      BWTInputFilenames.push_back( partialBWTname.str( ) );
      revBWTInputFilenames.push_back( partialrevBWTname.str( ) );
      qIntFilenames.push_back( qIntFilename.str( ) );
    }
  
  JoinedQIntervalManager imgr( qIntFilenames );
  
  BWTReader br( BWTInputFilenames );
  vector< NucleoCounter >* c = br.get_C();
  
  std::cout << "C size: " << c->size() << std::endl;
  std::cout << "C contains : " << std::endl;

  for (int nucl( 0 ); (unsigned int) nucl < ALPHABET_SIZE; ++nucl)
    {
      std::cout << ntoc( (Nucleotide) nucl ) << ": " << c->at( nucl ) << std::endl;
      JoinedQInterval jint ( c->at( nucl ), c->at( nucl+1 ),
			     c->at( nucl ), c->at( nucl+1 ) );
      imgr.add_q_interval ( jint, (Nucleotide) nucl );
    }

  imgr.swap_files();

  // JoinedQInterval* j = imgr.get_next_interval();
  // while( j != NULL )
  //   {
  //     std::cout << "Q-interval [ " << j->get_interval().get_begin()
  //     		<< " , " << j->get_interval().get_end() << " ) " << std::endl
  //     		<< "revQ-interval [ " << j->get_reverse_interval().get_begin()
  //     		<< " , " << j->get_reverse_interval().get_end() << " ) " 
  // 		<< std::endl;
  //     j = imgr.get_next_interval();
  //   }

  build_tau_intervals( br, imgr, *c, TAU);

  for( int i( 0 ); i < 50; ++i )
    {
      vector< JoinedQInterval* >* LT;
      std::cout << "Left search step #" << i+1 << std::endl;
      br.reset( );
      LT = search_step_left( br, imgr, *c );
      imgr.swap_files( );
      for( vector< JoinedQInterval* >::iterator it = (*LT).begin();
	   it != (*LT).end();
	   ++it)
	delete *it;
      delete LT;	   
    }

  delete c;

  return 0;
}
