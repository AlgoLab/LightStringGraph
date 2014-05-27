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

using std::string;
using std::vector;

struct Node
{
  vector< SequenceNumber >        _succs;
  vector< QInterval >             _labels;
  vector< SequenceLength >        _lens;
};

// Returns true if first is contained in second
bool
is_contained(const QInterval& first, const QInterval& second)
{
  // Works only for non-overlapping intervals.
  return (second.get_begin() <= first.get_begin() && second.get_end() >= first.get_end());
}

bool
add_dest(struct Node& source, const SequenceNumber& dest, const QInterval& label, const SequenceLength len)
{
  for(vector< QInterval >::const_iterator qint_it = source._labels.begin();
      qint_it != source._labels.end(); qint_it++)
    {
      if(is_contained(label, (*qint_it)))
        return false;
    }
  source._succs.push_back(dest);
  source._labels.push_back(label);
  source._lens.push_back(len);
  return true;
}

void
show_usage(){
  std::cerr << "Usage: sgbuild ";
  std::cerr << " -b <basename> ";
  std::cerr << " -m <max_arc_length> ";
  std::cerr << " -r <read_filename> ";
  std::cerr << " -l <read_length> ";
  std::cerr << " [-c]";
  std::cerr << std::endl;
  std::cerr << std::endl << "Parameters:" << std::endl;
  std::cerr << "\t-b <basename>       # (required)" << std::endl;
  std::cerr << "\t-m <max_arc_length> # (required)" << std::endl;
  std::cerr << "\t-r <read_filename>  # (required) " << std::endl;
  std::cerr << "\t-l <read_length>    # (required) " << std::endl;
  std::cerr << "\t[-c]                # (optional) count edges % default = false" << std::endl;
  std::cerr << std::endl;
}

struct options_t {
  bool initialized;
  std::string basename;
  std::string read_filename;
  SequenceLength max_arc_length;
  SequenceLength read_length;
  bool count_edges;
  bool exhaustive;

  options_t()
      :initialized(false),
       basename(""),
       read_filename(""),
       max_arc_length(0),
       read_length(0),
       count_edges(false),
       exhaustive(false)
  {};
};

