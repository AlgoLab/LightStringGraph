#include <partialBWTReader.h>

partialBWTReader::partialBWTReader ( string inputFilename )
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

partialBWTReader::partialBWTReader ( string inputFilename, BWTPosition start, 
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
  _position = 0;
  
  for( vector< NucleoCounter >::iterator it = occurrencesBeforeStart.begin();
       it != occurrencesBeforeStart.end();
       ++it)
    _occurrencesBeforeStart.push_back( *it );
  
  _buffer = new char[ BUFFERSIZE ]; 
  _fileIn.read( _buffer, BUFFERSIZE );
  _bufferlen = _fileIn.gcount();
}

vector< NucleoCounter >& partialBWTReader::get_Pi ( )
{
  return _occurrencesBeforeStart;
}

partialBWTReader::~partialBWTReader ( )
{
  delete[] _buffer;
  _fileIn.close();
}

// BWTPosition partialBWTReader::get_position ( )
// {
//   return _position;
// }

BWTPosition partialBWTReader::get_position ( ) const
{
  return (_start + _position);
}

bool partialBWTReader::move_to ( BWTPosition & p )
{
  while ( ( _bufferlen != 0 ) &&
	  ( _position < _bufferlen ) &&
	  ( _start + _position < p ) )
    {
      char currentChar = _buffer[ _position ];
      ++_occurrencesBeforeStart[ cton( currentChar ) ];
#ifdef DEBUG_VERBOSE
      if( cton( currentChar ) == NOT_IN_ALPHABET )
	std::cout << "Found NOT_IN_ALPHABET in position : " << _start + _position << std::endl;
      else if (currentChar == 'N')
	std::cout << "Found N in position : " << _start + _position << std::endl;
#endif
      ++_position;
      if ( _position == _bufferlen )
	{
	  _fileIn.read( _buffer, BUFFERSIZE );
	  _start += _bufferlen;
	  _position = 0;
	  _bufferlen = _fileIn.gcount();
	}
    }
  if( _start + _position  < p )
    {
#ifndef NDEBUG
      std::cerr << "ERROR: asked to reach position " << p << " but reached position "
		<< _start + _position << " instead." << std::endl;
#endif
      return false;
    }
  return true;
}
