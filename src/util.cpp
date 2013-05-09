#include <util.h>

char ntoc ( Nucleotide base )
{
  char ret ='N';

  switch ( base )
    {
    case BASE_A:
      ret = 'A';
      break;
    case BASE_$:
      ret = '$';
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
    case BASE_Z:
      ret = 'Z';
      break;
    // case NOT_IN_ALPHABET:
    //   ret = '-';
    //   break;
    default:
      std::cerr << "ERROR: Can't convert Nucleotide int( "
		<< base << " ) to char. Aborting." << std::endl;
      std::exit( -1 );
    }
  return ret;
}

Nucleotide cton ( char c )
{
  Nucleotide ret = ALPHABET_SIZE;
  
  switch ( c )
    {
    case '$':
      ret = BASE_$;
      break;
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
    case 'Z':
      ret = BASE_Z;
      break;
    default:
#ifndef DEBUG
      std::cerr << "ERROR: Received char '" << c << "' " 
		<< " int( " << int(c) << " ). Which base is it?"
		<< std::endl;
#endif
      //      ret = NOT_IN_ALPHABET;
      break;
      // std::cerr << "ERROR: Can't convert char( '" << c 
      // 		<< "' ) to Nucleotide. Aborting." << std::endl;
      // std::exit( -1 );
    }
  return ret;
}
