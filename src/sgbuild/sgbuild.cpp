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
#include <zlib.h>
#include <stdio.h>
#include <cassert>

#include "types.h"
#include "util.h"
#include "EndPosManager.h"
#include "edgeLabelInterval.h"

#include "kseq.h"
KSEQ_INIT(gzFile, gzread)

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
  std::cerr << " -m <maxarclen> ";
  std::cerr << " -r <readsfilename> ";
  std::cerr << " -l <readslen> ";
  std::cerr << " [-c]";
  std::cerr << " [-x]";
  std::cerr << std::endl;
  std::cerr << std::endl << "Parameters:" << std::endl;
  std::cerr << "\t-b <basename>       # (required)" << std::endl;
  std::cerr << "\t-m <maxarclen>      # (required)" << std::endl;
  std::cerr << "\t-r <readsfilename>  # (required) " << std::endl;
  std::cerr << "\t-l <readslen>       # (required) " << std::endl;
  std::cerr << "\t[-c]                # (optional) count edges % default = false" << std::endl;
  std::cerr << "\t[-x]                # (optional) exaustive   % default = false" << std::endl;
  std::cerr << std::endl;
}

bool
parse_cmds(const int argc, char** argv, string& basename, string& readsfilename,
           SequenceLength& maxarclen, int& readslen, bool& exaustive,
           bool& countedg)
{
  if(argc < 7)
    {
      show_usage();
      return false;
    }
  for(int i=1; i<argc; ++i)
    {
      if(string(argv[i]) == "-b")
        basename = string(argv[++i]);
      else if(string(argv[i]) == "-r")
        readsfilename = string(argv[++i]);
      else if(string(argv[i]) == "-m")
        maxarclen = atoi(argv[++i]);
      else if(string(argv[i]) == "-l")
        readslen = atoi(argv[++i]);
      else if(string(argv[i]) == "-x")
        exaustive = true;
      else if(string(argv[i]) == "-c")
        countedg = true;
      else
        {
          std::cerr << "Can't parse argument: " << argv[i] << std::endl;
          show_usage();
          return false;
        }
    }

  std::cerr << "Basename       : " << basename << std::endl
            << "Reads file     : " << readsfilename << std::endl
            << "Reads length   : " << readslen << std::endl
            << "Max arcs len   : " << maxarclen << std::endl;
  std::cerr << "Options: " << std::endl
            << "   count-edges : ";
  countedg ? std::cerr << "true" : std::cerr << "false";
  std::cerr << std::endl;
  std::cerr << "   exaustive   : ";
  exaustive ? std::cerr << "true" : std::cerr << "false";
  std::cerr << std::endl;

  return true;
}

void
print_vertex(const kseq_t* seq)
{
  std::cout << "VT\t" << seq->name.s <<"\t" << seq->seq.s << std::endl;
}

void
print_edge(const vector< string >& ids, const unsigned int source, const unsigned int dest,
           const int overlap, const int readslen)
{
  // Fields:
  // 0.  string ED
  // 1.  sequence 1 name
  // 2.  sequence 2 name
  // 3.  sequence 1 overlap start (0 based)
  // 4.  sequence 1 overlap end (inclusive)
  // 5.  sequence 1 length
  // 6.  sequence 2 overlap start (0 based)
  // 7.  sequence 2 overlap end (inclusive)
  // 8.  sequence 2 length
  // 9.  sequence 2 orientation (1 for reversed with respect to sequence 1)
  // 10. number of differences in overlap (0 for perfect overlaps, which is the default).
  std::cout << "ED " << ids[source] << " " << ids[dest] << " " << overlap
            << " " << readslen-1 << " " << readslen << " 0 " << readslen -overlap -1 << " " << readslen
            << " 0 0" << std::endl;
}

//Main

