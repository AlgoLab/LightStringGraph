#include "util.h"

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

ofstream& operator<<( ofstream& out, const EdgeInterval& edgeint )
{
  // First we store the _extendI interval
  BWTPosition begin = edgeint._extendI.get_begin( );
  out.write( (char *) &begin, sizeof( BWTPosition ) );
  BWTPosition end = edgeint._extendI.get_end( );
  out.write( (char *) &end, sizeof( BWTPosition ) );

  // Then we write the length of _suffixI (that should be the same as _len)
  unsigned long num_of_suffix = edgeint._suffixI.size( );
  out.write( (char *) &num_of_suffix, sizeof( unsigned long ) );
  // out.write( (char *) &( (unsigned long) edgeint._suffixI.size( ) ),
  // 	     sizeof( unsigned long ) );

  // Then, for each interval in _suffixI and for each length in _len we store
  // the interval and the related length.
  for( unsigned int i( 0 ); i < edgeint._suffixI.size( ); ++i )
    {
      begin = edgeint._suffixI[ i ]->get_begin( );
      out.write( (char *) &begin, sizeof( BWTPosition ) );
      end = edgeint._suffixI[ i ]->get_end( );
      out.write( (char *) &end, sizeof( BWTPosition ) );
      EdgeLength length = edgeint._len[ i ];
      out.write( (char *) &length, sizeof( EdgeLength ) );
      // out.write( (char *) &( edgeint._suffixI[ i ]->get_begin( ) ), sizeof( BWTPosition ) );
      // out.write( (char *) &( edgeint._len[ i ] ), sizeof ( EdgeLength ) );
    }
  out.flush( );
  return out;
}

ifstream& operator>>( ifstream& in, EdgeInterval** edgeint )
{
  // First we read the _extendI interval
  BWTPosition extend_b =0;
  in.read( (char *) &extend_b, sizeof( BWTPosition ) );
  // in.read( &extend_b, sizeof( BWTPosition ) );
  BWTPosition extend_e =0;
  in.read( (char *) &extend_e, sizeof( BWTPosition ) );
  //  in.read( &extend_e, sizeof( BWTPosition ) );
  
  // Then we read the number of intervals in _suffixI (an unsigned long)
  unsigned long length =0;
  in.read( (char *) &length, sizeof( unsigned long ) );
  //  in.read( &length, sizeof( unsigned long ) );
  
  // We read the first _suffixI (we are sure that at least ONE interval is in
  // _suffixI)
  BWTPosition suffix_b =0;
  in.read( (char *) &suffix_b, sizeof( BWTPosition ) );
  //  in.read( &suffix_b, sizeof( BWTPosition ) );
  BWTPosition suffix_e =0;
  in.read( (char *) &suffix_b, sizeof( BWTPosition ) );
  //  in.read( &suffix_e, sizeof( BWTPosition ) );
  EdgeLength suffix_len =0;
  in.read( (char *) &suffix_len, sizeof( EdgeLength ) );
  //  in.read( &suffix_len, sizeof( EdgeLength ) );

  if( extend_b != extend_e )
    {
      // Then we create the edgeinterval
      *edgeint = new EdgeInterval( extend_b, extend_e, suffix_b, suffix_e, suffix_len );

      // Finally, we read all the remaining intervals in _suffixI (if needed)
      for( unsigned long i( 1 ); i < length; ++i )
	{
	  in.read( (char *) &suffix_b, sizeof( BWTPosition ) );
	  in.read( (char *) &suffix_e, sizeof( BWTPosition ) );
	  in.read( (char *) &suffix_len, sizeof( EdgeLength ) );
	  QInterval* temp = new QInterval( suffix_b, suffix_e );
	  (*edgeint)->add_suffix_interval( temp, suffix_len );
	}
    }
  else
    {
      *edgeint = NULL;
    }
  return in;
}

ofstream& operator<<( ofstream& out, const JoinedQInterval& jint )
{
  // First we store the _int interval

  // std::cout << "WRITE JOINED Q INTERVAL " << jint._int.get_begin( )
  // 	    << " " << jint._int.get_end( ) << " "
  // 	    << jint._revint.get_begin( ) << " "
  // 	    << jint._revint.get_end( ) << std::endl;
  BWTPosition begin = jint._int.get_begin( );
  BWTPosition end = jint._int.get_end( );
  out.write( (char *) &begin, sizeof( BWTPosition ) );
  out.write( (char *) &end, sizeof( BWTPosition ) );
  // out.write( (char *) &(jint._int.get_begin( )),
  // 	     sizeof( BWTPosition ) );
  // out.write( (char *) &(jint._int.get_end( )),
  // 	     sizeof( BWTPosition ) );

  // Then we write the _revint interval
  begin = jint._revint.get_begin( );
  end = jint._revint.get_end( );
  out.write( (char *) &begin, sizeof( BWTPosition ) );
  out.write( (char *) &end, sizeof( BWTPosition ) );
  out.flush( );
  // out.write( (char *) &(jint._revint.get_begin( )), sizeof( BWTPosition ) );
  // out.write( (char *) &(jint._revint.get_end( )), sizeof( BWTPosition ) );

  return out;
}

ifstream& operator>>( ifstream& in, JoinedQInterval** jint )
{
  // Read the 4 positions ( _int.begin, _int.end, _revint.begin, _revint.end )
  BWTPosition ibegin=0, iend=0, rbegin=0, rend=0;
  in.read( (char *) &ibegin, sizeof( BWTPosition ) );
  in.read( (char *) &iend, sizeof( BWTPosition ) );
  in.read( (char *) &rbegin, sizeof( BWTPosition ) );
  in.read( (char *) &rend, sizeof( BWTPosition ) );
  if( ibegin != iend )
    {
      *jint = new JoinedQInterval( ibegin, iend, rbegin, rend );
    }
  else
    *jint = NULL;
  return in;
}
