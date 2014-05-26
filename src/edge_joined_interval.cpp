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
// #include "edge_joined_interval.h"

// EdgeInterval::EdgeInterval( const BWTPosition& sbegin, const BWTPosition& send,
//                             const BWTPosition& ebegin, const BWTPosition& eend )
//   : _extendI( sbegin, send )
// {
//   QInterval* new_interval = new QInterval( ebegin, eend );
//   _suffixI.push_back( new_interval );
//   _len.push_back( 1 );
// }

// EdgeInterval::EdgeInterval( const BWTPosition& sbegin, const BWTPosition& send,
//                             const BWTPosition& ebegin, const BWTPosition& eend,
//                             const EdgeLength& len )
//   : _extendI( sbegin, send )
// {
//   QInterval* new_interval = new QInterval( ebegin, eend );
//   _suffixI.push_back( new_interval );
//   _len.push_back( len );
// }

// EdgeInterval::EdgeInterval( const BWTPosition& sbegin, const BWTPosition& send,
//                             vector< QInterval* >& old_suffix_i,
//                             vector< EdgeLength >& old_length )
//   : _extendI( sbegin, send )
// {
//   // Derived EdgeInterval
//   // copy the content of old_suffix_i
//   for( vector< QInterval* >::iterator it = old_suffix_i.begin( );
//        it != old_suffix_i.end( ); ++it )
//     {
//       QInterval* new_interval =
//         new QInterval( (*it)->get_begin( ), (*it)->get_end( ) );
//       _suffixI.push_back( new_interval );
//     }
//   for( vector< EdgeLength >::iterator it = old_length.begin( );
//        it != old_length.end( ); ++it )
//     {
//       _len.push_back( *it );
//     }
// }

// EdgeInterval::~EdgeInterval( )
// {
//   for( vector< QInterval* >::iterator it = _suffixI.begin( );
//        it != _suffixI.end( ); ++it )
//     delete *it;
//   _len.clear( );
//   _suffixI.clear( );
// }

// void EdgeInterval::add_suffix_interval( const QInterval* q_int, EdgeLength len )
// {
//   // Adds a suffix interal tu _suffixI
//   QInterval* new_interval = new QInterval( q_int->get_begin( ), q_int->get_end( ) );
//   _suffixI.push_back( new_interval );
//   _len.push_back( len );
// }

// vector< EdgeLength >& EdgeInterval::get_len( )
// {
//   return _len;
// }

// const QInterval& EdgeInterval::get_first_interval( ) const
// {
//   return _extendI;
// }

// vector< QInterval* >& EdgeInterval::get_second_interval( )
// {
//   return _suffixI;
// }
