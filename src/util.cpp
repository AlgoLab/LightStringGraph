#include "util.h"

char ntoc ( Nucleotide base )
{
  char ret ='N';

  switch ( base )
    {
    case BASE_$:
      ret = '$';
      break;
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
    case BASE_Z:
      ret = 'Z';
      break;
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
    case 'a':
      ret = BASE_A;
      break;
    case 'C':
    case 'c':
      ret = BASE_C;
      break;
    case 'G':
    case 'g':
      ret = BASE_G;
      break;
    case 'T':
    case 't':
      ret = BASE_T;
      break;
    case 'N':
    case 'n':
      ret = BASE_N;
      break;
    case 'Z':
    case 'z':
      ret = BASE_Z;
      break;
    default:
#ifndef DEBUG
      std::cerr << "ERROR: Received char '" << c << "' "
                << " int( " << int(c) << " ). Which base is it?"
                << std::endl;
#endif
      break;
    }
  return ret;
}

// ofstream& operator<<( ofstream& out, const EdgeInterval& edgeint )
// {
//   // First we store the _extendI interval
//   BWTPosition begin = edgeint._extendI.get_begin( );
//   out.write( (char *) &begin, sizeof( BWTPosition ) );
//   BWTPosition end = edgeint._extendI.get_end( );
//   out.write( (char *) &end, sizeof( BWTPosition ) );

//   // Then we write the length of _suffixI (that should be the same as _len)
//   unsigned int num_of_suffix = edgeint._suffixI.size( );
//   out.write( (char *) &num_of_suffix, sizeof( unsigned int ) );

//   // Then, for each interval in _suffixI and for each length in _len we store
//   // the interval and the related length.
//   for( unsigned int i( 0 ); i < edgeint._suffixI.size( ); ++i )
//     {
//       begin = edgeint._suffixI[ i ]->get_begin( );
//       end = edgeint._suffixI[ i ]->get_end( );
//       EdgeLength length = edgeint._len[ i ];
//       if( begin != end )
//         {
//           out.write( (char *) &begin, sizeof( BWTPosition ) );
//           out.write( (char *) &end, sizeof( BWTPosition ) );
//           out.write( (char *) &length, sizeof( EdgeLength ) );
//         }
//     }
//   out.flush( );
//   return out;
// }

// ifstream& operator>>( ifstream& in, EdgeInterval* edgeint )
// {
//   // First we read the _extendI interval
//   BWTPosition extend_b =0;
//   in.read( (char *) &extend_b, sizeof( BWTPosition ) );
//   BWTPosition extend_e =0;
//   in.read( (char *) &extend_e, sizeof( BWTPosition ) );

//   // Then we read the number of intervals in _suffixI (an unsigned long)
//   unsigned int length =0;
//   in.read( (char *) &length, sizeof( unsigned int ) );

//   // We read the first _suffixI (we are sure that at least ONE interval is in
//   // _suffixI)
//   BWTPosition suffix_b =0;
//   in.read( (char *) &suffix_b, sizeof( BWTPosition ) );
//   BWTPosition suffix_e =0;
//   in.read( (char *) &suffix_e, sizeof( BWTPosition ) );
//   EdgeLength suffix_len =0;
//   in.read( (char *) &suffix_len, sizeof( EdgeLength ) );

//   if( extend_b != extend_e )
//     {
//       // Then we create the edgeinterval
//       edgeint = new EdgeInterval( extend_b, extend_e, suffix_b, suffix_e, suffix_len );

//       // Finally, we read all the remaining intervals in _suffixI (if needed)
//       for( unsigned long i( 1 ); i < length; ++i )
//         {
//           in.read( (char *) &suffix_b, sizeof( BWTPosition ) );
//           in.read( (char *) &suffix_e, sizeof( BWTPosition ) );
//           in.read( (char *) &suffix_len, sizeof( EdgeLength ) );
//           if( suffix_e > suffix_b )
//             {
//               QInterval temp( suffix_b, suffix_e );
//               edgeint->add_suffix_interval( &temp, suffix_len );
//             }
//         }
//     }
//   else
//     {
//       edgeint = NULL;
//     }
//   return in;
// }

// ifstream& operator>>( ifstream& in, EdgeInterval** edgeint )
// {
//   // First we read the _extendI interval
//   BWTPosition extend_b =0;
//   in.read( (char *) &extend_b, sizeof( BWTPosition ) );
//   BWTPosition extend_e =0;
//   in.read( (char *) &extend_e, sizeof( BWTPosition ) );

