#include <string>
#include <vector>
#include <deque>
#include <sstream>
#include <iostream>

#include "BWTReader.h"
#include "GSAReader.h"
#include "joined_q_interval.h"
#include "util.h"
#include "types.h"
#include "search.h"
#include "edge_joined_interval.h"

using std::vector;
using std::string;
using std::deque;
using std::stringstream;

int TAU    =0;
int CYCNUM =0;

void show_usage(){
    std::cerr << "Usage: lsg -B <BWTFilenamePrefix> ";
    std::cerr << "-R <RevBWTFilenamePrefix> ";
    std::cerr << "-G <gsaFilename> ";
    std::cerr << "-T <TAU> ";
    std::cerr << "-C <CycNum>" << std::endl;
    std::cerr << std::endl << "Options:" << std::endl;
    std::cerr << "\t-B, --BWT \t <BWTFilenamePrefix>" << std::endl;
    std::cerr << "\t-R, --RevBWT \t <RevBWTFilenamePrefix>" << std::endl;
    std::cerr << "\t-G, --GSA \t <gsaFilename>" << std::endl;
    std::cerr << "\t-T, --TAU \t <TAU>" << std::endl;
    std::cerr << "\t-C, --CycNum \t <CycNum>" << std::endl;
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
  vector< NucleoCounter >* rev_c = revbr.get_C( ); // Actually there's no need
						   // for this. Maybe we can
						   // simply check if c and
						   // rev_c are equal.
  std::cerr << "done." << std::endl;

  //  std::cerr << "C size: " << c->size() << std::endl;
  std::cerr << "C contains : " << std::endl;

  for (int nucl( BASE_A ); (unsigned int) nucl < ALPHABET_SIZE; ++nucl)
    {
      std::cerr << ntoc( (Nucleotide) nucl ) << ": " << c->at( nucl ) << std::endl;
      JoinedQInterval jint ( c->at( nucl ), c->at( nucl+1 ),
			     c->at( nucl ), c->at( nucl+1 ) );
      imgr.add_interval ( jint, (Nucleotide) nucl );
    }

  std::cout.flush( );
  imgr.swap_files();

  build_tau_intervals( br, imgr, *c, TAU);

  // temporary
  SGraph sg; // String graph
  Precedencies p; // p[ x ] = y if exists an edge y->x in sg

  for( int i( 0 ); i < CYCNUM; ++i ) // TODO: while exists interval in imgr or revimgr
    {
      vector< EdgeInterval* >* LT;
      vector< EdgeInterval* >* RT;

      std::cerr << "Left search step #" << i+1 << std::endl;
      br.reset( );
      revbr.reset( );
      LT = search_step_left( br, imgr, *c );
      imgr.swap_files( );

      std::cerr << "Right search step #" << i+1 << std::endl;
      RT = search_step_right( revbr, revimgr, *rev_c, LT );
      revimgr.swap_files( );

      // All intervals got from search_step_right should be in GSA[ $ ]
      bool rt_int_in_GSA$ = true; 

      std::cerr << "Analysis and delete" << std::endl;

      for( vector< EdgeInterval* >::iterator it = RT->begin( );
       	   it != RT->end( ); ++it )
      	{
	  for( unsigned int j =0; j < (*it)->get_second_interval( ).size( ); ++j )
	    {
	      // vector< GSAEntry* >* first_interval =
	      // 	gsardr.get( (*it)->get_first_interval( ).get_begin( ),
	      // 		    (*it)->get_first_interval( ).get_end( ) );
	      // vector< GSAEntry* >* second_interval =
	      // 	gsardr.get( (*it)->get_second_interval( )[ j ]->get_begin( ),
	      // 		    (*it)->get_second_interval( )[ j ]->get_end( ) );
	      // EdgeLength len = (*it)->get_len( )[ j ];
	      
	      // for( vector< GSAEntry* >::iterator it_f = first_interval->begin( );
	      // 	   it_f != first_interval->end( ); ++it_f )
	      // 	{
	      // 	  for( vector< GSAEntry* >::iterator it_s = second_interval->begin( );
	      // 	       it_s != second_interval->end( ); ++it_s )
	      // 	    {
	      // 	      checkIfIrreducible( sg, p, *it_f, *it_s, len );
	      // 	    }
	      // 	}

	      // for( vector< GSAEntry* >::iterator it_f = first_interval->begin( );
	      // 	   it_f != first_interval->end( ); ++it_f )
	      // 	  delete *it_f;
	      // delete first_interval;

	      // for( vector< GSAEntry* >::iterator it_s = second_interval->begin( );
	      // 	   it_s != second_interval->end( ); ++it_s )
	      // 	  delete *it_s;
	      // delete second_interval;
	    }
	  delete *it;
	}

     rt_int_in_GSA$ ?
       std::cerr << "All RT intervals are in GSA[$]." << std::endl :
       std::cerr << "Not all RT intervals are in GSA[$] (THAT'S NOT GOOD)." << std::endl ;
     
     LT->clear( );
     RT->clear( );
     vector< EdgeInterval* >( ).swap( *LT ); // Memory trick?
     vector< EdgeInterval* >( ).swap( *RT );
     delete LT;
     delete RT;
    }

  // TODO: find first read in a better way
  // unsigned int begin_of_graph =0;
  // for( std::map< unsigned int, SGEdge*>::iterator it = sg.begin( );
  //      it != sg.end( ); ++it )
  //   {
  //     if( p.find( it->second->first_read ) == p.end( ) )
  // 	{
  // 	  begin_of_graph = it->second->first_read;
  // 	}      
  //   }

  // bool ended = false;

  // // print edges
  // while( !ended )
  //   {
  //     std::cout << "Edge : " << sg[ begin_of_graph ]->first_read << " -> "
  // 		<< sg[ begin_of_graph ]->second_read << " len: "
  // 		<< sg[ begin_of_graph ]->len << std::endl;
  //     ended = ( sg.find( sg[begin_of_graph]->second_read ) == sg.end( ) );
  //     if( !ended )
  // 	{
  // 	  begin_of_graph = sg[begin_of_graph]->second_read;
  // 	}
  //   }

  std::cerr << "SG SIZE : " << sg.size( ) << std::endl;

  // std::cerr << "Output edges.." << std::endl;

  // // delete sg
  // bool first_edge = true;

  // std::cout << "{" << std::endl << "\t\"edges\": [" << std::endl;
  // for( std::map< unsigned int, SGEdge*>::iterator it = sg.begin( );
  //      it != sg.end( ); ++it )
  //   {
  //     if( !first_edge )
  // 	std::cout << "," << std::endl;
  //     else
  // 	first_edge = false;
  //     std::cout << "\t\t{" << std::endl
  // 		<< "\t\t\t\"from\" : " << it->second->first_read << "," << std::endl
  // 		<< "\t\t\t\"to\" : " << it->second->second_read << "," << std::endl
  // 		<< "\t\t\t\"length\" : " << it->second->len << std::endl
  // 		<< "\t\t}";
  //     delete it->second;
  //     it->second = NULL;
  //   }
  // std::cout << std::endl << "\t]" << std::endl << "}" << std::endl;
  

  delete c;
  delete rev_c;

  return 0;
}
