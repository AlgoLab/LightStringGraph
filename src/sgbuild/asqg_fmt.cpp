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
             const std::string& name, const char* seq) {
  out << "VT" << ASQG_FIELD_SEP << name << ASQG_FIELD_SEP << seq << '\n';
}

static void
print_entry(std::ostream& out,
	    const SequenceLength start, const SequenceLength end,
	    const SequenceLength lenseq)
{
  out << PRINT_SL(start) << ' '
      << PRINT_SL(end) << ' '
      << PRINT_SL(lenseq) << ' ';
}

void
print_edge(std::ostream& out,
           const std::string& source_id, const std::string& dest_id,
           const SequenceLength overlap, const SequenceLength readslen,
           const char reverse) {
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
  const bool reverse_source = (reverse & 0b1);
  const bool reverse_dest   = (reverse & 0b10);
  out << "ED" << ASQG_FIELD_SEP
      << source_id << ' '
      << dest_id << ' ';
  if(!reverse_source)
    print_entry(out, overlap, readslen -1, readslen);
  else
    print_entry(out, 0, readslen - overlap -1, readslen);
  if(reverse_dest)
    print_entry(out, overlap, readslen -1, readslen);
  else
    print_entry(out, 0, readslen - overlap -1, readslen);

  if(reverse_source ^ reverse_dest)
    out << "1 0" << std::endl;
  else
    out << "0 0" << std::endl;
}
