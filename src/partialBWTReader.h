// -*- c++ -*-

#ifndef PARTIAL_BWTREADER_H
#define PARTIAL_BWTREADER_H

#include <vector>
#include <string>
#include <cstdlib>
#include <fstream>

#include "types.h"
#include "util.h"

using std::vector;
using std::string;

// Class that handles a single portion of BWT.

class partialBWTReader
{
private:
  std::ifstream             _fileIn;                  // File from which we want to read;
  char*                     _buffer;                  // Current buffer
  BWTPosition               _start;                   // Buffer start position in BWT
  BWTPosition               _position;                // Position reached within the buffer
  BWTPosition               _bufferlen;               // Number of char read in the last "read"
                                                      // call, used while moving inside the buffer
  vector< NucleoCounter >   _occurrencesBeforeStart;  // PI vector

public:
  // Constructor
  partialBWTReader ( const string& inputFilename,
                     BWTPosition start= 0,
                     const vector< NucleoCounter >& occurrencesBeforeStart= vector< NucleoCounter >(ALPHABET_SIZE, 0));

  // Destructor
  ~partialBWTReader ( ) {
    delete[] _buffer;
    _fileIn.close();
  }

  // Get occurrences before current position
  const vector< NucleoCounter >& get_Pi ( ) const {
    return _occurrencesBeforeStart;
  }

  // Get absolute position in BWT
  BWTPosition get_position ( ) const {
    return (_start + _position);
  }

  // Move to position p in this BWT and update nucleotide occurrences accordingly.
  // Return value:
  // - true if p can be reached
  // - false otherwise
  bool move_to ( const BWTPosition & p );

  // Return nucleotide in the current position
  char get_current_nucleotide_char( ) const {
    return _buffer[_position];
  }

private:
  // no need of copy ctor nor assignment operator
  partialBWTReader ( ) { }
  partialBWTReader ( const partialBWTReader& other )
  { }
  partialBWTReader& operator= ( const partialBWTReader& other )
  { return *this; }
};

#endif
