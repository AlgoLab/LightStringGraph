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

#include "types.h"
#include "util.h"
#include "file_abstractions.h"
#include "EndPosManager.h"
#include "edgeLabelInterval.h"


#include "MultiFileManager.h"

// gzip-based multifile managers

template <bool WRITE_MODE>
struct GZipBuilder {

  typedef gzip_file_t file_t;

  file_t* operator()(const std::string& name)
  {
    file_t* res= new gzip_file_t(name, WRITE_MODE);
    _FAIL_IF(!res->is_open());
    return res;
  }

};

typedef MultiFileManager<gzip_file_t, GZipBuilder<true> > bucket_outfilemanager_t;
typedef MultiFileManager<gzip_file_t, GZipBuilder<false> > bucket_infilemanager_t;



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
#define DEFAULT_HAS_REVERSE_READS true

static inline char
bool_2_mask(const bool first, const bool second)
{
  const char x = (((int)first) | (((int)second) << 1));
  return x;
}


using std::string;
using std::vector;

// Returns true if first is contained in second
static
bool operator<=(const QInterval& first, const QInterval& second) {
  // Works only for non-overlapping intervals.
  return (second.begin <= first.begin
          && second.end >= first.end);
}

struct node_t
{
  typedef vector< QInterval >       labels_t;

  labels_t  labels;
};

struct graph_t
{
  typedef vector<node_t> nodes_t;

  gzip_file_t& graph_out_file;
  const SequenceNumber base_vertex;
  nodes_t nodes;
  EndPosManager& eomgr;
  const SequenceNumber no_of_reads;
  const bool has_reverse_reads;

  graph_t(gzip_file_t& graph_out_file_,
          EndPosManager& eomgr_, const SequenceNumber n_vertices, const SequenceNumber nreads,
	  const SequenceNumber base_vertex_=0, const bool has_rev_reads_=DEFAULT_HAS_REVERSE_READS)
    :graph_out_file(graph_out_file_), base_vertex(base_vertex_), nodes(n_vertices), eomgr(eomgr_),
     no_of_reads(nreads), has_reverse_reads(has_rev_reads_)
  {
  }

  bool
  add_edge(const SequenceNumber source,
           const SequenceNumber dest,
           const QInterval& label,
           const SequenceLength len) {
    const vector< QInterval >& labels= nodes[source-base_vertex].labels;
    for(vector< QInterval >::const_iterator qint_it = labels.begin();
        qint_it != labels.end(); ++qint_it) {
      if ((label <= *qint_it) || (same_node(source, dest)))
        {
          return false;
        }
    }
    // INFO("Added an an edge between " << source << " and " << dest << " len " << (int) len);
    //nodes[source-base_vertex].succs.push_back(dest);
    // INFO("succs size is " << nodes[source-base_vertex].succs.size() << " for " << source);
    nodes[source-base_vertex].labels.push_back(label);
    //nodes[source-base_vertex].lens.push_back(len);
    save_edge(source, dest, len);
    return true;
  }

private:
  bool
  same_node(const SequenceNumber source,
	    const SequenceNumber dest) const
  {
    return (//(source == dest) |
            ((source >=  no_of_reads) & (source == dest - no_of_reads)) |
            ((dest >= no_of_reads) & (dest == source - no_of_reads)));
  }

  void
  save_edge(const SequenceNumber i,
            const SequenceNumber succ,
            const SequenceLength len) {
    const SequenceNumber gathered_i= eomgr.get_elem(i);
    const bool reversei = (gathered_i >= no_of_reads/ (has_reverse_reads ? 2 : 1));
    const SequenceNumber reali = reversei ? gathered_i - no_of_reads/ (has_reverse_reads ? 2 : 1) : gathered_i;
    const SequenceNumber gathered_succ= eomgr.get_elem(succ);
    const bool reversej = (gathered_succ >= no_of_reads/ (has_reverse_reads ? 2 : 1));
    const SequenceNumber realsucc = reversej ? gathered_succ - no_of_reads/(has_reverse_reads ? 2 : 1) : gathered_succ;
// Edges of type 1->1 are equal to an edge 0->0 so we can discard them
// Edges of type 0->1 s.t. the first index is greater of the
// second are equal to an edge 1->0 s.t. the first index is
// smaller of the second
    if(realsucc < reali) {
      const char reversed = bool_2_mask(reversej, reversei);
      graph_out_file.write(realsucc);
      graph_out_file.write(reali);
      graph_out_file.write(len);
      graph_out_file.write(reversed);
    }
  }

};




