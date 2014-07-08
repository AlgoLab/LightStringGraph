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

#ifndef INTERVAL_MANAGER_H
#define INTERVAL_MANAGER_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "util.h"

using std::vector;
using std::string;

// Class that lets you read and save intervals from and to files.

template< class interval_t > class IntervalManager
{
private:
  struct IntervalRun {
    interval_t* _interval; // Interval
    long        _mult;     // Multiplicity

    IntervalRun() : _interval(NULL), _mult(0) {};
    IntervalRun(interval_t* t) : _interval(t), _mult(1) {};
    IntervalRun(interval_t* t, long mult) : _interval(t), _mult(mult) {};
  };

  vector< string >                          _filenames;     // Input filenames
  unsigned short                            _nextInputFile; // Index of next file to open in _filenames
  std::ifstream*                            _inputFile;       // Current input stream
  vector< IntervalRun >                     _buffer;        // current intervals
  typename vector< IntervalRun >::iterator  _nextInterval;     // next interval in buffer
  vector< std::ofstream* >                  _outputFiles;       // New intervals (interval_t) will be
  vector< IntervalRun >                     _outIntervals;  // Buffer for intervals to store in external memory

public:

  // Instantiate a Manager over filenames files
  // TODO: create a new constructor that accept a prefix (string) and
  //  an int and automagically creates the vector
  IntervalManager( vector< string >& filenames )
  {
    if ( filenames.size( ) == 0 )
      {
        std::cerr << "ERROR: Can't initialize an interval manager without any "
                  << "filename." << std::endl;
        _MY_FAIL;
      }
    for ( vector< string >::iterator it = filenames.begin( );
          it != filenames.end( ); ++it )
      {
        _filenames.push_back( *it );
        // touch files
        std::ifstream t((*it).c_str(), std::ios::binary);
        if(!t.good())
          {
            DEBUG_LOG_VERBOSE( *it << " does not exists" );
            std::ofstream temp ((*it).c_str(), std::ios::binary);
            temp.close();
          }
        else
          {
            DEBUG_LOG(*it << " does exists. This shouldn't happen and the program " \
                      << "will probably give erroneous results." );
          }
        t.close();
      }

    _inputFile = new std::ifstream( _filenames[ 0 ].c_str( ), std::ios::binary );
    if( _inputFile->fail() )
      {
        std::cerr << "ERROR: Can't open file : " << _filenames[ 0 ] << std::endl
                  << "Aborting..." << std::endl;
        delete _inputFile;
        _inputFile = NULL;
        _MY_FAIL;
      }
    _nextInputFile = 0;
    _populate_buffer( );
    _outIntervals = vector<IntervalRun>(ALPHABET_SIZE);
    _init_new_outputfiles( );
  } // IntervalManager

  // Destructor
  ~IntervalManager( )
  {
    for ( vector< std::ofstream* >::iterator it = _outputFiles.begin( );
          it != _outputFiles.end( ); ++it )
      {
        (*it)->close( );
        delete (*it);
        (*it) = NULL;
      }
    if( _inputFile != NULL )
      {
        _inputFile->close( );
        delete _inputFile;
        _inputFile = NULL;
      }
    for( typename vector< IntervalRun >::iterator it = _buffer.begin( );
         it != _buffer.end( ); ++it )
      {
        delete it->_interval;
        it->_interval = NULL;
      }

    for( typename vector< string >::iterator it = _filenames.begin();
         it != _filenames.end(); ++it)
      {
        remove((*it).c_str());
        remove( std::string((*it) + "_next").c_str());
      }
  } // ~IntervalManager

  // Delete current intervals, move files from output to input and create new
  // output files
  void swap_files( )
  {
    if( _inputFile != NULL )
      {
        _inputFile->close();
        delete _inputFile;
        _inputFile = NULL;
      }
    for(int n = BASE_A; n != ALPHABET_SIZE; n++)
      {
        if(_outIntervals[n]._mult !=0)
          {
            (*(_outputFiles[n])) << *(_outIntervals[n]._interval);
            (*(_outputFiles[n])).write((char*)&(_outIntervals[n]._mult), sizeof(long));
          }
      }
    for ( vector< std::ofstream* >::iterator it = _outputFiles.begin( );
          it != _outputFiles.end( );
          ++it)
      {
        (*it)->flush();
        (*it)->close();
        delete (*it);
      }
    _outputFiles.clear();
    for ( vector< string >::iterator it = _filenames.begin( );
          it != _filenames.end( );
          ++it)
      {
        std::ostringstream nextfile;
        nextfile << (*it) << "_next";
        remove( (*it).c_str() );
        if( rename( nextfile.str().c_str(), (*it).c_str() ) )
          {
            std::cerr << "ERROR: Can't rename " << nextfile.str() << " to " << *it
                      << std::endl << "Aborting..." << std::endl;
            _MY_FAIL;
          }
        remove( nextfile.str().c_str() );
        DEBUG_LOG_VERBOSE( "Renamed file " << nextfile.str() << " to " << *it);
      }
    _init_new_outputfiles( );
    _inputFile = new std::ifstream( _filenames[ 0 ].c_str( ), std::ios::binary );
    _nextInputFile = 1;
    _populate_buffer( );
  } // swap_files

  // Get next interval. If no intervals are left, return NULL
  interval_t* get_next_interval ( )
  {
    if( _nextInterval == _buffer.end( ) )
      {
        _populate_buffer( );
      }
    interval_t* i = NULL;
    if(_buffer.size() == 0)
      i = NULL;
    else
      {
        i = _nextInterval->_interval;
        if(--(_nextInterval->_mult) == 0)
          _nextInterval++;
      }
    return i;
  } // get_next_interval

  // Append i to outputfile[ n ]
  bool add_interval ( const interval_t& i, const Nucleotide n )
  {
    if ( (unsigned int) n >= _outputFiles.size() )
      {
        DEBUG_LOG( "ERROR: Can't add interval_t to file #" << n );
        return false;
      }
    if(_outIntervals[n]._interval == NULL)
      {
        interval_t* t = new interval_t(i);
        _outIntervals[n] = IntervalRun(t);
      }
    else if(i == *(_outIntervals[n]._interval))
        ++(_outIntervals[n]._mult);
    else
      {
        (*(_outputFiles[(int) n])) << *(_outIntervals[n]._interval);
        (*(_outputFiles[(int) n])).write((char *)&(_outIntervals[n]._mult), sizeof(long));
        delete (_outIntervals[n]._interval);
        interval_t* t = new interval_t(i);
        _outIntervals[n] = IntervalRun(t);
      }
    return true;
  } // add_interval

  // Return _nextInputFile
  unsigned short get_next_input_file( )
  {
    return _nextInputFile;
  }

private:
  // Initalize new output files
  void _init_new_outputfiles ( )
  {
    for ( vector< string >::iterator it = _filenames.begin( ); it != _filenames.end( );
          ++it)
      {
        std::ostringstream outfilename;
        outfilename << (*it) << "_next";
        _outputFiles.push_back( new std::ofstream( outfilename.str().c_str(),
                                                   std::ios::binary | std::ios::app) );
      }
    for(int n = BASE_A; n != ALPHABET_SIZE; ++n)
      {
        if(_outIntervals[n]._interval != NULL)
          delete (_outIntervals[n]._interval);
        _outIntervals[n] = IntervalRun();
      }
  } // _init_new_outputfiles

  // Read new intervals and store them in _buffer
  void _populate_buffer()
  {
    for(typename vector< IntervalRun >::iterator it = _buffer.begin();
        it != _buffer.end(); ++it)
      {
        if(it->_interval != NULL)
          delete it->_interval;
      }
    _buffer.clear( );

    interval_t* i = NULL;
    long runlength =0;
    while( _buffer.size( ) < IM_BUFFERSIZE &&
           (( _inputFile && ((*_inputFile) >> i ) && ((*_inputFile).read((char *)&runlength, sizeof(long)))) ||
            ( _nextInputFile < _filenames.size( ))))
      {
        if( i == NULL )
          {
            // end of file
            _inputFile->close( );
            delete _inputFile;
            _inputFile = new std::ifstream( _filenames[ _nextInputFile++ ].c_str( ),
                                            std::ios::binary );
          }
        else
          {
            IntervalRun x(i, runlength);
            _buffer.push_back( x );
            i = NULL;
          }
      }
    _nextInterval = _buffer.begin( );
    // // TODO: Refactor this method, there are too many nested if/else
    // bool isThereMore = true;
    // while ( _buffer.size() < BUFFERSIZE && isThereMore)
    //   {
    //    if( _inputFile != NULL && _inputFile->eof() )
    //      {
    //        _inputFile->close();
    //        delete _inputFile;
    //        if( _nextInputFile < _filenames.size() )
    //          {
    //      _inputFile = new std::ifstream( _filenames[ _nextInputFile++ ].c_str( ),
    //              std::ios::binary );
    //      // ++_nextInputFile;
    //          }
    //        else
    //          {
    //      _inputFile = NULL;
    //          }
    //      }
    //    else
    //      {
    //        if( _inputFile != NULL )
    //          {
    //      interval_t* intervalRead = new interval_t();
    //      _inputFile->read( (char *) intervalRead, sizeof( interval_t ) );
    //      isThereMore = ( _inputFile->gcount() > 0 ) ? true : false ;
    //      if( !isThereMore )
    //        {
    //          delete intervalRead;
    //          _inputFile->close();
    //          delete _inputFile;
    //          _inputFile = NULL;
    //          // ++_nextInputFile;
    //          if( _nextInputFile < _filenames.size() )
    //            {
    //        _inputFile = new std::ifstream( _filenames[ _nextInputFile++ ].c_str(),
    //                std::ios::binary );
    //        isThereMore = true;
    //            }
    //        }
    //      else
    //        {
    //          _buffer.push_back( intervalRead );
    //        }
    //          }
    //        else
    //          {
    //      isThereMore = false;
    //          }
    //      }
    //   }
    // _nextInterval = _buffer.begin( );
  }

};

#endif
