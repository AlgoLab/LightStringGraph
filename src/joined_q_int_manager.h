#ifndef Q_INT_MANAGER_H
#define Q_INT_MANAGER_H

#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <fstream>

#include <joined_q_interval.h>
#include <types.h>
#include <util.h>

using std::vector;
using std::string;

class JoinedQIntervalManager
{
private:
  vector< string > _filenames;
  unsigned short _nextInputFile;
  std::ifstream* _inputFile;
  vector< std::ofstream* > _outputFiles;
  vector< JoinedQInterval* > _buffer;
  vector< JoinedQInterval* >::iterator _nextInterval;

public:
  JoinedQIntervalManager ( vector< string >& filenames );
  void swap_files ( );
  JoinedQInterval* get_next_interval ( );
  bool add_q_interval ( JoinedQInterval& i, Nucleotide n );
  ~JoinedQIntervalManager ( );

private:
  void _populate_buffer ( );
  void _init_new_outputfiles ( );
  JoinedQIntervalManager ( ) 
  { }
  JoinedQIntervalManager ( JoinedQIntervalManager& other )
  { }
  JoinedQIntervalManager& operator= ( JoinedQIntervalManager& other )
  { return *this; }
};

#endif
