#ifndef BWTREADER_H
#define BWTREADER_H

#include <vector>
#include <string>
#include <cstdlib>
#include <fstream>

#include <types.h>
#include <util.h>

using std::vector;
using std::string;

// Class that handles sequential reading of the (partial) BWT from disk to RAM

class partialBWTReader
{
 private:
  std::ifstream _fileIn; // File from which we want to read;
  char* _buffer;
  BWTPosition _start; // Absolute (buffer) start position in BWT
  BWTPosition _position; // Position reached within the buffer
  BWTPosition _bufferlen; // Number of char read in the last "read" call
  vector< NucleoCounter > _occurrencesBeforeStart; // PI vectore, as defined

 public:
  partialBWTReader ( string inputFilename );
  partialBWTReader ( string inputFilename, BWTPosition start, vector< NucleoCounter >& occurrencesBeforeStart );
  ~partialBWTReader ( );
  vector< NucleoCounter >& get_Pi( );
  BWTPosition get_position ( );
  bool move_to ( BWTPosition & p );

 private:
  // no need of copy ctor nor assignment operator
  partialBWTReader ( ) { }
  partialBWTReader ( const partialBWTReader& other ) 
    { }
  partialBWTReader& operator= ( const partialBWTReader& other )
    { return *this; }
};

#endif
