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

#include <zlib.h>
#include "kseq.h"
KSEQ_INIT(gzFile, gzread)

#include "types.h"
#include "util.h"
#include "PrefixManager.h"
#include "edgeLabelInterval.h"


#include "asqg_fmt.h"

#include "MultiFileManager.h"

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

// no. of reads per bucket if not specified on the command line
#define DEFAULT_BUCKET_LENGTH 1000000

typedef InputMultiFileManager bucket_infilemanager_t;
typedef OutputMultiFileManager bucket_outfilemanager_t;

using std::string;
using std::vector;

// Returns true if first is contained in second
bool operator<=(const QInterval& first, const QInterval& second) {
  // Works only for non-overlapping intervals.
  return (second.get_begin() <= first.get_begin()
          && second.get_end() >= first.get_end());
}

struct node_t
{
  typedef vector< SequenceNumber >  succs_t;
  typedef vector< QInterval >       labels_t;
  typedef vector< SequenceLength >  lens_t;

  succs_t   succs;
  labels_t  labels;
  lens_t    lens;
};

struct graph_t
{
  typedef vector<node_t> nodes_t;

  const SequenceNumber base_vertex;
  nodes_t nodes;

  graph_t(const SequenceNumber n_vertices, const SequenceNumber base_vertex_=0)
      :base_vertex(base_vertex_), nodes(n_vertices)
  {
  }

  bool
  add_edge(const SequenceNumber& source,
           const SequenceNumber& dest,
           const QInterval& label,
           const SequenceLength len) {
    const vector< QInterval >& labels= nodes[source-base_vertex].labels;
    for(vector< QInterval >::const_iterator qint_it = labels.begin();
        qint_it != labels.end(); ++qint_it) {
      if (label <= *qint_it)
        return false;
    }
    nodes[source-base_vertex].succs.push_back(dest);
    nodes[source-base_vertex].labels.push_back(label);
    nodes[source-base_vertex].lens.push_back(len);
    return true;
  }

};

std::ostream& operator<<(std::ostream& out, const graph_t& graph) {
  SequenceNumber i= graph.base_vertex;
  for(graph_t::nodes_t::const_iterator it= graph.nodes.begin();
      it != graph.nodes.end(); ++it, ++i) {
    for(SequenceNumber j =0; j < it->succs.size(); ++j) {
      out.write(reinterpret_cast<const char*>(&(it->succs[j])), sizeof(SequenceNumber));
      out.write(reinterpret_cast<const char*>(&i), sizeof(SequenceNumber));
      out.write(reinterpret_cast<const char*>(&(it->lens[j])), sizeof(SequenceLength));
    }
  }
  return out;
}




void
show_usage(){
  std::cerr << "Usage: sgbuild ";
  std::cerr << " -b <basename> ";
  std::cerr << " -m <max_arc_length> ";
  std::cerr << " -r <read_filename> ";
  std::cerr << " -l <read_length> ";
  std::cerr << " [-g <bucket_length>] ";
  std::cerr << std::endl;
  std::cerr << std::endl << "Parameters:" << std::endl;
  std::cerr << "\t-b <basename>        # (required)" << std::endl;
  std::cerr << "\t-m <max_arc_length>  # (required)" << std::endl;
  std::cerr << "\t-r <read_filename>   # (required) " << std::endl;
  std::cerr << "\t-l <read_length>     # (required) " << std::endl;
  std::cerr << "\t[-g <bucket length>] # (optional) % default = " << DEFAULT_BUCKET_LENGTH << std::endl;
  std::cerr << std::endl;
}

struct options_t {
  bool initialized;
  std::string basename;
  std::string read_filename;
  SequenceLength max_arc_length;
  SequenceLength read_length;
  SequenceLength max_bucket_length;

  options_t()
      :initialized(false),
       basename(""),
       read_filename(""),
       max_arc_length(0),
       read_length(0),
       max_bucket_length(DEFAULT_BUCKET_LENGTH)
  {};
};


