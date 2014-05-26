// -*- c++ -*-

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
 * along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

#ifndef LCPITERATOR_H
#define LCPITERATOR_H

#include <istream>

#include "types.h"
#include "MultiFileIterator.h"

struct LCPFileValue {
  LCPValue v;
};

std::istream& operator>> (std::istream& is, LCPFileValue& li);

template <>
struct get_value<LCPFileValue,LCPValue> {
  LCPValue operator()(const LCPFileValue lfv) {
	 return LCPValue(lfv.v);
  }
};


typedef MultiFileIterator<LCPFileValue, LCPValue> LCPIterator;

#endif
