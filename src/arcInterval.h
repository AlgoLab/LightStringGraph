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
 * along with LightStringGraph.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

#ifndef ARC_INTERVAL_H
#define ARC_INTERVAL_H

#include "types.h"
#include "q_interval.h"

struct SeedInterval
{
  const SequenceNumber begin;
  const SequenceNumber end;

  SeedInterval(const SequenceNumber& begin_,
					const SequenceNumber& end_)
		:begin(begin_), end(end_)
  {}

  const size_t size() const {
	 return end-begin;
  }

  bool operator==(const SeedInterval& rhs) const
  {
    return (this->begin == rhs.begin && this->end == rhs.end);
  }
  
};

struct ArcInterval
{
  const QInterval es_interval;
  const SequenceLength ext_len;
  const SeedInterval seed_int;

  // Constructor
  ArcInterval(const QInterval& es_interval_,
              const SequenceLength& ext_len_,
              const SeedInterval& seed_int_)
		:es_interval(es_interval_), ext_len(ext_len_), seed_int(seed_int_)
  {}

  bool operator==(const ArcInterval& rhs) const
  {
    return (this->es_interval == rhs.es_interval &&
            this->ext_len == rhs.ext_len         &&
            this->seed_int == rhs.seed_int);
  }
};

#endif
