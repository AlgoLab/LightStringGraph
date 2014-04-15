#ifndef EXT_SORT_H
#define EXT_SORT_H

#include <climits>
#include <cmath>
#include <cstdlib>
#include <cstdio>

#ifdef DEBUG_VERBOSE
#include <bitset>
#endif

#include "types.h"
#include "util.h"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

#define BITS_OF_BWT_POSITION 	sizeof(BWTPosition) * CHAR_BIT
#define MERGED_FILES 		std::string("merged_files")
#define	P_FILE 			std::string("pfile")

// Looks like bitshift has some problem with 64 bits
static const BWTPosition extsmask[BITS_OF_BWT_POSITION] =
  {
    0b0000000000000000000000000000000000000000000000000000000000000001,
    0b0000000000000000000000000000000000000000000000000000000000000010,
    0b0000000000000000000000000000000000000000000000000000000000000100,
    0b0000000000000000000000000000000000000000000000000000000000001000,
    0b0000000000000000000000000000000000000000000000000000000000010000,
    0b0000000000000000000000000000000000000000000000000000000000100000,
    0b0000000000000000000000000000000000000000000000000000000001000000,
    0b0000000000000000000000000000000000000000000000000000000010000000,
    0b0000000000000000000000000000000000000000000000000000000100000000,
    0b0000000000000000000000000000000000000000000000000000001000000000,
    0b0000000000000000000000000000000000000000000000000000010000000000,
    0b0000000000000000000000000000000000000000000000000000100000000000,
    0b0000000000000000000000000000000000000000000000000001000000000000,
    0b0000000000000000000000000000000000000000000000000010000000000000,
    0b0000000000000000000000000000000000000000000000000100000000000000,
    0b0000000000000000000000000000000000000000000000001000000000000000,
    0b0000000000000000000000000000000000000000000000010000000000000000,
    0b0000000000000000000000000000000000000000000000100000000000000000,
    0b0000000000000000000000000000000000000000000001000000000000000000,
    0b0000000000000000000000000000000000000000000010000000000000000000,
    0b0000000000000000000000000000000000000000000100000000000000000000,
    0b0000000000000000000000000000000000000000001000000000000000000000,
    0b0000000000000000000000000000000000000000010000000000000000000000,
    0b0000000000000000000000000000000000000000100000000000000000000000,
    0b0000000000000000000000000000000000000001000000000000000000000000,
    0b0000000000000000000000000000000000000010000000000000000000000000,
    0b0000000000000000000000000000000000000100000000000000000000000000,
    0b0000000000000000000000000000000000001000000000000000000000000000,
    0b0000000000000000000000000000000000010000000000000000000000000000,
    0b0000000000000000000000000000000000100000000000000000000000000000,
    0b0000000000000000000000000000000001000000000000000000000000000000,
    0b0000000000000000000000000000000010000000000000000000000000000000,
    0b0000000000000000000000000000000100000000000000000000000000000000,
    0b0000000000000000000000000000001000000000000000000000000000000000,
    0b0000000000000000000000000000010000000000000000000000000000000000,
    0b0000000000000000000000000000100000000000000000000000000000000000,
    0b0000000000000000000000000001000000000000000000000000000000000000,
    0b0000000000000000000000000010000000000000000000000000000000000000,
    0b0000000000000000000000000100000000000000000000000000000000000000,
    0b0000000000000000000000001000000000000000000000000000000000000000,
    0b0000000000000000000000010000000000000000000000000000000000000000,
    0b0000000000000000000000100000000000000000000000000000000000000000,
    0b0000000000000000000001000000000000000000000000000000000000000000,
    0b0000000000000000000010000000000000000000000000000000000000000000,
    0b0000000000000000000100000000000000000000000000000000000000000000,
    0b0000000000000000001000000000000000000000000000000000000000000000,
    0b0000000000000000010000000000000000000000000000000000000000000000,
    0b0000000000000000100000000000000000000000000000000000000000000000,
    0b0000000000000001000000000000000000000000000000000000000000000000,
    0b0000000000000010000000000000000000000000000000000000000000000000,
    0b0000000000000100000000000000000000000000000000000000000000000000,
    0b0000000000001000000000000000000000000000000000000000000000000000,
    0b0000000000010000000000000000000000000000000000000000000000000000,
    0b0000000000100000000000000000000000000000000000000000000000000000,
    0b0000000001000000000000000000000000000000000000000000000000000000,
    0b0000000010000000000000000000000000000000000000000000000000000000,
    0b0000000100000000000000000000000000000000000000000000000000000000,
    0b0000001000000000000000000000000000000000000000000000000000000000,
    0b0000010000000000000000000000000000000000000000000000000000000000,
    0b0000100000000000000000000000000000000000000000000000000000000000,
    0b0001000000000000000000000000000000000000000000000000000000000000,
    0b0010000000000000000000000000000000000000000000000000000000000000,
    0b0100000000000000000000000000000000000000000000000000000000000000,
    0b1000000000000000000000000000000000000000000000000000000000000000
  };

inline BWTPosition _getmask(short int value, short int offset, short int step)
{
  BWTPosition m =0;
  for(short int i=0; i < step; ++i)
    {
      if(value & (1 << i))
	m |= (extsmask[offset + i]);
    }
  return m;
}

// Position to mask
inline short int _ptm(BWTPosition pos, short int offset, short int step)
{
  short int p =0;
  for(short int i=0; i<step; ++i)
    {
      if(pos & extsmask[offset + i])
	p |= (1 << i);
    }
  return p;
}

#define	MASK(value, offset)	_getmask(value, offset, CHAR_BIT)

#define PTM(pos, offset)	_ptm	(pos,	offset, CHAR_BIT)

// External memory sorting of extension pairs

std::ifstream* ext_sort(const std::string& filname_in );

#endif