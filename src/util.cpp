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

#include <cstring>
#include <cstdio>

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


void
write_interval( gzFile fout, const QInterval& i )
{
  gzwrite(fout, (char *) &i.begin, sizeof(BWTPosition));
  writelen<2>(fout, i.size());
}

bool
read_interval ( gzFile fin, QInterval& i )
{
  const size_t gcount= gzread( fin, (char *)&i.begin, sizeof( BWTPosition ));
  if (gcount != sizeof( BWTPosition) ) { return false; }
  i.end = i.begin + readlen<2>(fin);
  return i.size() > 0;
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

void
write_interval( gzFile fout, const ArcInterval& a )
{
  write_interval(fout, a.es_interval);
  gzwrite( fout, reinterpret_cast<const char *>(&a.ext_len), sizeof( a.ext_len ));
  write_interval(fout, a.seed_int);
}

bool
read_interval ( gzFile fin, ArcInterval& a )
{
  if (!read_interval(fin, a.es_interval)) return false;

  const size_t readbytes= gzread( fin, reinterpret_cast<char *>(&a.ext_len),
                                 sizeof( SequenceLength ));
  if (readbytes != sizeof(SequenceLength) ) { return false; }

  if (!read_interval(fin, a.seed_int)) { return false; }

  return true;
}

std::string now( const char* format = "%c" )
{
  std::time_t t = std::time(0) ;
  char cstr[128] ;
  std::strftime( cstr, sizeof(cstr), format, std::localtime(&t) ) ;
  return cstr ;
}

void
write_interval(gzFile fout, const SeedInterval& s )
{
  gzwrite( fout, reinterpret_cast<const char*>(&s.begin), sizeof(SequenceNumber));
  writelen<1>(fout, s.len);
}

bool
read_interval ( gzFile fin, SeedInterval& s )
{
  const size_t gcount= gzread( fin, reinterpret_cast<char *>(&s.begin), sizeof( SequenceNumber ));
  if (gcount != sizeof(SequenceNumber)) { return false; }
  s.len = readlen<1>(fin);
  return true;
}

void
write_interval( gzFile fout, const EdgeLabelInterval& e )
{
  write_interval( fout, e.label );
  write_interval( fout, e.reverse_label );
}

bool
read_interval ( gzFile fin, EdgeLabelInterval& e )
{
  if (!read_interval(fin, e.label)) return false;
  if (!read_interval(fin, e.reverse_label)) return false;
  return true;
}
