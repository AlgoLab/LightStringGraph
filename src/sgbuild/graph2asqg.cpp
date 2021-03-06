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
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cassert>
#include <boost/lexical_cast.hpp>

#include <zlib.h>
#include "kseq.h"
KSEQ_INIT(gzFile, gzread)

#include "types.h"
#include "util.h"


#include "asqg_fmt.h"

#ifndef LOG_MSG
#define LOG_MSG
#endif
#ifndef LOG_THRESHOLD
#define LOG_THRESHOLD LOG_LEVEL_INFO
#endif
#if LOG_THRESHOLD < LOG_LEVEL_INFO
#undef LOG_THRESHOLD
#define LOG_THRESHOLD LOG_LEVEL_INFO
#endif

#include "log.h"

using std::string;
using std::vector;

void
show_usage(){
  std::cerr << "Usage: graph2asqg ";
  std::cerr << " -b <basename> ";
  std::cerr << " [-r <read_filename>] ";
  std::cerr << " -l <read_length> ";
  std::cerr << " [-n]";
  std::cerr << std::endl;
  std::cerr << std::endl << "Parameters:" << std::endl;
  std::cerr << "\t-b <basename>        # (required)" << std::endl;
  std::cerr << "\t-r <read_filename>   # (optional, default: <basename>) " << std::endl;
  std::cerr << "\t-l <read_length>     # (required) " << std::endl;
  std::cerr << "\t-n                   # (optional) use numeric IDs instead of FASTA IDs" << std::endl;
  std::cerr << std::endl;
}

struct options_t {
  bool initialized;
  std::string basename;
  std::string read_filename;
  SequenceLength max_arc_length;
  SequenceLength read_length;
  bool use_numeric_ids;

  options_t()
      :initialized(false),
       basename(""),
       read_filename(""),
       max_arc_length(0),
       read_length(0),
       use_numeric_ids(false)
  {};
};


options_t
parse_cmds(const int argc, const char** argv)
{
  options_t opts;
  opts.initialized= false;

  for(int i=1; i<argc; ++i) {
    const std::string carg(argv[i]);
    if(carg == "-b")
      opts.basename= std::string(argv[++i]);
    else if(carg == "-r")
      opts.read_filename= std::string(argv[++i]);
    else if(carg == "-l")
      opts.read_length= atoi(argv[++i]);
    else if(carg == "-n")
      opts.use_numeric_ids= true;
    else {
      ERROR("Can't parse argument: " << argv[i]);
      show_usage();
      return opts;
    }
  }

  if (opts.read_filename == "") {
    opts.read_filename= opts.basename;
  }

  if (opts.read_filename == "" || opts.read_length == 0) {
    show_usage();
    return opts;
  }

  opts.initialized= true;
  return opts;
}

//Main
int
main(const int argc, const char** argv)
{
  const options_t opts= parse_cmds(argc, argv);

  if(!opts.initialized)
    return false;

  INFO("graph2asqg - Starting...");
  INFO("Basename      : " << opts.basename);
  INFO("Reads file    : " << opts.read_filename);
  INFO("Reads length  : " << PRINT_SL(opts.read_length));
  INFO("Read IDs      : " << (opts.use_numeric_ids? "numeric" : "fasta"));

  LDEBUG("Getting the number of reads...");
// Get the number of reads
  std::ifstream eomgr_tmp((opts.basename + "-end-pos").c_str(),
                          std::ios_base::binary);
  SequenceNumber tmp_ = 0;
  eomgr_tmp.read( reinterpret_cast<char*>(&tmp_), sizeof( SequenceNumber ) );
  const SequenceNumber no_of_reads = tmp_/2;
  eomgr_tmp.close();

  INFO("Building the string graph of " << no_of_reads << " reads.");

  INFO("Writing the string graph in ASQG format...");
  {
    const std::string read_pref= "r";
    vector< string > reads_ids;
    if (!opts.use_numeric_ids) {
      reads_ids.reserve(no_of_reads);
    }
    {
      INFO("Reading sequence IDs from the FASTA file (and writing vertices)...");
      gzFile fp= gzopen(opts.read_filename.c_str(), "r");
      kseq_t *seq= kseq_init(fp);
      SequenceNumber seq_count= 0;
      if (opts.use_numeric_ids) {
	while((kseq_read(seq) >= 0) && (seq_count < no_of_reads)) {
          print_vertex(std::cout, read_pref.c_str(), seq_count, seq->seq.s);
          ++seq_count;
        }
      } else {
        while ((kseq_read(seq) >= 0) && (seq_count < no_of_reads)) {
          reads_ids.push_back(seq->name.s);
          print_vertex(std::cout, seq->name.s, seq->seq.s);
	  ++seq_count;
        }
      }
      kseq_destroy(seq);
      gzclose(fp);
      INFO("Sequence IDs read (and vertices written)!");
    }
    {
      INFO("Writing edges...");
      SequenceNumber arc[2];
      SequenceNumber &source= arc[0], &dest= arc[1];
      SequenceLength arclen= 0;
      char reversed= 0;
      std::ifstream graphin( opts.basename + ".outlsg.reduced.graph", std::ios_base::binary);
      if (opts.use_numeric_ids) {
        while(graphin.read(reinterpret_cast<char*>(&arc), sizeof(arc)) &&
              graphin.read(reinterpret_cast<char*>(&arclen), sizeof(SequenceLength)) &&
              graphin.read(reinterpret_cast<char*>(&reversed), sizeof(char))) {
	  if(source >= no_of_reads)
	    source /=2;
	  if(dest >= no_of_reads)
	    dest /=2;
          print_edge(std::cout,
                     read_pref+boost::lexical_cast<std::string>(source),
                     read_pref+boost::lexical_cast<std::string>(dest),
                     arclen, opts.read_length, reversed);
        }
      } else {
        while(graphin.read(reinterpret_cast<char*>(&arc), sizeof(arc)) &&
              graphin.read(reinterpret_cast<char*>(&arclen), sizeof(SequenceLength)) &&
              graphin.read(reinterpret_cast<char*>(&reversed), sizeof(char))) {
	  if(source >= no_of_reads)
	    source /=2;
	  if(dest >= no_of_reads)
	    dest /=2;
          print_edge(std::cout, reads_ids[source], reads_ids[dest], arclen, opts.read_length, reversed);
        }
      }
      graphin.close();
      std::cout.flush();
      INFO("Edges written!");
    }
  }
  INFO("graph2asqg - Terminated!");
  return EXIT_SUCCESS;
}
