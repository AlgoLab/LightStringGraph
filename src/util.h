#ifndef UTIL_H
#define UTIL_H

#include <types.h>
#include <iostream>
#include <cstdlib>

// convert nucleotide (enum) to char
char ntoc ( Nucleotide base );

// convert char to nucleotide
Nucleotide cton ( char c );

#endif
