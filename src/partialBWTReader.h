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
  // Constructors
  partialBWTReader ( string inputFilename );
  partialBWTReader ( string inputFilename, BWTPosition start, vector< NucleoCounter >& occurrencesBeforeStart );

  // Destructor
  ~partialBWTReader ( );

  // Get occurrences before current position
  vector< NucleoCounter >& get_Pi( );

  // Get absolute position in BWT
  BWTPosition get_position ( ) const;

  // Move to position p in this BWT and update nucleotide occurrences accordingly.
  // Return value:
  // - true if p can be reached
  // - false otherwise
  bool move_to ( const BWTPosition & p );

  // Return nucleotide in the current position
  char get_current_nucleotide();

private:
  // no need of copy ctor nor assignment operator
  partialBWTReader ( ) { }
  partialBWTReader ( const partialBWTReader& other )
  { }
  partialBWTReader& operator= ( const partialBWTReader& other )
  { return *this; }
};

#endif
