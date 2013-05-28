#ifndef GSA_READER_H
#define GSA_READER_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "types.h"

using std::vector;
using std::string;

class GSAReader
{
private:
  std::ifstream _inputFile;

public:
  GSAReader( const string& );
  ~GSAReader( );
  vector< GSAEntry* >* get( const BWTPosition&, const BWTPosition& );

private:
  GSAReader( ) { };
  GSAReader( const GSAReader& other ) { };
  GSAReader& operator=( const GSAReader& other ) { return *this; };
  
};

#endif
