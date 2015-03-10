/**
 *
 *                     LightStringGraph
 *
 *            Lightweight String Graph Construction.
 *
 * Copyright (C) 2013, 2014 Stefano Beretta, Yuri Pirola, Marco Previtali
 *
 * Distributed under the terms of the GNU General Public License (or the Lesser
 * GPL).
 *
 * This file is part of LightStringGraph.
 *
 * LighStringGraph is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * LightStringGraph is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with LightStringGraph.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/
#include "util.h"

const NuclConv NuclConv::_conv;

NuclConv::NuclConv() {
  for (int i = 0; i <= UCHAR_MAX; ++i) {
    switch ( i ) {
      case CHAR_BASE_$:                       _cton[i]= BASE_$; break;
      case CHAR_BASE_A: case CHAR_BASE_A_LOW: _cton[i]= BASE_A; break;
      case CHAR_BASE_C: case CHAR_BASE_C_LOW: _cton[i]= BASE_C; break;
      case CHAR_BASE_G: case CHAR_BASE_G_LOW: _cton[i]= BASE_G; break;
      case CHAR_BASE_T: case CHAR_BASE_T_LOW: _cton[i]= BASE_T; break;
      case CHAR_BASE_N: case CHAR_BASE_N_LOW: _cton[i]= BASE_N; break;
      case CHAR_BASE_Z: case CHAR_BASE_Z_LOW: _cton[i]= BASE_Z; break;
      default: _cton[i]= ALPHABET_SIZE;                         break;
    }
  }
  _ntoc[BASE_$]= CHAR_BASE_$;
  _ntoc[BASE_A]= CHAR_BASE_A;
  _ntoc[BASE_C]= CHAR_BASE_C;
  _ntoc[BASE_G]= CHAR_BASE_G;
  _ntoc[BASE_N]= CHAR_BASE_N;
  _ntoc[BASE_T]= CHAR_BASE_T;
  _ntoc[BASE_Z]= CHAR_BASE_Z;
}


template <int bytenum>
struct integer_type {
};

template <>
struct integer_type<1> {
  typedef uint8_t type;
};

template <>
struct integer_type<2> {
  typedef uint16_t type;
};
template <>
struct integer_type<4> {
  typedef uint32_t type;
};
// Shift 64 is war
// template <>
// struct integer_type<8> {
//   typedef uint64_t type;
// };

template <int byte_num>
void writelen(ofstream& out, BWTPosition len)
{
  const uint8_t shift_amount= (byte_num * 8 -1);
  const BWTPosition mask= ((1ull << shift_amount) -1);
  while(len)
    {
      typename integer_type<byte_num>::type towrite=(len & mask);
      len >>= shift_amount;
      if (len)
        towrite |= (1ull << shift_amount);
      out.write((char *)&towrite, byte_num);
    }
}

template <int byte_num>
BWTPosition readlen(ifstream& in)
{
  const unsigned int shift_amount= (byte_num * 8 -1);
  const BWTPosition mask= ((1ull << shift_amount) -1);
  BWTPosition p =0;
  typename integer_type<byte_num>::type partialread =0;
  uint8_t iter=0;
  do{
    partialread =0;
    in.read((char *)&partialread, byte_num);
    if(in.gcount() < byte_num)
      return 0;
    p |= ((partialread & mask) << (shift_amount * iter));
    ++iter;
  }while(partialread & (1ull << shift_amount));
  return p;
}

ofstream& operator<<( ofstream& out, const QInterval& i )
{
  BWTPosition begin = i.get_begin( );
  BWTPosition end = i.get_end( );
  out.write( (char *) &begin, sizeof( BWTPosition ) );
  writelen<2>(out, end-begin);
  return out;
}

ifstream& operator>>( ifstream& in, QInterval*& i )
{
  BWTPosition begin =0, end=0;
  in.read((char *) &begin, sizeof( BWTPosition ) );
  if(in.gcount() == 0) { i = NULL; return in; }
  end = begin + readlen<2>(in);
  if(end == begin) i = NULL;
  else i = new QInterval( begin, end );
  return in;
}

ofstream& operator<<( ofstream& out, const GSAEntry& g )
{
  out.write( (char *) &g.sa, sizeof( SequenceLength ) );
  return out;
}

ifstream& operator>>( ifstream& in, GSAEntry& g )
{
  in.read( (char *) &g.sa, sizeof( SequenceLength ) );
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
  writelen<1>(out, s.end-s.begin);
  return out;
}

ifstream& operator>>( ifstream& in, SeedInterval*& s )
{
  SequenceNumber begin =0, end=0;
  in.read(reinterpret_cast<char*>(&begin), sizeof( SequenceNumber ) );
  if(in.gcount() == 0) { s = NULL; return in; }
  end = begin + readlen<1>(in);
  if(end == begin) s = NULL;
  else s = new SeedInterval( begin, end );
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
  delete label;
  delete reverse;
  return in;
}
