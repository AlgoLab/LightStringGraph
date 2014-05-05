#include <string>
#include <vector>
#include <deque>
#include <sstream>
#include <iostream>

#ifdef USESTXXL
#include <stxxl.h>
#endif

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

int       TAU         =0;
int       CYCNUM      =0;

void show_usage(){
  std::cerr << "Usage: lsg -B <BWTFilenamePrefix> ";
  std::cerr << "-G <gsaFilename> ";
  std::cerr << "-T <TAU> ";
  std::cerr << "-C <CycNum> ";
  // TODO: Manage reads with variable lengths and delete this
  std::cerr << "-l <readsLenght> ";
  std::cerr << std::endl;
  std::cerr << std::endl << "Options:" << std::endl;
  std::cerr << "\t-B, --basename \t <prefix>" << std::endl;
  std::cerr << "\t-G, --GSA \t <gsaFilename>" << std::endl;
  std::cerr << "\t-T, --TAU \t <TAU>" << std::endl;
  std::cerr << "\t-C, --CycNum \t <CycNum>" << std::endl;
  // TODO: Manage reads with variable lengths and delete this
  std::cerr << "\t-l, --reads-length \t <readsLength>" << std::endl;
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

  string basename = "";
  string gsaInputFileName = "";

  SequenceLength readsLen;

  for (int i = 1; i < argc; i++) {
    if (i + 1 != argc){
      if (string(argv[i]) == "--basename" || string(argv[i]) == "-B") {
        basename = string(argv[++i]);
      } else if (string(argv[i]) == "--GSA" || string(argv[i]) == "-G") {
        gsaInputFileName = string(argv[++i]);
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
      } else if (string(argv[i]) == "--reads-length" || string(argv[i]) == "-l") {
        stringstream convert(string(argv[++i]));
        if( !( convert >> readsLen ) ) {
          std::cerr << "Can't convert " << string( argv[ i ] ) << " to integer (readsLen)." << std::endl;
          std::cerr << "Aborting.." << std::endl;
          std::exit( -1 );
        }
      } else {
        std::cerr << "Invalid arguments, please try again.\n";
        return 1;
      }
    }
  }

  if ( basename == "" || gsaInputFileName == "") {
    std::cerr << "Missing argument(s)." << std::endl;
    show_usage();
    return 1;
  }
  vector< string > BWTInputFilenames;
  vector< string > LCPInputFilenames;
  vector< string > qIntFilenames;
  vector< string > baseqIntFilenames;

  GSAReader gsardr( gsaInputFileName );

  // create filenames and add them to the previous vectors
  for ( int i( BASE_$ ); i < ALPHABET_SIZE; ++i )
    {
      std::ostringstream \
		  partialBWTname,
		  partialLCPname,
		  qIntFilename,
		  baseqIntFilename;
      partialBWTname << basename << "-B0" << i;
      partialLCPname << basename << "-L0" << i;
      qIntFilename << ".QINT-" << i;
      baseqIntFilename << ".bQ-" << i;

      BWTInputFilenames.push_back( partialBWTname.str( ) );
      LCPInputFilenames.push_back( partialLCPname.str( ) );
      qIntFilenames.push_back( qIntFilename.str( ) );
      baseqIntFilenames.push_back( baseqIntFilename.str( ) );
    }

  //  JoinedQIntervalManager imgr( qIntFilenames );
  QIntervalManager qmgr( baseqIntFilenames );

  BWTReader br( BWTInputFilenames );

  std::cerr << "Building vector C..";
  std::cerr.flush( );

  vector< NucleoCounter >* c = br.get_C( );

  std::cerr << "done." << std::endl;
  std::cerr << std::endl << "Vector C" << std::endl;
  std::cerr << "Nucl\t| C(x) " << std::endl;
  std::cerr << "--------------------" << std::endl;
  for (int nucl( BASE_A ); nucl < ALPHABET_SIZE; ++nucl)
    {
      std::cerr << ntoc( (Nucleotide) nucl ) << "\t| " << c->at( nucl )
                << std::endl;
    }

  std::cerr << "@ " << now( "%I:%M:%S %p %Z" ) << std::endl;

  LCPIterator lcpit( LCPInputFilenames );
  GSAIterator gsait( gsaInputFileName );

  vector< QIntervalManager > qmgrs;
  build_basic_arc_intervals(br, lcpit, gsait, readsLen, TAU, *c, qmgrs);
  return -1;

  std::cerr << "Building base intervals (SEED length 1)" << std::endl;
  for (int nucl( BASE_A ); nucl < ALPHABET_SIZE; ++nucl)
    {
      QInterval qint( c->at( nucl ), c->at( nucl +1 ) );
      qmgr.add_interval( qint, (Nucleotide) nucl );
    }

  std::cerr.flush();
  qmgr.swap_files();

  build_tau_intervals( br, qmgr, gsardr, *c, TAU);

  for( int i( 0 ); i < CYCNUM; ++i )
    {
      std::cerr << "@ " << now( "%I:%M:%S %p %Z" ) << std::endl;
      std::cerr << "--> Left Step - " << i+1 << "/" << CYCNUM << std::endl;
      left_step( br, qmgr, gsardr, *c, i + TAU);
      br.reset();
      gsardr.reset();
      qmgr.swap_files();
    }

  // temporary
  // SGraph sg; // String graph
  // Precedencies p; // p[ x ] = y if exists an edge y->x in sg

  // for( int i( 0 ); i < CYCNUM; ++i ) // TODO: while exists interval in imgr or revimgr
  //   {
  //     stxxl::VECTOR_GENERATOR< PODEdgeInterval >::result prefix_interval;
  //     // vector< EdgeInterval* >* LT;
  //     // vector< EdgeInterval* >* RT;

  //     std::cerr << "[" << now( "%I:%M:%S %p %Z" ) << "]" << std::endl;
  //     std::cerr << "Left search step #" << i+1 << std::endl;
  //     br.reset( );
  //     revbr.reset( );
  //     size_t newlti = search_step_left( br, imgr, *c, prefix_interval);
  //     imgr.swap_files( );
  //     std::cerr << "[" << now( "%I:%M:%S %p %Z" ) << "]" << std::endl;
  //     std::cerr << "--> Sorting " << newlti << " elements..." << std::endl;
  //     stxxl::sort(prefix_interval.begin(), prefix_interval.end(), SortPODEdgeInterval(), MEMORY_SORT_SIZE);
  //     // std::ifstream* newLT = ext_sort( P_FILE );
  //     // remove(P_FILE.c_str());
  //     // std::ifstream* newLT = new std::ifstream( MERGED_FILES.c_str(), std::ios::binary );

  //     std::cerr << "[" << now( "%I:%M:%S %p %Z" ) << "]" << std::endl;
  //     std::cerr << "Right search step #" << i+1 << std::endl;
  //     search_step_right( revbr, revimgr, *rev_c, prefix_interval );

  //     revimgr.swap_files( );
  //     // newLT->close();
  //     // delete newLT;
  //     // RT->clear( );
  //     // delete RT;

  //     remove(MERGED_FILES.c_str());
  //     // All intervals got from search_step_right should be in GSA[ $ ]
  //   }

  // // for( std::vector<string>::iterator it=qIntFilenames.begin(); it != qIntFilenames.end();
  // //      ++it)
  // //   remove((*it).c_str());
  // // for( std::vector<string>::iterator it=revqIntFilenames.begin(); it != revqIntFilenames.end();
  // //      ++it)
  // //   remove((*it).c_str());

  delete c;

  return 0;
}