//   // Then we read the number of intervals in _suffixI (an unsigned long)
//   unsigned int length =0;
//   in.read( (char *) &length, sizeof( unsigned int ) );

//   // We read the first _suffixI (we are sure that at least ONE interval is in
//   // _suffixI)
//   BWTPosition suffix_b =0;
//   in.read( (char *) &suffix_b, sizeof( BWTPosition ) );
//   BWTPosition suffix_e =0;
//   in.read( (char *) &suffix_e, sizeof( BWTPosition ) );
//   EdgeLength suffix_len =0;
//   in.read( (char *) &suffix_len, sizeof( EdgeLength ) );

//   if( extend_b != extend_e )
//     {
//       // Then we create the edgeinterval
//       (*edgeint) = new EdgeInterval( extend_b, extend_e, suffix_b, suffix_e, suffix_len );

//       // Finally, we read all the remaining intervals in _suffixI (if needed)
//       for( unsigned long i( 1 ); i < length; ++i )
//         {
//           in.read( (char *) &suffix_b, sizeof( BWTPosition ) );
//           in.read( (char *) &suffix_e, sizeof( BWTPosition ) );
//           in.read( (char *) &suffix_len, sizeof( EdgeLength ) );
//           if( suffix_e > suffix_b )
//             {
//               QInterval temp( suffix_b, suffix_e );
//               (*edgeint)->add_suffix_interval( &temp, suffix_len );
//             }
//         }
//     }
//   else
//     {
//       edgeint = NULL;
//     }
//   return in;
// }

// ofstream& operator<<( ofstream& out, const JoinedQInterval& jint )
// {
//   // First we store the _int interval
//   BWTPosition begin = jint._int.get_begin( );
//   BWTPosition end = jint._int.get_end( );
//   out.write( (char *) &begin, sizeof( BWTPosition ) );
//   out.write( (char *) &end, sizeof( BWTPosition ) );

//   // Then we write the _revint interval
//   begin = jint._revint.get_begin( );
//   end = jint._revint.get_end( );
//   out.write( (char *) &begin, sizeof( BWTPosition ) );
//   out.write( (char *) &end, sizeof( BWTPosition ) );
//   out.flush( );

//   return out;
// }

// ifstream& operator>>( ifstream& in, JoinedQInterval** jint )
// {
//   // Read the 4 positions ( _int.begin, _int.end, _revint.begin, _revint.end )
//   BWTPosition ibegin=0, iend=0, rbegin=0, rend=0;
//   in.read( (char *) &ibegin, sizeof( BWTPosition ) );
//   in.read( (char *) &iend, sizeof( BWTPosition ) );
//   in.read( (char *) &rbegin, sizeof( BWTPosition ) );
//   in.read( (char *) &rend, sizeof( BWTPosition ) );
//   if( ibegin != iend )
//     {
//       *jint = new JoinedQInterval( ibegin, iend, rbegin, rend );
//     }
//   else
//     *jint = NULL;
//   return in;
// }

ofstream& operator<<( ofstream& out, const QInterval& i )
{
  BWTPosition begin = i.get_begin( );
  BWTPosition end = i.get_end( );
  out.write( (char *) &begin, sizeof( BWTPosition ) );
  out.write( (char *) &end, sizeof( BWTPosition ) );
  out.flush( );

  return out;
}

ifstream& operator>>( ifstream& in, QInterval*& i )
{
  BWTPosition begin =0, end=0;
  in.read((char *) &begin, sizeof( BWTPosition ) );
  if(in.gcount() == 0) { i = NULL; return in; }
  in.read((char *) &end, sizeof( BWTPosition ) );
  if(in.gcount() == 0) { i = NULL; return in; }
  i = new QInterval( begin, end );
  return in;
}

ofstream& operator<<( ofstream& out, const GSAEntry& g )
{
  out.write( (char *) &g.sa, sizeof( SequenceLength ) );
  out.write( (char *) &g.numSeq, sizeof( SequenceNumber ) );
  out.flush( );

  return out;
}

ifstream& operator>>( ifstream& in, GSAEntry& g )
{
  in.read( (char *) &g.sa, sizeof( SequenceLength ) );
  in.read( (char *) &g.numSeq, sizeof( SequenceNumber ) );
  return in;
}

