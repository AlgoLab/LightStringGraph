#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#include <joined_q_interval.h>
#include <util.h>
#include <types.h>

using std::vector;
using std::string;

int main ( int argc, char** argv )
{
  // buildgraph BWTFilenamePrefix revBWTFilenamePrefix gsaFilename
  //            argv[ 1 ]         argv[ 2 ]            argv[ 3 ]

  // TODO: manage non positional args
  vector< string > BWTInputFilenames;
  vector< string > revBWTInputFilenames;
  string gsaInputFileName ( argv[ 3 ] );

  // create filename for partial bwt
  for ( int i( 0 ); i < ALPHABET_SIZE; ++i )
    {
      std::ostringstream partialBWTname, partialrevBWTname;
      partialBWTname << argv[ 1 ] << i;
      partialrevBWTname << argv[ 2 ] << i;
      BWTInputFilenames.push_back( partialBWTname.str() );
      revBWTInputFilenames.push_back( partialrevBWTname.str( ) );
    }
}
