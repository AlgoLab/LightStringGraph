// -*- c++ -*-

#include "LCPReader.h"

LCPReader::LCPReader(std::vector< std::string >& filenamesIN)
  : _filenamesIN(filenamesIN)
{
  if(_filenamesIN.size() == 0)
    {
      std::cerr << "ERROR: Can't initialize an LCPReader without filenames."
                << std::endl << "Aborting." << std::endl;
      _MY_FAIL;
    }

  DEBUG_LOG_VERBOSE("Initializing LCPReader on files :");
  for(std::vector<std::string>::iterator it = _filenamesIN.begin();
      it != _filenamesIN.end(); ++it)
    {
      DEBUG_LOG_VERBOSE(*it);
    }
  _currentLCP    = new partialLCPReader(_filenamesIN[0]);
  _nextLCPIndex =1;
}

LCPReader::~LCPReader()
{
  delete _currentLCP;
}

BWTPosition LCPReader::get_position() const
{
  return _currentLCP->get_position();
}

bool LCPReader::next()
{
  _lastLCP = _currentLCP->get();
  if(_currentLCP->next())
    return true;

  
  DEBUG_LOG_VERBOSE("Can't move to next position, loading next batch");

  // End of _currentLCP
  if(_nextLCPIndex >= _filenamesIN.size())
    {
      DEBUG_LOG_VERBOSE("ERROR: Can't reach position " << _currentLCP->get_position() + 1 \
                        << "because the LCP size is " << _currentLCP->get_position());
      return false;
    }
  DEBUG_LOG_VERBOSE( "LCP trying to move to position " << _currentLCP->get_position() +1  \
                     << " that is outside the current file.");
  DEBUG_LOG_VERBOSE("Open LCP file: " << _filenamesIN[_nextLCPIndex]);

  partialLCPReader* nextLCP = new partialLCPReader( _filenamesIN[_nextLCPIndex],
                                                    _currentLCP->get_position() );
  ++_nextLCPIndex;
  delete _currentLCP;
  _currentLCP = nextLCP;
  return true;
}

LCPValue LCPReader::get() const
{
  return _currentLCP->get();
}
