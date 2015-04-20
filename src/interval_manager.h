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
#include <cstdio>

#include "util.h"

using std::vector;
using std::string;

template< class interval_t > class IntervalManagerI
{
protected:
  vector< string >                          _filenames;     // Input filenames
  unsigned short                            _nextInputFile; // Index of next file to open in _filenames
  std::ifstream*                            _inputFile;       // Current input stream
  vector< FILE* >                  _outputFiles;       // New intervals (interval_t) will be

public:

  IntervalManagerI(vector< string >& filenames)
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
  }

  virtual ~IntervalManagerI()
  {
    for ( vector< FILE* >::iterator it = _outputFiles.begin( );
          it != _outputFiles.end( ); ++it )
      {
        fclose(*it);
        (*it) = NULL;
      }
    if( _inputFile != NULL )
      {
        _inputFile->close( );
        delete _inputFile;
        _inputFile = NULL;
      }
    for( typename vector< string >::iterator it = _filenames.begin();
         it != _filenames.end(); ++it)
      {
        remove((*it).c_str());
        remove( std::string((*it) + "_next").c_str());
      }
  }

  virtual void swap_files()
  {
    if( _inputFile != NULL )
      {
        _inputFile->close();
        delete _inputFile;
        _inputFile = NULL;
      }
    for ( vector< FILE* >::iterator it = _outputFiles.begin( );
          it != _outputFiles.end( );
          ++it)
      {
        fflush(*it);
        fclose(*it);
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
  }

  virtual interval_t* get_next_interval() =0;
  virtual bool add_interval(const interval_t& i, const Nucleotide n) =0;

  // Return _nextInputFile
  unsigned short get_next_input_file( )
  {
    return _nextInputFile;
  }

protected:
  virtual void _populate_buffer() =0;
  virtual void _init_new_outputfiles() =0;
};

template< class interval_t > class IntervalManager
  : public IntervalManagerI< interval_t >
{
private:
  vector< interval_t* >                     _buffer;        // current intervals
  typename vector< interval_t* >::iterator  _nextInterval;     // next interval in buffer

public:

  // Instantiate a Manager over filenames files
  // TODO: create a new constructor that accept a prefix (string) and
  //  an int and automagically creates the vector
  IntervalManager( vector< string >& filenames )
    : IntervalManagerI<interval_t>(filenames)
  {
    _populate_buffer( );
    _init_new_outputfiles( );
  } // IntervalManager

  // Destructor
  ~IntervalManager( )
  {
    for( typename vector< interval_t* >::iterator it = _buffer.begin( );
         it != _buffer.end( ); ++it )
      {
        delete (*it);
        (*it) = NULL;
      }
  } // ~IntervalManager

  // Get next interval. If no intervals are left, return NULL
  interval_t* get_next_interval ( )
  {
    if( _nextInterval == _buffer.end( ) )
      {
        _populate_buffer( );
      }
    interval_t* i = ( _buffer.size( ) == 0 ) ? NULL : *_nextInterval++;
    return i;
  } // get_next_interval

  // Append i to outputfile[ n ]
  bool add_interval ( const interval_t& i, const Nucleotide n )
  {
    if ( (unsigned int) n >= this->_outputFiles.size() )
      {
        DEBUG_LOG( "ERROR: Can't add interval_t to file #" << n );
        return false;
      }
    write_interval(this->_outputFiles[ (int) n ], i);
    // (*(_outputFiles[ (int) n ])).write( (char *) (&i), sizeof( interval_t ) );
    return true;
  } // add_interval

protected:
  // Read new intervals and store them in _buffer
  virtual void _populate_buffer()
  {
    for( typename vector< interval_t* >::iterator it = _buffer.begin( );
         it != _buffer.end( ); ++it )
      {
        if(*it != NULL)
          delete *it;
      }
    _buffer.clear( );

    interval_t* i = NULL;
    while( _buffer.size( ) < IM_BUFFERSIZE &&
           ( ( this->_inputFile && ((*(this->_inputFile)) >> i ) ) || ( this->_nextInputFile < this->_filenames.size( ) ) ) )
      {
        if( i == NULL )
          {
            // end of file
            this->_inputFile->close( );
            delete this->_inputFile;
            this->_inputFile = new std::ifstream( this->_filenames[ this->_nextInputFile++ ].c_str( ),
                                                  std::ios::binary );
          }
        else
          {
            _buffer.push_back( i );
            i = NULL;
          }
      }
    _nextInterval = _buffer.begin( );
  }

  virtual void _init_new_outputfiles()
  {
    for ( vector< string >::iterator it = this->_filenames.begin( ); it != this->_filenames.end( );
          ++it)
      {
        std::ostringstream outfilename;
        outfilename << (*it) << "_next";
        this->_outputFiles.push_back( fopen( outfilename.str().c_str(), "wb") );
      }
  }
};


