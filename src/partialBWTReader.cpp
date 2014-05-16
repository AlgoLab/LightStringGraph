#include <partialBWTReader.h>

partialBWTReader::partialBWTReader ( const string& inputFilename,
                                     BWTPosition start,
                                     const vector< NucleoCounter >& occurrencesBeforeStart )
    :_buffer(new char[ BUFFERSIZE ]),
	  _start(start),
	  _position(0),
     _occurrencesBeforeStart(occurrencesBeforeStart)
{
  _fileIn.open( inputFilename.c_str() );
  if( _fileIn.fail() )
    {
      std::cerr << "ERROR: Can't open file " << inputFilename << std::endl
                << "Aborting." << std::endl;
      _MY_FAIL;
    }

  _fileIn.read( _buffer, BUFFERSIZE );
  _bufferlen = _fileIn.gcount();
}

bool partialBWTReader::move_to ( const BWTPosition & p )
{
  if( p < _start + _position )
    {
      std::cerr << "Can't move to " << p << " because the current position is "
                << _start + _position << std::endl;
      std::cerr << "_start    = " << _start << std::endl;
      std::cerr << "_position = " << _position << std::endl;
      _MY_FAIL;
    }
  while ( ( _bufferlen != 0 ) &&
          ( _position < _bufferlen ) &&
          ( _start + _position < p ) )
    {
      char currentChar = _buffer[ _position ];
      ++_occurrencesBeforeStart[ NuclConv::cton( currentChar ) ];
      ++_position;
      if ( _position == _bufferlen )
        {
          _fileIn.read( _buffer, BUFFERSIZE );
          _start += _bufferlen;
          _position = 0;
          _bufferlen = _fileIn.gcount();
          if(_bufferlen == 0)
            {
              return false;
            }
        }
    }
  if( (_start + _position  < p) || ((_position == 0) && (_bufferlen == 0)) )
    {
      DEBUG_LOG_VERBOSE("ERROR: asked to reach position " << p << " but reached position " \
                        << _start + _position << " instead.");

      return false;
    }

  return true;
}
