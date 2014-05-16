// -*- c++ -*-

#ifndef PARTIAL_BWTREADER_H
#define PARTIAL_BWTREADER_H

#include <vector>
#include <string>
#include <cstdlib>

#ifdef HAS_ZLIB
#include <zlib.h>
#else
#include <cstdio>
#endif

#include "types.h"
#include "util.h"


using std::vector;
using std::string;

// Class that handles a single portion of BWT.

class partialBWTReader
{
private:
#ifdef HAS_ZLIB
  gzFile         _fileIn;         // File from which we want to read;
#else
  FILE*          _fileIn;         // File from which we want to read;
#endif
  char* const    _buffer;         // Current buffer
  BWTPosition    _start;          // Buffer start position in BWT
  BWTPosition    _position;       // Position reached within the buffer
  BWTPosition    _bufferlen;      // Number of char read in the last "read"
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
    if (_fileIn != NULL) {
#ifdef HAS_ZLIB
      gzclose(_fileIn);
#else
      fclose(_fileIn);
#endif
    }
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

};

#endif
