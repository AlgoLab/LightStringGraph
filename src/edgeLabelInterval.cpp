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

#include "edgeLabelInterval.h"

EdgeLabelInterval::EdgeLabelInterval( const QInterval& forward, const QInterval& reverse )
  : _forward_interval( forward ), _reverse_interval( reverse )
{

}

EdgeLabelInterval::~EdgeLabelInterval( )
{

}

const QInterval& EdgeLabelInterval::get_label( ) const
{
  return _forward_interval;
}

const QInterval& EdgeLabelInterval::get_reverse_label( ) const
{
  return _reverse_interval;
}

bool EdgeLabelInterval::operator==(const EdgeLabelInterval& rhs) const
{
  return ((_forward_interval == rhs.get_label()) &&
          (_reverse_interval == rhs.get_reverse_label()));
}

bool EdgeLabelInterval::operator!=(const EdgeLabelInterval& rhs) const
{
  return (!((*this) == rhs));
}

EdgeLabelInterval& EdgeLabelInterval::operator=( const EdgeLabelInterval& other)
{
  _forward_interval = other.get_label();
  _reverse_interval = other.get_reverse_label();
  return *this;
}

bool EdgeLabelInterval::operator>( const EdgeLabelInterval& rhs) const
{
  if((_forward_interval.get_end() > rhs.get_label().get_end()) ||
     ((_forward_interval.get_end() == rhs.get_label().get_end()) &&
      (_forward_interval.get_begin() < rhs.get_label().get_begin())))
    return true;
  else
    return false;
}

bool EdgeLabelInterval::operator<( const EdgeLabelInterval& rhs) const
{
  return rhs > *this;
}
