// -*- c++ -*-

#ifndef BWTREADER_H
#define BWTREADER_H

#include <vector>
#include <string>
#include <cstdlib>

#ifdef STXXL
#include <stxxl.h>
#endif

#include "types.h"
#include "partialBWTReader.h"

using std::vector;
using std::string;

// Read a BWT splitted in multiple files

class BWTReader
{
private:
  const vector< string > _filenamesIN;       // names of BWT sections
  unsigned short         _nextBWTFilename;   // index of next BWTFilename in _filenamesIoN
  partialBWTReader*      _currentBWT;        // current BWT section
  BWTPosition            _maxPosition;       // MaxPosition of BWT

public:
  // Constructor
  BWTReader ( const vector< string >& filenamesIN );

  // Destructor
  ~BWTReader ( ) {
	 delete _currentBWT;
  }

  // Returns current position
  BWTPosition get_position ( ) const {
    return _currentBWT->get_position();
  }

  // Returns max position
  BWTPosition size( ) const {
    return _maxPosition;
  }

  // Move to position p in this BWT and update nucleotide occurrences accordingly.
  // Return value:
  // - true if p can be reached
  // - false otherwise
  bool move_to ( const BWTPosition& p );

  // Move to position p in this BWT, update nucleotide occurrences accordingly, and save
  // indexes of the occurrences of BASE_$ in prefixpos.
  // Return value:
  // - true if p can be reached
  // - false otherwise
  bool move_to_storing_sent( BWTPosition p, BWTPExtVect& prefixpos );

  // Return PI vector (occurrences of every nucleotide before current position).
  const vector< NucleoCounter >& get_Pi ( ) const {
    return _currentBWT->get_Pi ( );
  }

  // Return C (count)
  vector< NucleoCounter > get_C ( );

  // Reset BWT
  void reset ( );

  // Get current nucleotide
  Nucleotide get_current_nucleotide() const;

private:
  // no need of copy ctor nor assignment operator
  BWTReader ( ) { };
  BWTReader ( const BWTReader& other )
  { }
  BWTReader& operator= ( const BWTReader& other )
  { return *this; }

};

#endif
