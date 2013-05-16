#include <string>
#include <vector>
#include <deque>
#include <sstream>
#include <iostream>

#include "BWTReader.h"
#include "joined_q_interval.h"
#include "util.h"
#include "types.h"
#include "search.h"
#include "edge_joined_interval.h"

using std::vector;
using std::string;
using std::deque;


void show_usage(){
    std::cerr << "Usage: stringGraph -B <BWTFilenamePrefix> ";
    std::cerr << "-R <RevBWTFilenamePrefix> ";
    std::cerr << "-G <gsaFilename>" << std::endl;
    std::cerr << std::endl << "Options:" << std::endl;
    std::cerr << "\t-B, --BWT \t <BWTFilenamePrefix>" << std::endl;
    std::cerr << "\t-R, --RevBWT \t <RevBWTFilenamePrefix>" << std::endl;
    std::cerr << "\t-G, --GSA \t <gsaFilename>" << std::endl;
    std::cerr << std::endl;
}

/****************/
/* Program Main */
/****************/
int main ( int argc, char** argv )
{
    if(argc < 6){
        show_usage();
        return 1;
    }

    string bwt_pre = "";
    string rev_bwt_pre = "";
    string gsa_file = "";
    for (int i = 1; i < argc; i++) {
        if (i + 1 != argc){
            if (string(argv[i]) == "--BWT" || string(argv[i]) == "-B") {
                bwt_pre = string(argv[++i]);
            } else if (string(argv[i]) == "--RevBWT" || string(argv[i]) == "-R") {
                rev_bwt_pre = string(argv[++i]);
            } else if (string(argv[i]) == "--GSA" || string(argv[i]) == "-G") {
                gsa_file = string(argv[++i]);
            } else {
                std::cerr << "Invalid arguments, please try again.\n";
                return 1;
            }
        }
    }

    if ( bwt_pre == "" || rev_bwt_pre == "" || gsa_file == "") {
        std::cerr << "Missing argument(s)." << std::endl;
        show_usage();
        return 1;
    }
  vector< string > BWTInputFilenames;
  vector< string > revBWTInputFilenames;
  vector< string > qIntFilenames;
  vector< string > revqIntFilenames;

  string gsaInputFileName (gsa_file);

  // create filenames and add them to the previous vectors
  for ( int i( 0 ); i < ALPHABET_SIZE; ++i )
    {
      std::ostringstream partialBWTname, partialrevBWTname, qIntFilename, 
	revqIntFilename;
      partialBWTname << bwt_pre << i;
      partialrevBWTname << rev_bwt_pre << i;
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

  std::cout << "Building vector C..";
  std::cout.flush( );

  vector< NucleoCounter >* c = br.get_C( );
  vector< NucleoCounter >* rev_c = revbr.get_C( ); // Actually there's no need
						   // for this. Maybe we can
						   // simply check if c and
						   // rev_c are equal.
  std::cout << "done." << std::endl;

  std::cout << "C size: " << c->size() << std::endl;
  std::cout << "C contains : " << std::endl;

  for (int nucl( BASE_A ); (unsigned int) nucl < ALPHABET_SIZE; ++nucl)
    {
      std::cout << ntoc( (Nucleotide) nucl ) << ": " << c->at( nucl ) << std::endl;
      JoinedQInterval jint ( c->at( nucl ), c->at( nucl+1 ),
  			     c->at( nucl ), c->at( nucl+1 ) );
      imgr.add_interval ( jint, (Nucleotide) nucl );
    }

  imgr.swap_files();

  build_tau_intervals( br, imgr, *c, TAU);

  for( int i( 0 ); i < 85; ++i )
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
      
      // All intervals got from search_step_right should be in GSA[ $ ]
      bool rt_int_in_GSA$ = true; 

      for( deque< EdgeInterval* >::iterator it = (*RT).begin( );
  	   it != (*RT).end( ); ++it )
  	{
  	  if( (*it)->get_second_interval( ).get_end( ) > ( (BWTPosition) c->at( BASE_A ) ) ||
	      (*it)->get_first_interval( ).get_end( ) > ( (BWTPosition) c->at( BASE_A ) ) )
  	    {
  	      std::cout << "ERROR: interval not in GSA[ $ ]: [ "
			<< (*it)->get_first_interval( ).get_begin( ) << ", "
			<< (*it)->get_first_interval( ).get_end( )
			<< " ) & [ "
  	      		<< (*it)->get_second_interval( ).get_begin( ) << ", "
  	      		<< (*it)->get_second_interval( ).get_end( )
			<< " ) - GSA LIMIT: " << c->at( BASE_A ) << std::endl;
  	      rt_int_in_GSA$ = false;
  	    }
	  
	  // TODO:
	  // check_if_irreducible( (*it)->get_first_interval( ),  // Q as suffix
	  //                       (*it)->get_second_interval( ), // Q as prefix
	  //                       (*it)->get_len( ) );

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
