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
#ifndef ASQG_FMT_H
#define ASQG_FMT_H

#include <vector>
#include <string>
#include <ostream>

#include "types.h"

#define ASQG_FIELD_SEP '\t'
#define ASQG_FIELD_SEP_STR "\t"

void
print_vertex(std::ostream& out,
             const char* name, const char* seq);

void
print_vertex(std::ostream& out,
             const char* prefix, const SequenceNumber id,
             const char* seq);

void
print_edge(std::ostream& out,
           const std::string& source_id, const std::string& dest_id,
           const SequenceLength overlap, const SequenceLength readslen,
           const char reverse);


#endif
