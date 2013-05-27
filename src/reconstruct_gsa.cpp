// Reconstruct GSA[ $ ]

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "types.h"
#include "BWTReader.h"
#include "util.h"

using std::vector;

void show_usage( )
{
  std::cerr << "Usage: bgsa -i <GSAFileInput> ";
  std::cerr << "-o <GSAFileOutput> ";
  std::cerr << "-b <BWTPrefix>";
  std::cerr << std::endl << "Options:" << std::endl;
  std::cerr << "\t-i, --input \t <GSAFileInput>" << std::endl;
  std::cerr << "\t-o, --output \t <GSAFileOutput>" << std::endl;
  std::cerr << "\t-b, --BWT \t <BWTPrefix>" << std::endl;
  std::cerr << std::endl;
}

ifstream& operator>>( ifstream& in, GSAEntry& x )
{
  in >> x.sa;
  in >> x.numSeq;
  return in;
}

ofstream& operator<<( ofstream& out, GSAEntry& x )
{
  out << x.sa;
  out << x.numSeq;
  return out;
}

int main( int argc, char** argv )
{
  if( argc < 4 )
    {
      show_usage( );
      return 1;
    }
 
  string inputGSA ="";
  string outputGSA ="";
  string bwtprefix ="";
 
  for (int i = 1; i < argc; i++)
    {
    if (i + 1 != argc)
      {
	if (string(argv[i]) == "--input" || string(argv[i]) == "-i") 
	  {
	    inputGSA = string(argv[++i]);
	  } 
	else if (string(argv[i]) == "--output" || string(argv[i]) == "-o") 
	  {
	    outputGSA = string(argv[++i]);
	  } 
	else if (string(argv[i]) == "--BWT" || string(argv[i]) == "-b") 
	  {
	    bwtprefix = string(argv[++i]);
	  } 
      } 
    else 
      {
	std::cerr << "Invalid arguments, please try again.\n";
	return 1;
      }
    }

  vector< string > BWTInputFilenames;
  for( int i( 0 ); i < ALPHABET_SIZE; ++i )
    {
      std::ostringstream partialBWTname;
      partialBWTname << bwtprefix << i;
      BWTInputFilenames.push_back( partialBWTname.str( ) );
    }

  BWTReader br( BWTInputFilenames );
  ifstream gsain( inputGSA.c_str( ), std::ios::binary );
  ofstream gsaout( outputGSA.c_str( ), std::ios::binary );
  
  BWTPosition nextp =1;
  NucleoCounter sentinelnum =0;

  std::cout << "Reconstructing GSA from " << inputGSA << " to " << outputGSA
	    << " using BWT files " << bwtprefix << "*..." << std::endl;

  while( br.move_to( nextp ) )
    {
      if( br.get_Pi( )[ BASE_$ ] == sentinelnum +1 )
	{
	  ++sentinelnum;
	  GSAEntry x;
	  gsain.seekg( nextp * sizeof( GSAEntry ) );
	  gsain >> x;
	  gsaout << x;
	  // every now and then update the output...
	  if( sentinelnum % 5678 == 0 )
	    std::cout << "\rNumber of $ found : " << sentinelnum
		      << "\t\tCurrent position : " << nextp;
	}
      ++nextp;
    }
  std::cout << "\rNumber of $ found : " << sentinelnum
	    << "\t\tCurrent position : " << nextp << std::endl;

  std::cout << "..done" << std::endl;
  
  gsain.close( );
  gsaout.close( );

  return 0;
}