void
show_usage(){
  std::cerr << "Usage: redbuild ";
  std::cerr << " -b <basename> ";
  std::cerr << " -m <max_arc_length> ";
  std::cerr << " [-g <bucket_length>] ";
  std::cerr << " [-r] ";
  std::cerr << std::endl;
  std::cerr << std::endl << "Parameters:" << std::endl;
  std::cerr << "\t-b <basename>        # (required)" << std::endl;
  std::cerr << "\t-m <max_arc_length>  # (required)" << std::endl;
  std::cerr << "\t[-g <bucket length>] # (optional) % default = " << DEFAULT_BUCKET_LENGTH << std::endl;
  std::cerr << "\t[-r]                 # (optional) % default = " << DEFAULT_HAS_REVERSE_READS << std::endl;
  std::cerr << std::endl;
}

struct options_t {
  bool initialized;
  std::string basename;
  SequenceLength max_arc_length;
  SequenceNumber max_bucket_length;
  bool has_reverse_reads;

  options_t()
      :initialized(false),
       basename(""),
       max_arc_length(0),
       max_bucket_length(DEFAULT_BUCKET_LENGTH),
       has_reverse_reads(DEFAULT_HAS_REVERSE_READS)
  {};
};


options_t
parse_cmds(const int argc, const char** argv)
{
  options_t opts;
  opts.initialized= false;
  if(argc < 5) {
    show_usage();
    return opts;
  }
  for(int i=1; i<argc; ++i) {
    const std::string carg(argv[i]);
    if(carg == "-b")
      opts.basename= std::string(argv[++i]);
    else if(carg == "-m")
      opts.max_arc_length= atoi(argv[++i]);
    else if(carg == "-r")
      opts.has_reverse_reads= !DEFAULT_HAS_REVERSE_READS;
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

bool write_bucket_elem(gzip_file_t& out_bucketfile,
                       SequenceNumber arc[4],
                       BWTPosition label[2],
                       SequenceLength l) {
  out_bucketfile.write(arc[0]);
  out_bucketfile.write_compressed<SequenceNumber, 1>(arc[1]-arc[0]);
  out_bucketfile.write(arc[2]);
  out_bucketfile.write_compressed<SequenceNumber, 1>(arc[3]-arc[2]);
  out_bucketfile.write(label[0]);
  out_bucketfile.write_compressed<BWTPosition, 2>(label[1]-label[0]);
  out_bucketfile.write(l);
  return true;
}

bool read_bucket_elem(gzip_file_t& in_bucketfile,
                      SequenceNumber arc[4],
                      BWTPosition label[2],
                      SequenceLength& l) {
  SequenceNumber diff_arc1(0), diff_arc2(0);
  BWTPosition diff_label(0);

  bool res= in_bucketfile.read(arc[0]);
  if (!res) return false;
  in_bucketfile.read_compressed<SequenceNumber, 1>(diff_arc1);
  arc[1]= arc[0]+diff_arc1;

  res= in_bucketfile.read(arc[2]);
  if (!res) return false;
  in_bucketfile.read_compressed<SequenceNumber, 1>(diff_arc2);
  arc[3]= arc[2]+diff_arc2;

  res= in_bucketfile.read(label[0]);
  if (!res) return false;
  in_bucketfile.read_compressed<BWTPosition, 2>(diff_label);
  label[1]= label[0]+diff_label;

  return in_bucketfile.read(l);
}


void multiplex_intervals(vector<gzip_file_t*>& in_arc_files,
                         vector<gzip_file_t*>& in_label_files,
                         bucket_outfilemanager_t& out_bucketfiles,
                         const SequenceLength max_arc_length,
                         const SequenceNumber max_bucket_length)
{
  SequenceNumber arc[4];
  BWTPosition label[2];
  SequenceNumber &sourcebegin= arc[2], &sourceend= arc[3];
  SequenceNumber orig_sourceend;

  for(SequenceLength l(0); l < max_arc_length; ++l) {
    gzip_file_t& in_arcfile= *in_arc_files[l];
    gzip_file_t& in_labfile= *in_label_files[l];
    while (in_arcfile.read(arc) && in_labfile.read(label)) {
      size_t bucket_no= sourcebegin/max_bucket_length;
      orig_sourceend= sourceend;
      sourceend= (bucket_no+1)*max_bucket_length;
      while (sourceend<orig_sourceend) {
        write_bucket_elem(out_bucketfiles[bucket_no], arc, label, l);
        sourcebegin= sourceend;
        sourceend += max_bucket_length;
        ++bucket_no;
      }
      sourceend= orig_sourceend;
      write_bucket_elem(out_bucketfiles[bucket_no], arc, label, l);
    }
  }
}

void reduce_bucket(EndPosManager& eomgr,
                   gzip_file_t& in_bucketfile,
                   gzip_file_t& out_graph,
                   long& no_of_edges,
                   const SequenceNumber base_vertex,
                   const SequenceNumber bucket_length,
                   const SequenceNumber no_of_reads) {
  LDEBUG("Reading and reducing bucket...");

  graph_t graph(out_graph, eomgr, bucket_length, no_of_reads, base_vertex);
  SequenceNumber arc[4];
  BWTPosition label[2];
  SequenceLength l;
  SequenceNumber &destbegin= arc[0], &destend= arc[1];
  SequenceNumber &sourcebegin= arc[2], &sourceend= arc[3];
  while (read_bucket_elem(in_bucketfile, arc, label, l)) {
    _FAIL_IF((sourceend - base_vertex > bucket_length));
    for(SequenceNumber src= sourcebegin; src < sourceend; ++src) {
      for(SequenceNumber dst= destbegin; dst < destend; ++dst) {
        if (graph.add_edge(src, dst, QInterval(label[0], label[1]), l))
          ++no_of_edges;
      }
    }
  }
  LDEBUG("Bucket read and reduced!");
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
  INFO("Max arc len   : " << PRINT_SL(opts.max_arc_length));
  INFO("Max bucket len: " << opts.max_bucket_length);

  LDEBUG("Getting the number of reads...");
// Get the number of reads
  std::ifstream eomgr_tmp((opts.basename + "-end-pos").c_str(),
                          std::ios_base::binary);
  SequenceNumber tmp_ = 0;
  eomgr_tmp.read( reinterpret_cast<char*>(&tmp_), sizeof( SequenceNumber ) );
  const SequenceNumber no_of_reads = tmp_;
  eomgr_tmp.close();

  INFO("Building the string graph of " << no_of_reads << " reads.");

  const size_t no_of_buckets= 1 + ((no_of_reads - 1) / opts.max_bucket_length);
  const SequenceNumber real_bucket_length= 1 + ((no_of_reads - 1) / no_of_buckets);

  INFO("Splitting the set of reads in " << no_of_buckets <<
       " subsets of, at most, " << real_bucket_length << " reads.");


  INFO("Starting multiplexing the interval representation of the overlap graph...");
  {
    vector< gzip_file_t* > arcsFiles;
    vector< gzip_file_t* > labelFiles;
    bucket_outfilemanager_t out_bucketfiles(opts.basename, ".tmplsg.bucket_");
    for(SequenceLength j(0); j < opts.max_arc_length; ++j) {
      const std::string jstr(n2str(PRINT_SL(j)));
      arcsFiles.push_back(new gzip_file_t(opts.basename + ".outlsg.arcs_"  + jstr, false));
      labelFiles.push_back(new gzip_file_t(opts.basename + ".outlsg.labels_"  + jstr, false));
    }
    multiplex_intervals(arcsFiles, labelFiles, out_bucketfiles,
                        opts.max_arc_length, real_bucket_length);
    for(vector< gzip_file_t* >::iterator it= arcsFiles.begin();
        it != arcsFiles.end(); ++it)
      delete *it;
    for(vector< gzip_file_t* >::iterator it= labelFiles.begin();
        it != labelFiles.end(); ++it)
      delete *it;
  }
  INFO("Multiplexing terminated!");

  INFO("Starting reduction of each bucket...");
  long no_of_edges= 0;
  {
    EndPosManager eomgr(opts.basename + "-end-pos");
    gzip_file_t out_graph( opts.basename + ".outlsg.reduced.graph", true);
    SequenceNumber base_vertex= 0;
    for (size_t bucket_number= 0;
         bucket_number < no_of_buckets;
         ++bucket_number) {
      INFO("Reducing bucket #" << std::setw(4) << std::left << std::setfill(' ') << bucket_number <<
           " (" << std::setw(12) << std::right << std::setfill(' ') << no_of_edges << " edges so far)");
      const SequenceNumber bucket_length= std::min(no_of_reads, base_vertex+real_bucket_length)-base_vertex;
      {
        gzip_file_t in_bucketfile(opts.basename + ".tmplsg.bucket_" + n2str(bucket_number));
        reduce_bucket(eomgr, in_bucketfile, out_graph, no_of_edges, base_vertex, bucket_length, no_of_reads);
      }
      base_vertex += bucket_length;
// Remove bucket file
      remove((opts.basename + ".tmplsg.bucket_" + n2str(bucket_number)).c_str());
    }
  }
  INFO("Reduction terminated!");
  INFO("Number of edges: " << no_of_edges);

  INFO("redbuild - Terminated!");
  return EXIT_SUCCESS;
}