// Class that lets you read and save intervals from and to files.

template< class interval_t > class IntervalManagerRLE
  : public IntervalManagerI< interval_t >
{
private:
  struct IntervalRun {
    interval_t* _interval; // Interval
    long        _mult;     // Multiplicity

    IntervalRun() : _interval(NULL), _mult(0) {};
    IntervalRun(interval_t* t) : _interval(t), _mult(1) {};
    IntervalRun(interval_t* t, long mult) : _interval(t), _mult(mult) {};
  };

  vector< IntervalRun >                     _buffer;        // current intervals
  typename vector< IntervalRun >::iterator  _nextInterval;     // next interval in buffer
  vector< IntervalRun >                     _outIntervals;  // Buffer for intervals to store in external memory

public:

  // Instantiate a Manager over filenames files
  // TODO: create a new constructor that accept a prefix (string) and
  //  an int and automagically creates the vector
  IntervalManagerRLE( vector< string >& filenames )
    : IntervalManagerI<interval_t>(filenames)
  {
    _outIntervals = vector< IntervalRun >(ALPHABET_SIZE);
    _populate_buffer( );
    _init_new_outputfiles( );
  } // IntervalManager

  // Destructor
  ~IntervalManagerRLE()
  {
    for( typename vector< IntervalRun >::iterator it = _buffer.begin( );
         it != _buffer.end( ); ++it )
      {
        delete it->_interval;
        it->_interval = NULL;
      }
  } // ~IntervalManager

  // Delete current intervals, move files from output to input and create new
  // output files
  virtual void swap_files( )
  {
    for(int n = BASE_A; n != ALPHABET_SIZE; n++)
      {
        if(_outIntervals[n]._mult !=0)
          {
            write_interval(this->_outputFiles[n], *(this->_outIntervals[n]._interval));
            fwrite((char*)&(this->_outIntervals[n]._mult), sizeof(long), 1, this->_outputFiles[n]);
          }
      }
    IntervalManagerI<interval_t>::swap_files();
  } // swap_files

  // Get next interval. If no intervals are left, return NULL
  interval_t* get_next_interval ( )
  {
    if(_nextInterval == _buffer.end( ) )
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
    if ( (unsigned int) n >= this->_outputFiles.size() )
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
        write_interval(this->_outputFiles[(int) n], *(_outIntervals[n]._interval));
        fwrite((char *)&(_outIntervals[n]._mult), sizeof(long), 1, this->_outputFiles[(int) n]);
        delete (_outIntervals[n]._interval);
        interval_t* t = new interval_t(i);
        _outIntervals[n] = IntervalRun(t);
      }
    return true;
  } // add_interval

protected:
  // Initalize new output files
  virtual void _init_new_outputfiles( )
  {
    for ( vector< string >::iterator it = this->_filenames.begin( ); it != this->_filenames.end( );
          ++it)
      {
        std::ostringstream outfilename;
        outfilename << (*it) << "_next";
        this->_outputFiles.push_back( fopen( outfilename.str().c_str(), "wb") );
      }
    for(int n = BASE_A; n != ALPHABET_SIZE; ++n)
      {
        if(_outIntervals[n]._interval != NULL)
          delete (_outIntervals[n]._interval);
        _outIntervals[n] = IntervalRun();
      }
  } // _init_new_outputfiles

  // Read new intervals and store them in _buffer
  virtual void _populate_buffer()
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
           (( this->_inputFile && ((*(this->_inputFile)) >> i ) && ((*(this->_inputFile)).read((char *)&runlength, sizeof(long)))) ||
            ( this->_nextInputFile < this->_filenames.size( ))))
      {
        if( i == NULL )
          {
            // end of file
            this->_inputFile->close( );
            delete this->_inputFile;
            this->_inputFile = new std::ifstream( this->_filenames[ this->_nextInputFile++ ].c_str( ),
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
  }
};

#endif
