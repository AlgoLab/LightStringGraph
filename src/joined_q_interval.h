// // -*- c++ -*-

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

//
//
// LEGACY
//
//

// #ifndef JOINED_Q_INTERVAL_H
// #define JOINED_Q_INTERVAL_H

// #include <vector>
// #include <fstream>

// #include "q_interval.h"

// using std::vector;
// using std::ofstream;
// using std::ifstream;

// // JoinedQInterval represent a couple of q-intervals q1-int on BWT and
// // q2-int on BWT' such that q2 = rev(q1).

// class JoinedQInterval
// {
// protected:
//   QInterval _int, _revint; // int is on BWT while revint is on BWT'

// public:
//   // ctor
//   JoinedQInterval ( const BWTPosition & begin, const BWTPosition & end,
//                     const BWTPosition & revbegin, const BWTPosition & revend );
//   JoinedQInterval ( ) :  _int( 0, 0), _revint( 0, 0 ) { };
//   // destructor
//   ~JoinedQInterval ( );

//   BWTPosition get_size ( ) const;
//   const QInterval & get_interval ( ) const;
//   const QInterval & get_reverse_interval ( ) const;

//   friend ofstream& operator<<( ofstream&, const JoinedQInterval& );
//   friend ifstream& operator>>( ifstream&, JoinedQInterval** );

//   /* // Backward extend this interval with n and return the new */
//   /* // JoinedQInterval for nQ and rev(Q)n */
//   /* JoinedQInterval* backward_extend_with ( Nucleotide n ) const; */

//   /* // Backward extend this interval with every possibile nucleotide in */
//   /* // the alphabet */
//   /* vector< JoinedQInterval* >* backward_extend_all ( ) const; */

// private:
//   // no need of copy ctor nor assignment operator
//   JoinedQInterval ( const JoinedQInterval& other ) :
//     _int(0,0), _revint(0,0)
//   { };
//   JoinedQInterval& operator= ( const JoinedQInterval& other )
//   { return *this; };
// };

// #endif
