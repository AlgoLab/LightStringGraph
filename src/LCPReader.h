// -*- c++ -*-
#ifndef LCPREADER_H
#define LCPREADER_H

#include <iostream>
#include <vector>
#include <string>

#include "types.h"
#include "partialLCPReader.h"

class LCPReader
{
private:
  std::vector< std::string >  _filenamesIN;
  unsigned short              _nextLCPIndex;
  partialLCPReader*           _currentLCP;
  LCPValue                    _lastLCP;


public:
  // Constructor
  LCPReader(std::vector< std::string >& filenamesIN);

  // Destructor
  ~LCPReader();

  // Return current position
  BWTPosition get_position() const;

  // Move to the next position if possibile.
  // Return value:
  //  - true if the position is reached
  //  - false otherwise
  bool next();

  // Get the current value of LCP
  LCPValue get() const;

private:
  // no need of copy ctor nor assignement operator
  LCPReader() { }
  LCPReader(const LCPReader& other) { }
  LCPReader& operator=(const LCPReader& other) { return *this; }
};

#endif