ofstream& operator<<( ofstream& out, const ArcInterval& a )
{
  out << a.es_interval;
  out.write( reinterpret_cast<const char *>(&a.ext_len), sizeof( a.ext_len ) );
  out << a.seed_int;

  return out;
}

ifstream& operator>>( ifstream& in, ArcInterval*& a )
{
  QInterval* es_int= NULL;
  in >> es_int;
  if(es_int == NULL) { a = NULL; return in; }

  SequenceLength len=0;
  in.read( reinterpret_cast<char *>(&len), sizeof( SequenceLength ) );
  if(in.gcount() == 0) { a = NULL; return in; }

  SeedInterval* s= NULL;
  in >> s;
  if(s == NULL) { a = NULL; return in; }

  a = new ArcInterval( *es_int, len, *s );
  delete es_int;
  delete s;
  return in;
}

std::string now( const char* format = "%c" )
{
  std::time_t t = std::time(0) ;
  char cstr[128] ;
  std::strftime( cstr, sizeof(cstr), format, std::localtime(&t) ) ;
  return cstr ;
}

ofstream& operator<<( ofstream& out, const SeedInterval& s )
{
  out.write( reinterpret_cast<const char*>(&s.begin), sizeof( SequenceNumber ) );
  out.write( reinterpret_cast<const char*>(&s.end), sizeof( SequenceNumber ) );
  return out;
}

ifstream& operator>>( ifstream& in, SeedInterval*& s )
{
  SequenceNumber begin =0, end=0;
  in.read(reinterpret_cast<char*>(&begin), sizeof( SequenceNumber ) );
  if(in.gcount() == 0) { s = NULL; return in; }
  in.read(reinterpret_cast<char*>(&end), sizeof( SequenceNumber ) );
  if(in.gcount() == 0) { s = NULL; return in; }
  s = new SeedInterval( begin, end );
  return in;
}

ofstream& operator<<( ofstream& out, const EdgeLabelInterval& e )
{
  out << e.get_label( );
  out << e.get_reverse_label( );
  return out;
}

ifstream& operator>>( ifstream& in, EdgeLabelInterval*& e )
{
  QInterval *label=NULL, *reverse=NULL;
  in >> label;
  if(!label) { e =NULL; return in; }
  in >> reverse;
  if(!reverse){ e = NULL; return in; }
  e = new EdgeLabelInterval( *label, *reverse );
  return in;
}

// ifstream& operator>>( ifstream& in, EdgeLabelInterval** e )
// {
//   QInterval *label=NULL, *reverse=NULL;
//   in >> label;
//   if(!label) { *e =NULL; return in; }
//   in >> reverse;
//   if(!reverse){ *e = NULL; return in; }
//   *e = new EdgeLabelInterval( *label, *reverse );
//   return in;
// }

// LEGACY
// void checkIfIrreducible( SGraph& s, Precedencies& p, GSAEntry* prefixI, GSAEntry* suffixI, EdgeLength& len )
// {
//   // Check if the edge beetween suffixI->numSeq and prefixI->numSeq is the best
//   // found until now with length len
//   if( s.find( suffixI->numSeq ) == s.end( ) )
//     {
//       // Add
//       SGEdge* e = new SGEdge( );
//       e->first_read = suffixI->numSeq;
//       e->second_read = prefixI->numSeq;
//       e->len = len;
//       s.insert( std::pair< unsigned int, SGEdge* >( e->first_read, e ) );

//       if( p.find( prefixI->numSeq ) != p.end( ) )
//         p.find( prefixI->numSeq )->second.push_back( suffixI->numSeq );
//       else
//         {
//           vector< unsigned int > v;
//           v.push_back( suffixI->numSeq );
//           p.insert( std::pair< unsigned int, vector< unsigned int > >( prefixI->numSeq,
//                                                                        v ) );
//         }
//     }
//   else
//     {
//       SGEdge* e1 = (* (s.find( suffixI->numSeq ))).second;
//       if( e1->len > len )
//         {
//           e1->len = len;
//           e1->first_read = suffixI->numSeq;
//           e1->second_read = prefixI->numSeq;
//           if( p.find( prefixI->numSeq ) != p.end( ) )
//             p.find( prefixI->numSeq )->second.push_back( suffixI->numSeq );
//           else
//             {
//               vector< unsigned int > v;
//               v.push_back( suffixI->numSeq );
//               p.insert( std::pair< unsigned int, vector< unsigned int > >( prefixI->numSeq,
//                                                                            v ) );
//             }
//         }
//     }
// }
