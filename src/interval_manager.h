// -*- c++ -*-

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
  vector< string >                          _filenames;     // Input filenames
  unsigned short                            _nextInputFile; // Index of next file to open in _filenames
  std::ifstream*                            _inputFile;       // Current input stream
  vector< std::ofstream* >                  _outputFiles;       // New intervals (interval_t) will be
  vector< interval_t* >                     _buffer;        // current intervals
  typename vector< interval_t* >::iterator  _nextInterval;     // next interval in buffer

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
        std::exit( -1 );
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
        std::exit( -1 );
      }
    _nextInputFile = 0;
    _populate_buffer( );
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
    for( typename vector< interval_t* >::iterator it = _buffer.begin( );
         it != _buffer.end( ); ++it )
      {
        delete (*it);
        (*it) = NULL;
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
            std::exit( -1 );
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
    interval_t* i = ( _buffer.size( ) == 0 ) ? NULL : *_nextInterval++;
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
    (*(_outputFiles[ (int) n ])) << i;
    // (*(_outputFiles[ (int) n ])).write( (char *) (&i), sizeof( interval_t ) );
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
  } // _init_new_outputfiles

  // Read new intervals and store them in _buffer
  void _populate_buffer()
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
           ( ( _inputFile && ((*_inputFile) >> i ) ) || ( _nextInputFile < _filenames.size( ) ) ) )
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
            _buffer.push_back( i );
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
