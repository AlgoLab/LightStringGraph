#include <string>
#include <vector>
#include <deque>
#include <sstream>
#include <iostream>
#include <stxxl.h>

#include "BWTReader.h"
#include "GSAReader.h"
#include "joined_q_interval.h"
#include "util.h"
#include "types.h"
#include "search.h"
#include "edge_joined_interval.h"
#include "ext_sort.h"

using std::vector;
using std::string;
using std::deque;
using std::stringstream;

int			TAU    		=0;
int			CYCNUM 		=0;
//const stxxl::uint64	BLOCK_SIZE 	=256 * 1024 * 1024; // 256 MBytes
const stxxl::uint64	MEMORY_SORT_SIZE=512 * 1024 * 1024; // 512 MBytes

void show_usage(){
    std::cerr << "Usage: lsg -B <BWTFilenamePrefix> ";
    std::cerr << "-R <RevBWTFilenamePrefix> ";
    std::cerr << "-G <gsaFilename> ";
    std::cerr << "-T <TAU> ";
    std::cerr << "-C <CycNum> ";
    // std::cerr << "-M <memoryConsumption> ";
    std::cerr << std::endl;
    std::cerr << std::endl << "Options:" << std::endl;
    std::cerr << "\t-B, --BWT \t <BWTFilenamePrefix>" << std::endl;
    std::cerr << "\t-R, --RevBWT \t <RevBWTFilenamePrefix>" << std::endl;
    std::cerr << "\t-G, --GSA \t <gsaFilename>" << std::endl;
    std::cerr << "\t-T, --TAU \t <TAU>" << std::endl;
    std::cerr << "\t-C, --CycNum \t <CycNum>" << std::endl;
    // std::cerr << "\t-M, --Mem \t <memoryConsumption>";
    // std::cerr << std::endl;
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
	    } else if (string(argv[i]) == "--TAU" || string(argv[i]) == "-T") {
	      stringstream convert(string(argv[++i]));
	      if( !( convert >> TAU ) ) {
		std::cerr << "Can't convert " << string( argv[ i ] ) << " to integer (TAU)." << std::endl;
		std::cerr << "Aborting.." << std::endl;
		std::exit( -1 );
	      }
	    } else if (string(argv[i]) == "--CycNum" || string(argv[i]) == "-C") {
	      stringstream convert(string(argv[++i]));
	      if( !( convert >> CYCNUM ) ) {
		std::cerr << "Can't convert " << string( argv[ i ] ) << " to integer (CYCNUM)." << std::endl;
		std::cerr << "Aborting.." << std::endl;
		std::exit( -1 );
	      }
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

  string gsaInputFileName( gsa_file );

  GSAReader gsardr( gsaInputFileName );

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

  std::cerr << "Building vector C..";
  std::cerr.flush( );

  vector< NucleoCounter >* c = br.get_C( );
#ifdef DEBUG_VERBOSE
  std::cerr << "Building vector rev_C..";
#endif
  vector< NucleoCounter >* rev_c = revbr.get_C( ); // Actually there's no need
						   // for this. Maybe we can
						   // simply check if c and
						   // rev_c are equal.
 
  std::cerr << "done." << std::endl;
  std::cerr << "C contains : " << std::endl;

  for (int nucl( BASE_A ); (unsigned int) nucl < ALPHABET_SIZE; ++nucl)
    {
      std::cerr << ntoc( (Nucleotide) nucl ) << ": " << c->at( nucl )
		<< std::endl;
      JoinedQInterval jint ( c->at( nucl ), c->at( nucl+1 ),
			     c->at( nucl ), c->at( nucl+1 ) );
      imgr.add_interval ( jint, (Nucleotide) nucl );
    }

#ifdef DEBUG_VERBOSE
  bool crevcequal = true;
  for(int nucl(BASE_A); (unsigned int) nucl < ALPHABET_SIZE; ++nucl)
    {
      if(c->at(nucl) != rev_c->at(nucl))
	crevcequal = false;
    }
  if(crevcequal)
    std::cerr << "C and REV_C are equals" << std::endl;
  else
    std::cerr << "C and REV_C are not equals" << std::endl;
#endif

  std::cerr.flush( );
  imgr.swap_files();

  build_tau_intervals( br, imgr, *c, TAU);

  // temporary
  // SGraph sg; // String graph
  // Precedencies p; // p[ x ] = y if exists an edge y->x in sg

  for( int i( 0 ); i < CYCNUM; ++i ) // TODO: while exists interval in imgr or revimgr
    {
      stxxl::VECTOR_GENERATOR< PODEdgeInterval >::result prefix_interval;
      // vector< EdgeInterval* >* LT;
      // vector< EdgeInterval* >* RT;

      std::cerr << "[" << now( "%I:%M:%S %p %Z" ) << "]" << std::endl;
      std::cerr << "Left search step #" << i+1 << std::endl;
      br.reset( );
      revbr.reset( );
      size_t newlti = search_step_left( br, imgr, *c, prefix_interval);
      imgr.swap_files( );
      std::cerr << "[" << now( "%I:%M:%S %p %Z" ) << "]" << std::endl;
      std::cerr << "--> Sorting " << newlti << " elements..." << std::endl;
      stxxl::sort(prefix_interval.begin(), prefix_interval.end(), SortPODEdgeInterval(), MEMORY_SORT_SIZE);
      // std::ifstream* newLT = ext_sort( P_FILE );
      // remove(P_FILE.c_str());
      // std::ifstream* newLT = new std::ifstream( MERGED_FILES.c_str(), std::ios::binary );
      
      std::cerr << "[" << now( "%I:%M:%S %p %Z" ) << "]" << std::endl;
      std::cerr << "Right search step #" << i+1 << std::endl;
      search_step_right( revbr, revimgr, *rev_c, prefix_interval );

      revimgr.swap_files( );
      // newLT->close();
      // delete newLT;
      // RT->clear( );
      // delete RT;

      remove(MERGED_FILES.c_str());
      // All intervals got from search_step_right should be in GSA[ $ ]
    }

  // for( std::vector<string>::iterator it=qIntFilenames.begin(); it != qIntFilenames.end();
  //      ++it)
  //   remove((*it).c_str());
  // for( std::vector<string>::iterator it=revqIntFilenames.begin(); it != revqIntFilenames.end();
  //      ++it)
  //   remove((*it).c_str());

  delete c;
  delete rev_c;

  return 0;
}