options_t
parse_cmds(const int argc, const char** argv)
{
  options_t opts;
  opts.initialized= false;
  if(argc < 7) {
    show_usage();
    return opts;
  }
  for(int i=1; i<argc; ++i) {
    const std::string carg(argv[i]);
    if(carg == "-b")
      opts.basename= std::string(argv[++i]);
    else if(carg == "-r")
      opts.read_filename= std::string(argv[++i]);
    else if(carg == "-m")
      opts.max_arc_length= atoi(argv[++i]);
    else if(carg == "-l")
      opts.read_length= atoi(argv[++i]);
    else if(carg == "-g") {
      opts.max_bucket_length= atoi(argv[++i]);
      _FAIL_IF(opts.max_bucket_length==0);
    }
    else {
      ERROR("Can't parse argument: " << argv[i]);
      show_usage();
      return opts;
    }
  }

  opts.initialized= true;
  return opts;
}

void multiplex_intervals(vector<std::ifstream*> in_arc_files,
                         vector<std::ifstream*> in_label_files,
                         bucket_outfilemanager_t& out_bucketfiles,
                         const SequenceLength max_arc_length,
                         const SequenceLength max_bucket_length)
{
  SequenceNumber arc[4];
  BWTPosition label[2];
  SequenceNumber &sourcebegin= arc[2];

  for(SequenceLength l(0); l < max_arc_length; ++l) {
    std::ifstream& in_arcfile= *in_arc_files[l];
    std::ifstream& in_labfile= *in_label_files[l];
    while (in_arcfile.read(reinterpret_cast<char*>(&arc), sizeof(arc)) &&
           in_labfile.read(reinterpret_cast<char*>(&label), sizeof(label))) {
      const size_t bucket_no= sourcebegin/max_bucket_length;
      out_bucketfiles[bucket_no].write(reinterpret_cast<const char*>(&arc), sizeof(arc));
      out_bucketfiles[bucket_no].write(reinterpret_cast<const char*>(&label), sizeof(label));
      out_bucketfiles[bucket_no].write(reinterpret_cast<const char*>(&l), sizeof(l));
    }
  }
}

void reduce_bucket(std::ifstream& in_bucketfile,
                   std::ofstream& out_graph,
                   long& no_of_edges,
                   const SequenceNumber base_vertex,
                   const SequenceNumber bucket_length) {
  LDEBUG("Reading and reducing bucket...");

  graph_t graph(bucket_length, base_vertex);
  SequenceNumber arc[4];
  BWTPosition label[2];
  SequenceLength l;
  SequenceNumber &destbegin= arc[0], &destend= arc[1];
  SequenceNumber &sourcebegin= arc[2], &sourceend= arc[3];
  while (in_bucketfile.read(reinterpret_cast<char*>(&arc), sizeof(arc)) &&
         in_bucketfile.read(reinterpret_cast<char*>(&label), sizeof(label)) &&
         in_bucketfile.read(reinterpret_cast<char*>(&l), sizeof(l))) {
    for(SequenceNumber src= sourcebegin; src < sourceend; ++src) {
      for(SequenceNumber dst= destbegin; dst < destend; ++dst) {
        if (graph.add_edge(src, dst, QInterval(label[0], label[1]), l))
          ++no_of_edges;
      }
    }
  }
  LDEBUG("Bucket read and reduced!");
  LDEBUG("Writing resulting partial string graph...");
  out_graph << graph;
  LDEBUG("Partial string graph written!");
}


std::string n2str(const int n) {
  std::stringstream nstream;
  nstream << n;
  return nstream.str();
}


