#include <BWTReader.h>

BWTReader::BWTReader ( string inputFilename )
{
  _fileIn.open( inputFilename.c_str() );
  if( _fileIn.fail() )
    {
      std::cerr << "ERROR: Can't open file " << inputFilename << std::endl
		<< "Aborting." << std::endl;
      std::exit( -1 );
    }

  for ( int i( 0 ); i < ALPHABET_SIZE; ++i )
    _occurrencesBeforeStart.push_back( 0 );
  
  _start =0;
  _position = _start;

  _buffer = new char[ BUFFERSIZE ];
  _fileIn.read( _buffer, BUFFERSIZE );
  _bufferlen = _fileIn.gcount();
}

BWTReader::BWTReader ( string inputFilename, BWTPosition start, 
		       vector< NucleoCounter >& occurrencesBeforeStart )
{
  _fileIn.open( inputFilename.c_str() );
  if( _fileIn.fail() )
    {
      std::cerr << "ERROR: Can't open file " << inputFilename << std::endl
		<< "Aborting." << std::endl;
      std::exit( -1 );
    }

  _start = start;
  _position = _start;
  
  for( vector< NucleoCounter >::iterator it = occurrencesBeforeStart.begin();
       it != occurrencesBeforeStart.end();
       ++it)
    _occurrencesBeforeStart.push_back( *it );
  
  _buffer = new char[ BUFFERSIZE ]; 
  _fileIn.read( _buffer, BUFFERSIZE );
  _bufferlen = _fileIn.gcount();
}

vector< NucleoCounter >& BWTReader::get_Pi ( )
{
  return _occurrencesBeforeStart;
}

BWTReader::~BWTReader ( )
{
  delete[] _buffer;
  _fileIn.close();
}

// BWTPosition BWTReader::get_position ( )
// {
//   return _position;
// }

BWTPosition BWTReader::get_position ( )
{
  return (_start + _position);
}

bool BWTReader::move_to ( BWTPosition & p )
{
  while ( ( _bufferlen != 0 ) && 
	  ( _position < _bufferlen ) && 
	  ( _start + _position < p ) )
    {
      char currentChar = _buffer[ _position ];
      ++_occurrencesBeforeStart[ cton( currentChar ) ];
      ++_position;
      if ( _position == _bufferlen )
	{
	  _fileIn.read( _buffer, BUFFERSIZE );
	  _start += _bufferlen;
	  _position = 0;
	  _bufferlen = _fileIn.gcount();
	}
    }
  if( _start + _position  != p )
    {
#ifdef DEBUG
      std::cerr << "ERROR: asked to reach position " << p << " but reached position " 
		<< _start + _position << " instead." << std::endl;
#endif
      return false;
    }
  return true;
}
