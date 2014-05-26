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

/* #ifndef Q_INT_MANAGER_H */
/* #define Q_INT_MANAGER_H */

/* /\* #include "interval_manager.h" *\/ */
/* /\* #include "joined_q_interval.h" *\/ */

/* /\* typedef IntervalManager< JoinedQInterval > JoinedQIntervalManager; *\/ */

/* /\* #include <vector> *\/ */
/* /\* #include <string> *\/ */
/* /\* #include <cstdlib> *\/ */
/* /\* #include <iostream> *\/ */
/* /\* #include <sstream> *\/ */
/* /\* #include <cstdio> *\/ */
/* /\* #include <fstream> *\/ */

/* /\* #include <joined_q_interval.h> *\/ */
/* /\* #include <types.h> *\/ */
/* /\* #include <util.h> *\/ */

/* /\* using std::vector; *\/ */
/* /\* using std::string; *\/ */

/* /\* class JoinedQIntervalManager *\/ */
/* /\* { *\/ */
/* /\* private: *\/ */
/* /\*   vector< string > _filenames; // Input filenames *\/ */
/* /\*   unsigned short _nextInputFile; *\/ */
/* /\*   std::ifstream* _inputFile; *\/ */
/* /\*   vector< std::ofstream* > _outputFiles; // new Q-intervals will be *\/ */
/* /\*             // written here *\/ */
/* /\*   vector< JoinedQInterval* > _buffer; *\/ */
/* /\*   vector< JoinedQInterval* >::iterator _nextInterval; *\/ */

/* /\* public: *\/ */
/* /\*   // Instantiate a Manager over filenames files *\/ */
/* /\*   // TODO: create a new constructor that accept a prefix (string) and *\/ */
/* /\*   // an int and automagically creates the vector *\/ */
/* /\*   JoinedQIntervalManager ( vector< string >& filenames ); *\/ */

/* /\*   // Move next q-intervals to current q-intervals (so we can read *\/ */
/* /\*   // them) and create new output files *\/ */
/* /\*   void swap_files ( ); *\/ */

/* /\*   // Get next interval. If no intervals are left, return NULL *\/ */
/* /\*   JoinedQInterval* get_next_interval ( ); *\/ */

/* /\*   // Append i to outputfile[ n ] *\/ */
/* /\*   bool add_q_interval ( JoinedQInterval& i, Nucleotide n ); *\/ */

/* /\*   // Destructor *\/ */
/* /\*   ~JoinedQIntervalManager ( ); *\/ */

/* /\* private: *\/ */

/* /\*   // Add BUFFERSIZE joined_q_intervals to _buffer *\/ */
/* /\*   // BUFFERSIZE should be defined in types.h *\/ */
/* /\*   void _populate_buffer ( ); *\/ */

/* /\*   // Create new outputfiles (filename + "_next") *\/ */
/* /\*   void _init_new_outputfiles ( ); *\/ */


/* /\*   JoinedQIntervalManager ( )  *\/ */
/* /\*   { } *\/ */
/* /\*   JoinedQIntervalManager ( JoinedQIntervalManager& other ) *\/ */
/* /\*   { } *\/ */
/* /\*   JoinedQIntervalManager& operator= ( JoinedQIntervalManager& other ) *\/ */
/* /\*   { return *this; } *\/ */
/* /\* }; *\/ */

/* #endif */
