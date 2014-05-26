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
// #ifndef EDGE_JOINED_INTERVAL_H
// #define EDGE_JOINED_INTERVAL_H

// #include <vector>
// #include <fstream>
// #include <iostream>
// #include <limits>

// #include "q_interval.h"
// #include "types.h"

// using std::vector;
// using std::ofstream;
// using std::ifstream;

// /*******************************************************************************
//  * EdgeInterval stores 2 intervals on BWT' and should be used during the right
//  * extension procedure.
//  *******************************************************************************/

// struct PODEdgeInterval {
//   BWTPosition b1;
//   BWTPosition e1;
//   BWTPosition b2;
//   BWTPosition e2;

//   PODEdgeInterval& operator=(const PODEdgeInterval& x)
//   {
//     b1 = x.b1;
//     e1 = x.e1;
//     b2 = x.b2;
//     e2 = x.e2;
//     return *this;
//   }
// };

// struct SortPODEdgeInterval {

//   bool operator()(const PODEdgeInterval& x, const PODEdgeInterval& y) const
//   {
//     return (x.b1 < y.b1);
//   }

//   static PODEdgeInterval min_value()
//   {
//     PODEdgeInterval x;
//     x.b1 = std::numeric_limits<BWTPosition>::min();
//     x.e1 = std::numeric_limits<BWTPosition>::min();
//     x.b2 = std::numeric_limits<BWTPosition>::min();
//     x.e2 = std::numeric_limits<BWTPosition>::min();
//     return x;
//   }

//   static PODEdgeInterval max_value()
//   {
//     PODEdgeInterval x;
//     x.b1 = std::numeric_limits<BWTPosition>::max();
//     x.e1 = std::numeric_limits<BWTPosition>::max();
//     x.b2 = std::numeric_limits<BWTPosition>::max();
//     x.e2 = std::numeric_limits<BWTPosition>::max();
//     return x;
//   }
// };

// class EdgeInterval
// {
// protected:
//   vector< QInterval* > _suffixI; // intervals on BWT' relative to reads that
//   // have Q as suffix
//   QInterval _extendI; // interval on BWT' relative to reads that have Q as
//   // prefix (the interval we should extend)
//   vector< EdgeLength > _len; // Edge length

// public:
//   // ctor
//   EdgeInterval( const BWTPosition &, const BWTPosition &,
//                 const BWTPosition &, const BWTPosition & );
//   EdgeInterval( const BWTPosition &, const BWTPosition &,
//                 const BWTPosition &, const BWTPosition &,
//                 const EdgeLength& );
//   EdgeInterval( ) : _suffixI( ), _extendI( 0, 0 ), _len( 0 ) { };
//   EdgeInterval( const BWTPosition&, const BWTPosition&, vector< QInterval* >&,
//                 vector< EdgeLength >& );

//   // destructor
//   ~EdgeInterval( );

//   friend ofstream& operator<<( ofstream&, const EdgeInterval& );
//   friend ifstream& operator>>( ifstream&, EdgeInterval** );

//   void add_suffix_interval( const QInterval*, const EdgeLength );

//   vector< EdgeLength >& get_len( );
//   const QInterval & get_first_interval( ) const;
//   vector< QInterval* >& get_second_interval( );

// private:
//   EdgeInterval( const EdgeInterval& other) :
//     _suffixI( ), _extendI( 0, 0 ), _len( 0 ) { };

//   EdgeInterval& operator=( const EdgeInterval& other ) { return *this; }
// };

// #endif
