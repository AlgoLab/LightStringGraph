#include "GSAReader.h"

GSAReader::GSAReader( const string& filename )
  : _inputFile( filename.c_str( ), std::ios::binary ),
    _currentPosition( 0 )
{
  _inputFile.seekg( 0, std::ios_base::end );
  _maxPosition = _inputFile.tellg() / sizeof( GSAEntry );
  _inputFile.seekg( 0, std::ios_base::beg );
  _inputFile >> _lastGSA;
}

GSAReader::~GSAReader( )
{
  _inputFile.close( );
}

void GSAReader::get_seq_sent( int length, SeqNExtVect& rsuff, BWTPosition end )
{
  while( (_lastGSA.sa == (SequenceLength) length) && _currentPosition < end )
    {
      rsuff.push_back(_lastGSA.numSeq);
      move_to( _currentPosition +1 );
    }
}

BWTPosition GSAReader::get_position( )
{
  return _currentPosition;
}

void GSAReader::get_seq_at_pos( BWTPExtVect& positions, SeqNExtVect& indexes, BWTPosition end )
{
  for( size_t p( 0 ); p < positions.size() && positions[p] < end; ++p )
    {
      if( move_to(positions[p]) )
        indexes.push_back(_lastGSA.numSeq);
    }
}

bool GSAReader::move_to( const BWTPosition& p )
{
  if(_currentPosition > p )
    {
      DEBUG_LOG_VERBOSE("ERROR: Can't reach position " << p << " (can't move backward).");
      DEBUG_LOG_VERBOSE("Current Position is " << _currentPosition);

      return false;
    }
  else if( p > _maxPosition )
    {
      DEBUG_LOG_VERBOSE("ERROR: Can't move to " << p);
      DEBUG_LOG_VERBOSE(" _maxPosition is " << _maxPosition);

      return false;
    }
  else if( _currentPosition < p )
    {
      _inputFile.seekg( p * sizeof(GSAEntry), std::ios_base::beg );
      _inputFile >> _lastGSA;
      _currentPosition = p;
    }

  return true;
}

void GSAReader::reset( )
{
  _inputFile.clear( );
  _inputFile.seekg( 0, _inputFile.beg );
  _inputFile >> _lastGSA;
  _currentPosition = 0;
}

GSAEntry GSAReader::get_v( )
{
  return _lastGSA;
}