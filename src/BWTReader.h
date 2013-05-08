#ifndef BWTREADER_H
#define BWTREADER_H

#include <vector>
#include <string>
#include <cstdlib>

#include <types.h>
#include <partialBWTReader.h>

using std::vector;
using std::string;

class BWTReader
{
 private:
  vector< string > _filenamesIN;   // Names of BWT sections
  unsigned short _nextBWTFilename; // index of next BWTFilename in
				   // _filenamesIN
  partialBWTReader* _currentBWT;
  
 public:
  BWTReader ( vector< string >& filenamesIN );
  ~BWTReader ( );
  BWTPosition get_position ( ) const;
  bool move_to ( BWTPosition& p );
  vector< NucleoCounter >& get_Pi ( );
  vector< NucleoCounter >* get_C ( ); // Gets the values of the count function

 private:
  void reset ( );

  // no need of copy ctor nor assignment operator
  BWTReader ( ) { };
  BWTReader ( const BWTReader& other )
  { }
  BWTReader& operator= ( const BWTReader& other )
  { return *this; }

};

#endif
