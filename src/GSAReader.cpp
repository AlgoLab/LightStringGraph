#include "GSAReader.h"

GSAReader::GSAReader( const string& filename )
  : _inputFile( filename.c_str( ), std::ios::binary )
{
}

GSAReader::~GSAReader( )
{
  _inputFile.close( );
}

vector< GSAEntry* >* GSAReader::get( const BWTPosition& start, const BWTPosition& end )
{
  if( end < start )
    {
      std::cerr << "ERROR: " << end << " less than " << start
		<< ". Can't read GSA entries." << std::endl;
      return NULL;
    }
  vector< GSAEntry* >* v = new vector< GSAEntry* >( );

  unsigned int i =0;

  unsigned int start_byte = start * sizeof( GSAEntry );

  while( i < end-start )
    {
      _inputFile.seekg( i * sizeof( GSAEntry ) + start_byte );
      GSAEntry* x = new GSAEntry( );
      _inputFile.read( (char *) &x->sa, sizeof( unsigned int ) );
      _inputFile.read( (char *) &x->numSeq, sizeof( unsigned int ) );
      v->push_back( x );
      ++i;
    }

    return v;
}
