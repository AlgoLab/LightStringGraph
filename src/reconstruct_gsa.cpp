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
  std::cerr << std::endl << "Options:" << std::endl;
  std::cerr << "\t-i, --input \t <GSAFileInput>" << std::endl;
  std::cerr << "\t-o, --output \t <GSAFileOutput>" << std::endl;
  std::cerr << std::endl;
}

ifstream& operator>>( ifstream& in, GSAEntry& x )
{
  in.read( (char *) &x.sa, sizeof( unsigned int ) );
  in.read( (char *) &x.numSeq, sizeof( unsigned int ) );
  return in;
}

ofstream& operator<<( ofstream& out, GSAEntry& x )
{
  out.write( (char *) &x.sa, sizeof( unsigned int ) );
  out.write( (char *) &x.numSeq, sizeof( unsigned int ) );
  out.flush( );
  return out;
}

int main( int argc, char** argv )
{
  if( argc < 3 )
    {
      show_usage( );
      return 1;
    }
 
  string inputGSA ="";
  string outputGSA ="";
 
  for (int i = 1; i < argc; i++)
    {
    if (i + 1 != argc)
      {
	if (string(argv[i]) == "--input" || string(argv[i]) == "-i") 
	    inputGSA = string(argv[++i]);
	else if (string(argv[i]) == "--output" || string(argv[i]) == "-o") 
	    outputGSA = string(argv[++i]);
      } 
    else 
      {
	std::cerr << "Invalid arguments, please try again.\n";
	return 1;
      }
    }

  ifstream gsain( inputGSA.c_str( ), std::ios::binary );
  ofstream gsaout( outputGSA.c_str( ), std::ios::binary );
  
  NucleoCounter sentinelnum =0;

  std::cerr << "Reconstructing GSA from " << inputGSA << " to " << outputGSA
	    << "*..." << std::endl;

  GSAEntry x;

  while( gsain >> x )
    {
      if( x.sa == 0 )
	{
	  gsaout << x;
	  ++sentinelnum;
	  if( sentinelnum % 5678 == 0 )
	    std::cerr << "\rNumber of $ found : " << sentinelnum;
	}
    }

  std::cerr << "\rNumber of $ found : " << sentinelnum << std::endl;
  std::cerr << "..done" << std::endl;
  
  gsain.close( );
  gsaout.close( );

  return 0;
}