//Main
int
main(const int argc, const char** argv)
{
  const options_t opts= parse_cmds(argc, argv);

  if(!opts.initialized)
    return false;

  INFO("redbuild - Starting...");
  INFO("Basename      : " << opts.basename);
  INFO("Reads file    : " << opts.read_filename);
  INFO("Reads length  : " << opts.read_length);
  INFO("Max arc len   : " << opts.max_arc_length);
  INFO("Max bucket len: " << opts.max_bucket_length);

  LDEBUG("Getting the number of reads...");
// Get the number of reads
  std::ifstream pmgr_tmp((opts.basename + ".outlsg.lexorder").c_str(),
                         std::ios_base::binary);
  pmgr_tmp.seekg(0, std::ios_base::end);
  const SequenceNumber no_of_reads = pmgr_tmp.tellg()/sizeof(SequenceNumber);
  pmgr_tmp.close();

  INFO("Building the string graph of " << no_of_reads << " reads.");

  const size_t no_of_buckets= 1 + ((no_of_reads - 1) / opts.max_bucket_length);
  const SequenceNumber real_bucket_length= 1 + ((no_of_reads - 1) / no_of_buckets);

  INFO("Splitting the set of reads in " << no_of_buckets <<
       " subsets of, at most, " << real_bucket_length << " reads.");


  INFO("Starting multiplexing the interval representation of the overlap graph...");
  {
    vector< std::ifstream* > arcsFiles;
    vector< std::ifstream* > labelFiles;
    bucket_outfilemanager_t out_bucketfiles(opts.basename, ".tmplsg.bucket_");
    for(SequenceLength j(0); j < opts.max_arc_length; ++j) {
      const std::string jstr(n2str(j));
      arcsFiles.push_back(new std::ifstream((opts.basename + ".outlsg.arcs_"  + jstr).c_str(),
                                            std::ios_base::in | std::ios_base::binary));
      labelFiles.push_back(new std::ifstream((opts.basename + ".outlsg.labels_"  + jstr).c_str(),
                                             std::ios_base::in | std::ios_base::binary));
    }
    multiplex_intervals(arcsFiles, labelFiles, out_bucketfiles,
                        opts.max_arc_length, real_bucket_length);
    for(vector< std::ifstream* >::iterator it= arcsFiles.begin();
        it != arcsFiles.end(); ++it)
      delete *it;
    for(vector< std::ifstream* >::iterator it= labelFiles.begin();
        it != labelFiles.end(); ++it)
      delete *it;
  }
  INFO("Multiplexing terminated!");

  INFO("Starting reduction of each bucket...");
  long no_of_edges= 0;
  {
    std::ofstream out_graph( opts.basename + ".outlsg.reduced.graph", std::ios_base::binary);
    SequenceNumber base_vertex= 0;
    for (size_t bucket_number= 0;
         bucket_number < no_of_buckets;
         ++bucket_number) {
      INFO("Reducing bucket #" << std::setw(4) << std::left << std::setfill(' ') << bucket_number <<
           " (" << std::setw(12) << std::right << std::setfill(' ') << no_of_edges << " edges so far)");
      std::ifstream in_bucketfile(opts.basename + ".tmplsg.bucket_" + n2str(bucket_number),
                                  std::ios::binary);
      const SequenceNumber bucket_length= std::min(no_of_reads, base_vertex+real_bucket_length)-base_vertex;
      reduce_bucket(in_bucketfile, out_graph, no_of_edges, base_vertex, bucket_length);
      base_vertex += bucket_length;
// Remove bucket file
      remove((opts.basename + ".tmplsg.bucket_" + n2str(bucket_number)).c_str());
    }
  }
  INFO("Reduction terminated!");
  INFO("Number of edges: " << no_of_edges);


  INFO("Writing the string graph in ASQG format...");
  {
    vector< string > reads_ids;
    reads_ids.reserve(no_of_reads);
    {
      INFO("Reading sequence IDs from the FASTA file (and writing vertices)...");
      gzFile fp= gzopen(opts.read_filename.c_str(), "r");
      kseq_t *seq= kseq_init(fp);
      while (kseq_read(seq) >= 0) {
        reads_ids.push_back(seq->name.s);
        print_vertex(std::cout, seq->name.s, seq->seq.s);
      }
      kseq_destroy(seq);
      gzclose(fp);
      INFO("Sequence IDs read (and vertices written)!");
    }
    {
      INFO("Writing edges...");
      PrefixManager pmgr(opts.basename + ".outlsg.lexorder");
      SequenceNumber arc[2];
      SequenceNumber &source= arc[0], &dest= arc[1];
      SequenceLength arclen =0;
      SequenceNumber rsource= 0, rdest= 0;
      std::ifstream graphin( opts.basename + ".outlsg.reduced.graph", std::ios_base::binary);
      while(graphin.read(reinterpret_cast<char*>(&arc), sizeof(arc)) &&
            graphin.read(reinterpret_cast<char*>(&arclen), sizeof(SequenceLength))) {
        rsource= pmgr.get_elem(source);
        rdest= pmgr.get_elem(dest);
        print_edge(std::cout, reads_ids, rsource, rdest, arclen, opts.read_length);
      }
      graphin.close();
      std::cout.flush();
      INFO("Edges written!");
    }
  }
  INFO("redbuild - Terminated!");
  return EXIT_SUCCESS;
}
