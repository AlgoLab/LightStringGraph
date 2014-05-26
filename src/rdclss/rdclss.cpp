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
// Reads classifier

#include <string>
#include <vector>

#include <sstream>
#include <iostream>

#include "BWTReader.h"
#include "GSAReader.h"
#include "util.h"
#include "search.h"
#include "types.h"
#include "q_interval.h"

using std::vector;
using std::string;

void spliced_classifier( BWTReader& bwtr, GSAReader& gsar,
			 QIntervalManager& imgr, vector< NucleoCounter >& C, 
			 char splicedflag[ ], long readsnum )
{
  QInterval* jqin;
  unsigned int nwintc=0;

  while( (jqin = imgr.get_next_interval( )) )
    {
      BWTPosition begin = jqin->get_begin( );
      BWTPosition end   = jqin->get_end( );
      bwtr.move_to( begin );
      vector< NucleoCounter > occ_begin( bwtr.get_Pi( ) );
      bwtr.move_to( end );
      vector< NucleoCounter > occ_end( bwtr.get_Pi( ) );

      // check if there are different bases in [ begin, end )
      int numofbases =0;
      int extending_base =0;
      for( int base( BASE_A ); base <= BASE_T; ++base )
	{
	  if( occ_end[ base ] != occ_begin[ base ] )
	    {
	      ++numofbases;
	      extending_base = base;
	    }
	}
      if( numofbases > 1 )
	{
	  vector< GSAEntry* >* gsaint = gsar.get( begin, end );
	  for( vector< GSAEntry* >::iterator it = gsaint->begin( ); it != gsaint->end( ); ++it )
	    {
	      if( (*it)->numSeq >= readsnum )
		(*it)->numSeq -= readsnum;
	      long arrayposition = (*it)->numSeq / 8;
	      short offset = (*it)->numSeq % 8;
	      splicedflag[ arrayposition ] |= (0b10000000 >> offset);
	      delete *it;
	    }
	  delete gsaint;
	}
      else if( numofbases == 1 )
	{
	  BWTPosition new_begin = C[ extending_base ] + occ_begin[ extending_base ];
	  BWTPosition new_end = C[ extending_base ] + occ_end[ extending_base ];
	  QInterval new_int( new_begin, new_end );
	  imgr.add_interval( new_int, (Nucleotide) extending_base );
	  ++nwintc;
	}
    }
  std::cerr << "--> Created " << nwintc << " new intervals." << std::endl;
}

void show_usage( )
{
  std::cerr << "Usage: rclss -B <BWTFilenamePrefix> ";
  std::cerr << "-G <gsaFilename> ";
  std::cerr << "-N <numberOfReads> ";
  std::cerr << "[-T <tau>] ";
  std::cerr << "[-C <cycnum>] ";
  std::cerr << "-o <outputfile>" << std::endl;
  std::cerr << std::endl << "Options:" << std::endl;
  std::cerr << "\t-B --BWT <BWTPrefix> \t : BWT file prefix" << std::endl;
  std::cerr << "\t-G --GSA <gsaFilename> \t : GSA file prefix" << std::endl;
  std::cerr << "\t-N --number-of-reads \t : Number of reads in BWT" << std::endl;
  std::cerr << "\t-T --tau \t : Tau value" << std::endl;
  std::cerr << "\t-C --cyc-num \t : How many cycles after tau?" << std::endl;
  std::cerr << "\t-o --output-file \t : Output file" << std::endl;
  std::cerr << std::endl;
}

/**********************/
/* Program Main       */
/**********************/

