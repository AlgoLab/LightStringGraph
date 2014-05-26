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
  std::cerr << "-l <length> ";
  std::cerr << std::endl << "Options:" << std::endl;
  std::cerr << "\t-i, --input \t <GSAFileInput>" << std::endl;
  std::cerr << "\t-o, --output \t <GSAFileOutput>" << std::endl;
  std::cerr << "\t-l, --length \t <length>" << std::endl;
  std::cerr << std::endl;
}

int main( int argc, char** argv )
{
  if( argc < 7 )
    {
      show_usage( );
      return 1;
    }

  string inputGSA ="";
  string outputGSA ="";
  SequenceLength seqlen;

  for (int i = 1; i < argc; i++)
    {
      if (i + 1 != argc)
        {
          if (string(argv[i]) == "--input" || string(argv[i]) == "-i")
            inputGSA = string(argv[++i]);
          else if (string(argv[i]) == "--output" || string(argv[i]) == "-o")
            outputGSA = string(argv[++i]);
          else if (string(argv[i]) == "--length" || string(argv[i]) == "-l")
            {
              std::stringstream convert(string(argv[++i]));
              if( !( convert >> seqlen ) )
                {
                  std::cerr << "Can't convert " << string( argv[ i ] ) << " to integer (length)." << std::endl;
                  std::cerr << "Aborting.." << std::endl;
                  std::exit( -1 );
                }
            }
        }
      else
        {
          std::cerr << "Invalid arguments, please try again.\n";
          return 1;
        }
    }

  ifstream gsain( inputGSA.c_str( ), std::ios::binary );
  ofstream gsaout( outputGSA.c_str( ), std::ios::binary );

  std::cerr << "Reconstructing GSA from " << inputGSA << " to " << outputGSA
            << "...";

  GSAEntry x;

  while( gsain >> x )
    {
      x.sa = seqlen - x.sa;
      gsaout << x;
    }

  gsain.close( );
  gsaout.close( );
  std::cerr << "..done" << std::endl;

  return 0;
}