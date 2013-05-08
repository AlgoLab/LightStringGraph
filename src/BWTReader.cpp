#include <BWTReader.h>

BWTReader::BWTReader ( vector< string >& filenamesIN )
  : _filenamesIN( filenamesIN )
{
  if ( _filenamesIN.size() == 0 )
    {
      std::cerr << "ERROR: Can't initialize a BWTReader without filenames."
		<< std::endl << "Aborting." << std::endl;
      std::exit( -1 );
    }

#ifndef NDEBUG
  std::cout << "Initializing BWTReader on files :" << std::endl;
  for ( vector< string >::iterator it = _filenamesIN.begin();
	it != _filenamesIN.end();
	++it)
    std::cout << *it << std::endl;
#endif

  if ( _filenamesIN.size() >= 1 )
    {
      _currentBWT = new partialBWTReader( _filenamesIN[ 0 ] );
      _nextBWTFilename = 1;
    }
}

BWTReader::~BWTReader ( )
{
  delete _currentBWT;
}

BWTPosition BWTReader::get_position ( ) const
{
  return _currentBWT->get_position();
}

bool BWTReader::move_to ( BWTPosition & p )
{
  while( !_currentBWT->move_to( p ) )
    {
      if( _nextBWTFilename >= _filenamesIN.size() )
	{
	  std::cerr << "In BWTRedear::move_to: " << std::endl;
	  std::cerr << "ERROR: Can't reach position " << p
		    << " because the BWT size is " 
		    << _currentBWT->get_position( ) << std::endl
		    << "Aborting." << std::endl;
	  std::exit( -1 );
	}
      std::cout << "Moving to partialBWT : " << _filenamesIN[ _nextBWTFilename ] << std::endl;
      partialBWTReader* nextBWT = new partialBWTReader( _filenamesIN[ _nextBWTFilename++ ], 
							_currentBWT->get_position( ),
							_currentBWT->get_Pi( ) );
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

  for ( ; _nextBWTFilename < ALPHABET_SIZE -1; ++_nextBWTFilename)
    {
      while ( _currentBWT->move_to( pos ) )
	{
	  pos += (BWTPosition) BUFFERSIZE;
	}
      std::cout << "Switch to BWT file number: " << _nextBWTFilename << std::endl;
      partialBWTReader* nextBWT = new partialBWTReader( _filenamesIN[ _nextBWTFilename ],
							_currentBWT->get_position( ),
							_currentBWT->get_Pi( ) );
      delete _currentBWT;
      _currentBWT = nextBWT;
    }

  vector< NucleoCounter >* C = new vector< NucleoCounter >();
  for( vector< NucleoCounter >::iterator it = _currentBWT->get_Pi().begin();
        it != _currentBWT->get_Pi().end();
        ++it )
     {
       C->push_back(*it);
     }

  // Get back to the first partial BWT
  reset ( );
  return C;
}

void BWTReader::reset ( )
{
  delete _currentBWT;
  _currentBWT = new partialBWTReader ( _filenamesIN[ 0 ] );
  _nextBWTFilename = 1;
}
