#ifndef UTIL_H
#define UTIL_H

#include <types.h>
#include <iostream>
#include <cstdlib>

// convert nucleotide (enum) to char
char ntoc ( Nucleotide base )
{
  char ret ='N';

  switch ( base )
    {
    case BASE_A:
      ret = 'A';
      break;
    case BASE_C:
      ret = 'C';
      break;
    case BASE_G:
      ret = 'G';
      break;
    case BASE_T:
      ret = 'T';
      break;
    case BASE_N:
      ret = 'N';
      break;
    default:
      std::cerr << "ERROR: Can't convert Nucleotide int( " 
		<< base << " ) to char. Aborting." << std::endl;
      std::exit( -1 );
    }
  return ret;
}

// convert char to nucleotide
Nucleotide cton ( char c )
{
  Nucleotide ret = ALPHABET_SIZE;
  
  switch ( c )
    {
    case 'A':
      ret = BASE_A;
      break;
    case 'C':
      ret = BASE_C;
      break;
    case 'G':
      ret = BASE_G;
      break;
    case 'T':
      ret = BASE_T;
      break;
    case 'N':
      ret = BASE_N;
      break;
    default:
      std::cerr << "ERROR: Can't convert char( '" << c 
		<< "' ) to Nucleotide. Aborting." << std::endl;
      std::exit( -1 );
    }
  return ret;
}

#endif
