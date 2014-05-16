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
#include "BWTIterator.h"
#include "GSAIterator.h"
#include "LCPIterator.h"
#include "util.h"
#include "types.h"
#include "search.h"
#include "extend_symbol_pile.h"
#include "MultiFileManager.h"

using std::vector;
using std::string;
using std::deque;
using std::stringstream;


void show_usage(){
  std::cerr << "Usage: lsg ";
  std::cerr << "-B <basename> ";
  std::cerr << "[-G <GSAFilename>] ";
  std::cerr << "[-T <TAU>] ";
  std::cerr << "[-C <CycNum>] ";
  std::cerr << "[-l <readLenght>]";
  std::cerr << std::endl;
  std::cerr << std::endl << "Options:" << std::endl;
  std::cerr << "\t-B, --basename    <basename>     # (required)" << std::endl;
  std::cerr << "\t-G, --GSA         <GSAFilename>  # (default: '<basename>.pairSA')" << std::endl;
  std::cerr << "\t-T, --TAU         <TAU>          # (default: 0)" << std::endl;
  std::cerr << "\t-C, --CycNum      <CycNum>       # (default: 0)" << std::endl;
  std::cerr << "\t-l, --read-length <readLength>   # 0 if unknown or not fixed (default: 0)" << std::endl;
  std::cerr << std::endl;
}

/****************/
/* Program Main */
/****************/
int main ( int argc, char** argv )
{
  if(argc < 3){
    show_usage();
    return 1;
  }

  string basename = "";
  string gsaInputFileName = "";

  SequenceLength readLen= 0;  // If readLen==0, then the program does not
										// assume that all reads have the same length

  SequenceLength TAU= 0;
  int CYCNUM= 0;

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
      } else if (string(argv[i]) == "--read-length" || string(argv[i]) == "-l") {
        stringstream convert(string(argv[++i]));
        if( !( convert >> readLen ) ) {
          std::cerr << "Can't convert " << string( argv[ i ] ) << " to integer (readLen)." << std::endl;
          std::cerr << "Aborting.." << std::endl;
          std::exit( -1 );
        }
      } else {
        std::cerr << "Invalid arguments, please try again.\n";
        return 1;
      }
    }
  }

  if ( basename == "" ) {
    std::cerr << "Missing argument(s)." << std::endl;
    show_usage();
    return 1;
  }
  if (gsaInputFileName == "") {
	 gsaInputFileName= basename+".pairSA";
  }
  vector< string > BWTInputFilenames;
  vector< string > LCPInputFilenames;
  vector< string > qIntFilenames;
  vector< string > baseqIntFilenames;
  vector< string > basicArcIntervalFilenames;
  vector< vector< string > > edgeIntFilenames;
  vector< string > extendSymbolFilenames;

  GSAReader gsardr( gsaInputFileName );

  // create filenames and add them to the previous vectors
  for ( int i( BASE_$ ); i < ALPHABET_SIZE; ++i )
    {
      std::ostringstream \
		  partialBWTname,
		  partialLCPname,
		  qIntFilename,
		  baseqIntFilename,
		  basicArcIntervalName;
      std::stringstream edgeIntFilename;
      partialBWTname << basename << "-B0" << i;
      partialLCPname << basename << "-L0" << i;
      basicArcIntervalName << basename << LSG_INFIX_TMP ".bai-SIGMA_" << i;
      qIntFilename << basename << LSG_INFIX_TMP ".QINT-" << i;
      baseqIntFilename << basename << LSG_INFIX_TMP ".bQ-" << i;

      BWTInputFilenames.push_back( partialBWTname.str( ) );
      LCPInputFilenames.push_back( partialLCPname.str( ) );
      qIntFilenames.push_back( qIntFilename.str( ) );
      baseqIntFilenames.push_back( baseqIntFilename.str( ) );
      basicArcIntervalFilenames.push_back( basicArcIntervalName.str() );
    }

  //  JoinedQIntervalManager imgr( qIntFilenames );
  //  QIntervalManager qmgr( baseqIntFilenames );

  BWTReader br( BWTInputFilenames );
  SameLengthArcIntervalManager qmgr( baseqIntFilenames );

  std::cerr << "Building vector C..";
  std::cerr.flush( );

  vector< NucleoCounter >* c = br.get_C( );

  std::cerr << "done." << std::endl;
  std::cerr << std::endl << "Vector C" << std::endl;
  std::cerr << "Nucl\t| C(x) " << std::endl;
  std::cerr << "--------------------" << std::endl;
  for (int nucl( BASE_A ); nucl < ALPHABET_SIZE; ++nucl)
    {
      std::cerr << NuclConv::ntoc( (Nucleotide) nucl ) << "\t| " << c->at( nucl )
                << std::endl;
    }

  std::cerr << "@ " << now( "%I:%M:%S %p %Z" ) << std::endl;

  BWTIterator bwtit( BWTInputFilenames );
  LCPIterator lcpit( LCPInputFilenames );
  GSAIterator gsait( gsaInputFileName );

  PrefixManager pref_mgr(basename + LSG_INFIX_OUT ".lexorder");

  BasicArcIntervalManager baimgr(basicArcIntervalFilenames, "-LEN_");

  SequenceLength max_len = build_basic_arc_intervals(bwtit, lcpit, gsait, pref_mgr, readLen, TAU, *c, baimgr);

  for(SequenceLength j(0); j < max_len; ++j)
    {
      std::ostringstream extsymfn, edgeIntFilename;
      extsymfn << basename << LSG_INFIX_TMP ".extsym-LEN_" << j;
      extendSymbolFilenames.push_back(extsymfn.str());

      edgeIntFilenames.push_back( vector< string >( ) );

      for(int i(0); i < ALPHABET_SIZE; ++i)
        {
          edgeIntFilename << basename << LSG_INFIX_TMP ".arc-SIGMA_" << i << "-LEN_" << j;
          edgeIntFilenames[j].push_back( edgeIntFilename.str( ) );
          edgeIntFilename.str(string(""));
          edgeIntFilename.clear();
        }
    }

  EdgeLabelIntervalManager edgemgr( edgeIntFilenames );
  OutputMultiFileManager arcsOut(basename + LSG_INFIX_OUT ".arcs_");
  OutputMultiFileManager labelOut(basename + LSG_INFIX_OUT ".labels_");

  for( int i( 1 ); i <= CYCNUM; ++i )
    {
      LCPIterator lcpit( LCPInputFilenames );
      GSAIterator gsait( gsaInputFileName );

      ExtendSymbolPile extsym_p(extendSymbolFilenames);

      std::cerr << "@ " << now( "%I:%M:%S %p %Z" ) << std::endl;
      // std::cerr << "--> Left Step - " << i+1 << "/" << CYCNUM << std::endl;
      // left_step( br, qmgr, gsardr, *c, i + TAU);
      std::cerr << "-> Extend-Arc-Intervals - " << i << "/" << CYCNUM << std::endl;
      extend_arc_intervals(TAU + i, *c, br, gsait, qmgr, baimgr[i+TAU], extsym_p, edgemgr, pref_mgr, arcsOut);

      extsym_p.switch_mode( );

      br.reset();
      gsardr.reset();
      gsait.reset();
      edgemgr.swap_files();

      std::cerr << "-> Extend-Arc-Labels - " << i << "/" << CYCNUM << std::endl;
      extend_arc_labels(edgemgr, extsym_p, *c, br, gsait, lcpit, max_len, labelOut);

      br.reset();
      gsardr.reset();
      qmgr.swap_files();
    }

  delete c;
  return 0;
}
