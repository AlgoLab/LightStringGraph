#include "BWTReader.h"

BWTReader::BWTReader ( vector< string >& filenamesIN )
  : _filenamesIN( filenamesIN )
{
  if ( _filenamesIN.size() == 0 )
    {
      std::cerr << "ERROR: Can't initialize a BWTReader without filenames."
                << std::endl << "Aborting." << std::endl;
      std::exit( -1 );
    }


  DEBUG_LOG_VERBOSE("Initializing BWTReader on files :");
  for ( vector< string >::iterator it = _filenamesIN.begin();
        it != _filenamesIN.end();
        ++it)
    {
      DEBUG_LOG_VERBOSE(*it);
    }

  if ( _filenamesIN.size() >= 1 )
    {
      _currentBWT = new partialBWTReader( _filenamesIN[ 0 ] );
      _nextBWTFilename = 1;
    }

  _maxPosition =0; // Useless to compute right now, will be computed by get_C
}

BWTReader::~BWTReader ( )
{
  delete _currentBWT;
}

BWTPosition BWTReader::get_position ( ) const
{
  return _currentBWT->get_position();
}

BWTPosition BWTReader::size( ) const
{
  return _maxPosition;
}

bool BWTReader::move_to_storing_sent( BWTPosition p, BWTPExtVect& prefixpos )
{
  for( BWTPosition i = _currentBWT->get_position(); i < p; ++i )
    {
      if( move_to( i ) )
        {
          if( cton(_currentBWT->get_current_nucleotide()) == BASE_$  )
            {
              prefixpos.push_back( i );
            }
        }
      else
        return false;
    }
  return move_to( p );
}


bool BWTReader::move_to ( const BWTPosition & p )
{
  while( !_currentBWT->move_to( p ) )
    {
      if( _nextBWTFilename >= _filenamesIN.size() )
        {
          
          DEBUG_LOG_VERBOSE("In BWTReader::move_to: ");
          DEBUG_LOG_VERBOSE("ERROR: Can't reach position " << p \
                            << " because the BWT size is " <<   \
                            _currentBWT->get_position() );
          return false;
        }

      DEBUG_LOG_VERBOSE("BWT trying to move to position " << p << " that is outside current file.");
      DEBUG_LOG_VERBOSE("Open BWT file: " << _filenamesIN[ _nextBWTFilename ]);

      partialBWTReader* nextBWT =
        new partialBWTReader( _filenamesIN[ _nextBWTFilename++ ], _currentBWT->get_position( ), _currentBWT->get_Pi( ) );

      delete _currentBWT;
      _currentBWT = nextBWT;
    }
  return true;
}

vector< NucleoCounter >& BWTReader::get_Pi ( )
{
  return _currentBWT->get_Pi ( );
}

vector< NucleoCounter >* BWTReader::get_C ( )
{
  BWTPosition pos = 0;

  // Move to the end
  // for ( ; _nextBWTFilename < ALPHABET_SIZE -1; ++_nextBWTFilename)
  {
    while ( this->move_to( pos ) )
      {
        pos += (BWTPosition) BUFFERSIZE;
      }
  }

  DEBUG_LOG_VERBOSE("Reached the end..");

  vector< NucleoCounter >* C = new vector< NucleoCounter >();
  NucleoCounter acc =0;
  C->push_back( acc );
  for( vector< NucleoCounter >::iterator it = _currentBWT->get_Pi().begin();
       it != _currentBWT->get_Pi().end(); ++it )
    {
      acc += *it;
      C->push_back(acc);
    }

  _maxPosition = _currentBWT->get_position( );
  // Get back to the first partial BWT
  reset ( );
  return C;
}

void BWTReader::reset ( )
{
  delete _currentBWT;
  _currentBWT = new partialBWTReader ( _filenamesIN[ 0 ] );
  DEBUG_LOG_VERBOSE("(RESET) Open BWT file: " << _filenamesIN[ 0 ]);
  _nextBWTFilename = 1;
}

Nucleotide BWTReader::get_current_nucleotide( ) const
{
  return cton(_currentBWT->get_current_nucleotide());
}
