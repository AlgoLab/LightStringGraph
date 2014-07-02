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
#include "asqg_fmt.h"

void
print_vertex(std::ostream& out,
             const char* name, const char* seq) {
  out << "VT\t" << name << '\t' << seq << '\n';
}

void
print_edge(std::ostream& out,
           const std::vector< std::string >& ids,
           const SequenceNumber source, const SequenceNumber dest,
           const SequenceLength overlap, const SequenceLength readslen) {
  // Fields:
  // 0.  string ED
  // 1.  sequence 1 name
  // 2.  sequence 2 name
  // 3.  sequence 1 overlap start (0 based)
  // 4.  sequence 1 overlap end (inclusive)
  // 5.  sequence 1 length
  // 6.  sequence 2 overlap start (0 based)
  // 7.  sequence 2 overlap end (inclusive)
  // 8.  sequence 2 length
  // 9.  sequence 2 orientation (1 for reversed with respect to sequence 1)
  // 10. number of differences in overlap (0 for perfect overlaps, which is the default).
  out << "ED "
      << ids[source] << ' '
      << ids[dest] << ' '
      << PRINT_SL(overlap) << ' '
      << PRINT_SL(readslen-1) << ' '
      << PRINT_SL(readslen) << ' '
      << "0 "
      << PRINT_SL(readslen - overlap - 1) << ' '
      << PRINT_SL(readslen) << ' '
      << "0 0\n";
}
