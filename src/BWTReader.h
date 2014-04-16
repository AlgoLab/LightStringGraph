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
  vector< string >    _filenamesIN;       // names of BWT sections
  unsigned short      _nextBWTFilename;   // index of next BWTFilename in _filenamesIoN
  partialBWTReader*   _currentBWT;      // current BWT section

public:
  // Constructor
  BWTReader ( vector< string >& filenamesIN );

  // Destructor
  ~BWTReader ( );

  // Returns current position
  BWTPosition get_position ( ) const;

  // Move to position p in this BWT and update nucleotide occurrences accordingly.
  // Return value:
  // - true if p can be reached
  // - false otherwise
  bool move_to ( BWTPosition& p );

  // Move to position p in this BWT, update nucleotide occurrences accordingly, and save
  // indexes of the occurrences of BASE_$ in prefixpos.
  // Return value:
  // - true if p can be reached
  // - false otherwise
  bool move_to_storing_sent( BWTPosition p, BWTPExtVect& prefixpos );

  // Return PI vector (occurrences of every nucleotide before current position).
  vector< NucleoCounter >& get_Pi ( );

  // Return C (count)
  vector< NucleoCounter >* get_C ( );

  // Reset BWT
  void reset ( );

private:
  // no need of copy ctor nor assignment operator
  BWTReader ( ) { };
  BWTReader ( const BWTReader& other )
  { }
  BWTReader& operator= ( const BWTReader& other )
  { return *this; }

};

#endif
