#ifndef GSA_READER_H
#define GSA_READER_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <stxxl.h>

#include "types.h"
#include "util.h"

using std::vector;
using std::string;

class GSAReader
{
private:
  std::ifstream	_inputFile;
  BWTPosition 	_currentPosition;
  BWTPosition	_maxPosition;
  GSAEntry	_lastGSA;

public:
  GSAReader( const string& );
  ~GSAReader( );
  bool move_to( const BWTPosition& );
  void reset( );
  //  vector< GSAEntry* >* get( const BWTPosition&, const BWTPosition& );
  GSAEntry get_v( );
  // Gets reads index of contiguous suffixes which length is "length" starting
  // from _currentPosition.
  // _currentPosition and _inputFile will be modified!
  void get_seq_sent( int length, SeqNExtVect& rsuff ); 
  void get_seq_at_pos( BWTPExtVect& position, SeqNExtVect& indexes );
  BWTPosition get_position( );

private:
  GSAReader( ) { };
  GSAReader( const GSAReader& other ) { };
  GSAReader& operator=( const GSAReader& other ) { return *this; };
  
};

#endif