int
main(int argc, char** argv)
{
  bool exaustive = false;
  bool countedg  = false;
  string basename, readsfilename;
  int readslen =0;
  SequenceLength maxarclen;
  long edges = 0;

  if(!parse_cmds(argc, argv, basename, readsfilename, maxarclen, readslen, exaustive, countedg))
    return false;

  std::ifstream eomgr_tmp((basename + "-end-pos").c_str(),
                          std::ios_base::binary);
  SequenceNumber tmp_ = 0;
  eomgr_tmp.read( reinterpret_cast<char*>(&tmp_), sizeof( SequenceNumber ) );
  const SequenceNumber no_of_reads = tmp_;
  eomgr_tmp.close();

  vector< struct Node > graph(no_of_reads);

  vector< string > reads_ids;

  std::cerr << "Gathering sequence IDs...";
  gzFile fp;
  kseq_t *seq;
  fp = gzopen(readsfilename.c_str(), "r");
  seq = kseq_init(fp);
  while (kseq_read(seq) >= 0)
    {
      reads_ids.push_back(string(seq->name.s));
      print_vertex(seq);
    }
  kseq_destroy(seq);
  gzclose(fp);
  std::cerr << "done." << std::endl;

  vector< std::ifstream* > arcsFiles;
  vector< std::ifstream* > labelFiles;

  for(SequenceLength j(0); j < maxarclen; ++j)
    {
      std::stringstream arcFilename;
      std::stringstream labelFilename;
      arcFilename << basename << ".outlsg.arcs_" << j;
      arcsFiles.push_back(new std::ifstream(arcFilename.str().c_str(),
                                            std::ios_base::in | std::ios_base::binary));
      labelFilename << basename << ".outlsg.labels_" << j;
      labelFiles.push_back(new std::ifstream(labelFilename.str().c_str(),
                                             std::ios_base::in | std::ios_base::binary));
    }

  EndPosManager eomgr(basename + "-end-pos");

  std::cerr << "Building the graph...";
  for(SequenceLength j(0); j < maxarclen; ++j)
    {
      SequenceNumber sourcebegin=0, sourceend=0;
      SequenceNumber destbegin=0, destend=0;
      BWTPosition labelbegin=0, labelend=0;
      while(arcsFiles[j]->read(reinterpret_cast<char*>(&destbegin), sizeof(SequenceNumber))    &&
            arcsFiles[j]->read(reinterpret_cast<char*>(&destend), sizeof(SequenceNumber))      &&
            arcsFiles[j]->read(reinterpret_cast<char*>(&sourcebegin), sizeof(SequenceNumber))  &&
            arcsFiles[j]->read(reinterpret_cast<char*>(&sourceend), sizeof(SequenceNumber))    &&
            labelFiles[j]->read(reinterpret_cast<char*>(&labelbegin), sizeof(BWTPosition))     &&
            labelFiles[j]->read(reinterpret_cast<char*>(&labelend), sizeof(BWTPosition)))
        {
          vector< SequenceNumber > d_v;
          vector< SequenceNumber > s_v;
          eomgr.get_elems(sourcebegin, sourceend - sourcebegin, s_v);
          eomgr.get_elems(destbegin,   destend   - destbegin,   d_v);
          for(vector< SequenceNumber >::size_type i =0; i < s_v.size(); ++i)
            {
              for(vector< SequenceNumber >::size_type k =0; k < d_v.size(); ++k)
                {
                  if(!exaustive)
                    {
                      if(add_dest(graph[s_v[i]], d_v[k], QInterval(labelbegin, labelend), j))
                          ++edges;
                    }
                  else
                    {
                      print_edge(reads_ids, d_v[k], s_v[i], j, readslen);
                      ++edges;
                    }
                }
            }
        }
    }
  std::cerr << "done." << std::endl;

  if(!exaustive)
    {
      std::cerr << "Output edges to stdout...";
      for(unsigned int i =0; i < graph.size(); ++i)
        for(unsigned int j=0; j < graph[i]._succs.size(); ++j)
          print_edge(reads_ids, graph[i]._succs[j], i, graph[i]._lens[j], readslen);
      std::cerr << "done." << std::endl;
    }

  for(vector< std::ifstream* >::size_type i =0; i < arcsFiles.size(); ++i)
    delete arcsFiles[i];

  for(vector< std::ifstream* >::size_type i =0; i < labelFiles.size(); ++i)
    delete labelFiles[i];

  if(countedg)
    std::cerr << "Produced " << edges << " edges." << std::endl;

  return 0;
}
