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
// #include <joined_q_int_manager.h>

// // JoinedQIntervalManager::JoinedQIntervalManager ( vector< string >& filenames )
// // {
// //   if ( filenames.size( ) == 0 )
// //     {
// //       std::cerr << "ERROR: Can't initalize a JoinedQIntervalManager without any filename."
// //     << std::endl;
// //       std::exit( -1 );
// //     }

// //   for ( vector< string >::iterator it = filenames.begin( );
// //   it != filenames.end( );
// //   ++it)
// //     {
// //       _filenames.push_back( *it );
// //       // touch files
// //       std::ofstream temp ((*it).c_str(), std::ios::binary);
// //       temp.close();
// //     }
// //   _inputFile = new std::ifstream( _filenames[ 0 ].c_str( ),
// //           std::ios::binary );
// //   _nextInputFile = 1;

// //   if( _inputFile->fail() )
// //     {
// //       std::cerr << "ERROR: Can't open file : " << _filenames[ 0 ] << std::endl
// //     << "Aborting..." << std::endl;
// //       delete _inputFile;
// //       _inputFile = NULL;
// //       std::exit( -1 );
// //     }
// //   _init_new_outputfiles( );
// // }

// // JoinedQIntervalManager::~JoinedQIntervalManager ( )
// // {
// //   for ( vector< std::ofstream* >::iterator it = _outputFiles.begin( );
// //   it != _outputFiles.end( );
// //   ++it )
// //     {
// //       (*it)->close();
// //       delete *it;
// //       *it = NULL;
// //     }
// //   if( _inputFile != NULL )
// //     {
// //       _inputFile->close();
// //       delete _inputFile;
// //       _inputFile = NULL;
// //     }
// //   for( vector< JoinedQInterval* >::iterator it = _buffer.begin();
// //        it != _buffer.end();
// //        ++it )
// //     {
// //       delete *it;
// //       *it = NULL;
// //     }
// // }

// // /* get_next_interval returns the next interval if it exists, NULL
// //    otherwise */
// // JoinedQInterval* JoinedQIntervalManager::get_next_interval ( )
// // {
// //   if( _nextInterval == _buffer.end( ) )
// //     {
// //       for( vector< JoinedQInterval* >::iterator it = _buffer.begin( );
// //      it != _buffer.end( );
// //      ++it )
// //   {
// //     if( (*it) != NULL )
// //       {
// //         delete *it;
// //       }
// //   }
// //       _buffer.clear();
// //       _populate_buffer( );
// //       _nextInterval = _buffer.begin();
// //       if( _buffer.size( ) == 0 )
// //   return NULL;
// //     }
// //   JoinedQInterval* i = *_nextInterval;
// //   ++_nextInterval;
// //   return i;
// // }

// // void JoinedQIntervalManager::swap_files( )
// // {
// //   if( _inputFile != NULL )
// //     {
// //       _inputFile->close();
// //       delete _inputFile;
// //       _inputFile = NULL;
// //     }
// //   for ( vector< std::ofstream* >::iterator it = _outputFiles.begin( );
// //   it != _outputFiles.end( );
// //   ++it)
// //     {
// //       (*it)->flush();
// //       (*it)->close();
// //       delete *it;
// //     }
// //   _outputFiles.clear();
// //   for ( vector< string >::iterator it = _filenames.begin( );
// //   it != _filenames.end( );
// //   ++it)
// //     {
// //       std::ostringstream nextfile;
// //       nextfile << *it << "_next";
// //       remove( (*it).c_str() );
// //       if( rename( nextfile.str().c_str(), (*it).c_str() ) )
// //   {
// //     std::cerr << "ERROR: Can't rename " << nextfile.str() << " to " << *it
// //         << std::endl << "Aborting..." << std::endl;
// //     std::exit( -1 );
// //   }
// //       remove( nextfile.str().c_str() );
// // #ifdef DEBUG_VERBOSE
// //   std::cerr << "Renamed file " << nextfile.str() << " to " << *it << std::endl;
// // #endif
// //     }
// //   _init_new_outputfiles( );
// //   _inputFile = new std::ifstream( _filenames[ 0 ].c_str( ),
// //           std::ios::binary );
// //   _nextInputFile = 1;
// // }


// // bool JoinedQIntervalManager::add_q_interval ( JoinedQInterval& i, Nucleotide n )
// // {
// //   if ( (unsigned int) n >= _outputFiles.size() )
// //     {
// // #ifdef DEBUG_VERBOSE
// //       std::cerr << "ERROR: Can't add joined-q-interval [ "
// //     << i.get_interval().get_begin()
// //     << " - " << i.get_interval().get_end() << " , "
// //     << i.get_reverse_interval().get_begin() << " - "
// //     << i.get_reverse_interval().get_end() << " ] to file #" << n
// //     << std::endl;
// // #endif
// //       return false;
// //     }
// // #ifdef DEBUG_VERBOSE
// //   std::cerr << "WRITING " << i.get_interval().get_begin() << " "
// //       << i.get_interval().get_end() << " "
// //       << i.get_reverse_interval().get_begin() << " "
// //       << i.get_reverse_interval().get_end() << " TO " << ntoc( n ) << std::endl;
// // #endif
// //   (*(_outputFiles[ (int) n ])).write( (char *) (&i), sizeof( JoinedQInterval ) );
// //   return true;
// // }

// // void JoinedQIntervalManager::_init_new_outputfiles ( )
// // {
// //   for ( vector< string >::iterator it = _filenames.begin( );
// //   it != _filenames.end( );
// //   ++it)
// //     {
// //       std::ostringstream outfilename;
// //       outfilename << *it << "_next";
// //       _outputFiles.push_back( new std::ofstream( outfilename.str().c_str(),
// //              std::ios::binary | std::ios::app) );
// //     }
// // }

// // void JoinedQIntervalManager::_populate_buffer()
// // {
// //   // TODO: Refactor this method, there are too many nested if/else
// //   bool isThereMore = true;
// //   while ( _buffer.size() < BUFFERSIZE && isThereMore)
// //     {
// //       if( _inputFile != NULL && _inputFile->eof() )
// //   {
// //     _inputFile->close();
// //     delete _inputFile;
// //     if( _nextInputFile < _filenames.size() )
// //       {
// //         _inputFile = new std::ifstream( _filenames[ _nextInputFile++ ].c_str( ),
// //                 std::ios::binary );
// //         // ++_nextInputFile;
// //       }
// //     else
// //       {
// //         _inputFile = NULL;
// //       }
// //   }
// //       else
// //   {
// //     if( _inputFile != NULL )
// //       {
// //         JoinedQInterval* intervalRead = new JoinedQInterval(0, 0, 0, 0);
// //         _inputFile->read( (char *) intervalRead, sizeof( JoinedQInterval ) );
// //         isThereMore = ( _inputFile->gcount() > 0 ) ? true : false ;
// //         if( !isThereMore )
// //     {
// //       delete intervalRead;
// //       _inputFile->close();
// //       delete _inputFile;
// //       _inputFile = NULL;
// //       // ++_nextInputFile;
// //       if( _nextInputFile < _filenames.size() )
// //         {
// //           _inputFile = new std::ifstream( _filenames[ _nextInputFile++ ].c_str(),
// //                   std::ios::binary );
// //           isThereMore = true;
// //         }
// //     }
// //         else
// //     {
// //       _buffer.push_back( intervalRead );
// //     }
// //       }
// //     else
// //       {
// //         isThereMore = false;
// //       }
// //   }
// //     }
// //   _nextInterval = _buffer.begin( );
// // }