std::ostream& operator<<(std::ostream& out, const options_t& opts) {
  if (!opts.initialized) {
    out << "Options not initialized.";
  } else {
    out << "Basename     : " << opts.basename << '\n'
        << "Reads file   : " << opts.read_filename << '\n'
        << "Reads length : " << opts.read_length << '\n'
        << "Max arc len  : " << opts.max_arc_length << '\n'
        << "Count edges  : " << std::boolalpha << opts.count_edges << '\n'
        << "Overlap graph: " << std::boolalpha << opts.exhaustive;
  }
  return out;
}



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
    else if(carg == "-c")
      opts.count_edges= true;
    else {
      std::cerr << "Can't parse argument: " << argv[i] << std::endl;
      show_usage();
      return opts;
    }
  }

  opts.initialized= true;
  std::cerr << opts << std::endl;
  return opts;
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
  long edges = 0;

  if(!opts.initialized)
    return false;

  std::ifstream pmgr_tmp((opts.basename + ".outlsg.lexorder").c_str(),
                         std::ios_base::binary);
  pmgr_tmp.seekg(0, std::ios_base::end);
  const SequenceNumber elem_num = pmgr_tmp.tellg()/sizeof(SequenceNumber);
  pmgr_tmp.close();

  vector< string > reads_ids;

  vector< std::ifstream* > arcsFiles;
  vector< std::ifstream* > labelFiles;

  for(SequenceLength j(0); j < opts.max_arc_length; ++j) {
    const std::string jstr(n2str(j));
    arcsFiles.push_back(new std::ifstream((opts.basename + ".outlsg.arcs_"  + jstr).c_str(),
                                          std::ios_base::in | std::ios_base::binary));
    labelFiles.push_back(new std::ifstream((opts.basename + ".outlsg.labels_"  + jstr).c_str(),
                                           std::ios_base::in | std::ios_base::binary));
  }

  PrefixManager pmgr(opts.basename + ".outlsg.lexorder");

  std::cerr << "Building the graph...";
  { // This block is needed in order to ensure that memory allocated by vector 'graph' is
    // released whenever the graph has been written to the file
  vector< struct Node > graph(elem_num);
  vector< SequenceNumber > d_v;
  vector< SequenceNumber > s_v;
  for(SequenceLength j(0); j < opts.max_arc_length; ++j)
    {
      BWTPosition sourcebegin=0, sourceend=0;
      SequenceNumber destbegin=0, destend=0;
      BWTPosition labelbegin=0, labelend=0;
      while(arcsFiles[j]->read(reinterpret_cast<char*>(&destbegin), sizeof(BWTPosition))       &&
            arcsFiles[j]->read(reinterpret_cast<char*>(&destend), sizeof(BWTPosition))         &&
            arcsFiles[j]->read(reinterpret_cast<char*>(&sourcebegin), sizeof(SequenceNumber))  &&
            arcsFiles[j]->read(reinterpret_cast<char*>(&sourceend), sizeof(SequenceNumber))    &&
            labelFiles[j]->read(reinterpret_cast<char*>(&labelbegin), sizeof(BWTPosition))     &&
            labelFiles[j]->read(reinterpret_cast<char*>(&labelend), sizeof(BWTPosition)))
        {
          pmgr.get_elems(sourcebegin, sourceend - sourcebegin, s_v);
          pmgr.get_elems(destbegin,   destend   - destbegin,   d_v);
          for(vector< SequenceNumber >::size_type i =0; i < s_v.size(); ++i)
            {
              for(vector< SequenceNumber >::size_type k =0; k < d_v.size(); ++k)
                {
                  if(add_dest(graph[s_v[i]], d_v[k], QInterval(labelbegin, labelend), j))
                    ++edges;
                }
            }
        }
    }
  std::cerr << "done." << std::endl;

  std::ofstream graphout( opts.basename + ".outlsg.reduced.graph", std::ios_base::binary);
  std::cerr << "Flushing the graph to file...";
  for(SequenceNumber i =0; i < graph.size(); ++i)
    {
      for(SequenceNumber j =0; j < graph[i]._succs.size(); ++j)
        {
          graphout.write(reinterpret_cast<char*>(&graph[i]._succs[j]), sizeof(SequenceNumber));
          graphout.write(reinterpret_cast<char*>(&i), sizeof(SequenceNumber));
          graphout.write(reinterpret_cast<char*>(&graph[i]._lens[j]), sizeof(SequenceLength));
        }
    }
  graphout.close();
  graph.clear();
  }
  std::cerr << "done." << std::endl;

  std::cerr << "Gathering sequence IDs...";
  gzFile fp;
  kseq_t *seq;
  fp = gzopen(opts.read_filename.c_str(), "r");
  seq = kseq_init(fp);
  while (kseq_read(seq) >= 0)
    {
      reads_ids.push_back(seq->name.s);
      print_vertex(std::cout, seq->name.s, seq->seq.s);
    }
  kseq_destroy(seq);
  gzclose(fp);
  std::cerr << "done." << std::endl;

  std::cerr << "Printing edges..." << std::endl;
  SequenceNumber source =0, dest =0;
  SequenceLength arclen =0;
  std::ifstream graphin( opts.basename + ".outlsg.reduced.graph", std::ios_base::binary);
  while(graphin.read(reinterpret_cast<char*>(&source), sizeof(SequenceNumber)) &&
        graphin.read(reinterpret_cast<char*>(&dest), sizeof(SequenceNumber)) &&
        graphin.read(reinterpret_cast<char*>(&arclen), sizeof(SequenceLength)))
    {
      print_edge(std::cout, reads_ids, source, dest, arclen, opts.read_length);
    }
  graphin.close();
  std::cout.flush();
  std::cerr << "done." << std::endl;

  for(vector< std::ifstream* >::size_type i =0; i < arcsFiles.size(); ++i)
    delete arcsFiles[i];

  for(vector< std::ifstream* >::size_type i =0; i < labelFiles.size(); ++i)
    delete labelFiles[i];

  if(opts.count_edges)
    std::cerr << "Produced " << edges << " edges." << std::endl;

  return 0;
}