int main( int argc, char** argv )
{
  if( argc < 8 )
    {
      show_usage( );
      return 1;
    }
  
  string bwt_pre = "";
  string gsa_file = "";
  string output_file = "";
  long nreads =0;
  int CLASSIFIER_TAU = 3;
  int CLASSIFIER_CYCNUM = 6;

  for( int i( 1 ); i < argc; ++i )
    {
      if( i+1 != argc )
	{
	  if( string( argv[ i ] ) == "--BWT" || string( argv[ i ] ) == "-B" )
	    {
	      bwt_pre = string( argv[ ++i ] );
	    } else if ( string( argv[ i ] ) == "--GSA" || string( argv[ i ] ) == "-G" )
	    {
	      gsa_file = string( argv[ ++i ] );
	    } else if ( string( argv[ i ] ) == "--output-file" || string( argv[ i ] ) == "-o" )
	    {
	      output_file = string( argv[ ++i ] );
	    } else if ( string( argv[ i ] ) == "--number-of-reads" || string( argv[ i ] ) == "-N" )
	    {
	      nreads = atol( argv[ ++i ] );
	    } else if ( string( argv[ i ] ) == "--tau" || string( argv[ i ] ) == "-T" )
	    {
	      CLASSIFIER_TAU = atoi( argv[ ++i ] );
	    } else if ( string( argv[ i ] ) == "--cyc-num" || string( argv[ i ] ) == "-C" )
	    {
	      CLASSIFIER_CYCNUM = atoi( argv[ ++i ] );
	    } else
	    {
	      std::cerr << "Invalid arguments, please try again." << std::endl;
	      return 1;
	    }
	}
    }

  if( bwt_pre == "" || gsa_file == "" || output_file == "" )
    {
      std::cerr << "Missing argument(s)." << std::endl;
      show_usage( );
      return 1;
    }

  vector< string > BWTInputFilenames;
  vector< string > qIntFilenames;
  vector< string > revqIntFilenames;

  string gsaInputFileName( gsa_file );
  GSAReader gsardr( gsaInputFileName );

  // create filenames and add them to the previous vectors
  for( int i( 0 ); i < ALPHABET_SIZE; ++i )
    {
      std::ostringstream partialBWTname, qIntFilename, revqIntFilename;
      partialBWTname << bwt_pre << i;
      qIntFilename << ".QINT-" << i;
      revqIntFilename << ".revQINT-" << i;

      BWTInputFilenames.push_back( partialBWTname.str( ) );
      qIntFilenames.push_back( qIntFilename.str( ) );
      revqIntFilenames.push_back( revqIntFilename.str( ) );
    }

  QIntervalManager imgr( qIntFilenames );
  
  BWTReader br( BWTInputFilenames );
  
  std::cerr << "Building vector C..";
  std::cerr.flush( );

  vector< NucleoCounter >* c = br.get_C( );

  std::cerr << "done." << std::endl;

  for (int nucl( BASE_A ); (unsigned int) nucl < ALPHABET_SIZE; ++nucl)
    {
      QInterval jint ( c->at( nucl ), c->at( nucl+1 ) );
      imgr.add_interval( jint, (Nucleotide) nucl );
    }

  std::cerr.flush( );
  imgr.swap_files();

  build_tau_intervals( br, imgr, *c, CLASSIFIER_TAU );

  int numchar = (int)( nreads / 8 ) + ( nreads % 8 != 0 ? 1 : 0 );

  char* splicedflag = new char[ numchar ];
  for( int i( 0 ); i < numchar; ++i )
    splicedflag[ i ] = 0x0;
  
  for( int i( 0 ); i < CLASSIFIER_CYCNUM; ++i ) // TODO: while exists interval in imgr
    {
      std::cerr << "Extension step #" << (i+1) << std::endl;
      br.reset( );
      spliced_classifier( br, gsardr, imgr, *c, splicedflag, nreads );
      imgr.swap_files( );
    }

  // TODO: Output spliced reads in some way

  for( int i( 0 ); i < numchar; ++i )
    {
      for( int j( 0 ); j < 8 && i*8+j < nreads; ++j )
	{
	  if( splicedflag[ i ] & (0b10000000 >> j ) )
	    std::cerr << 1;
	  else
	    std::cerr << 0;
	}
    }

  std::cerr << std::endl;

  delete c;
  delete splicedflag;
}
