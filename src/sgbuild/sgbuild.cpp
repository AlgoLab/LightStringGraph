#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cassert>

#include "types.h"
#include "util.h"
#include "PrefixManager.h"
#include "edgeLabelInterval.h"

using std::string;
using std::vector;

struct Node
{
  SequenceNumber                  _id;
  vector< struct Node* >          _succs;
  vector< QInterval >             _labels;
  vector< SequenceLength >        _lens;
  
  Node(SequenceNumber id) : _id(id) {};
};

void
stretch(vector< struct Node* >& graph, vector< struct Node* >::size_type n)
{
  while(graph.size() <= n)
    {
      graph.push_back(new struct Node(graph.size()));
    }
}

// Returns true if first is contained in second
bool
is_contained(const QInterval& first, const QInterval& second)
{
  // Works only for non-overlapping intervals.
  return (second._begin < first._begin || second._end > first._end);
}

bool
add_dest(struct Node* source, struct Node* dest, const QInterval& label, SequenceLength len)
{
  for(vector< QInterval >::const_iterator qint_it = source->_labels.begin();
      qint_it != source->_labels.end(); qint_it++)
    {
      if(is_contained(label, (*qint_it)))
        return false;
    }
  source->_succs.push_back(dest);
  source->_labels.push_back(label);
  source->_lens.push_back(len);
  return true;
}

void
print_node(struct Node n)
{
  assert(n._succs.size() == n._labels.size() && n._labels.size() == n._lens.size());
  for(unsigned int i=0; i<n._succs.size(); ++i)
    {
      std::cout << n._succs[i]->_id << " -> " << n._id
                << " [" << n._lens[i] -1 << "]" << std::endl;
    }
}

void
show_usage(){
  std::cerr << "Usage: sgbuild ";
  std::cerr << " <basename> ";
  std::cerr << " <maxarclen> ";
  std::cerr << std::endl;
  std::cerr << std::endl << "Parameters:" << std::endl;
  std::cerr << "\t<basename>     # (required)" << std::endl;
  std::cerr << "\t<maxarclen>    # (required)" << std::endl;
  std::cerr << std::endl;
}

//Main

int
main(int argc, char** argv)
{
  if(argc != 3)
    {
      show_usage();
      return 1;
    }

  vector< struct Node* > graph;

  vector< std::ifstream* > arcsFiles;
  vector< std::ifstream* > labelFiles;

  string basename(argv[1]);
  SequenceLength maxarclen = atoi(argv[2]);

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
  PrefixManager pmgr(basename + ".outlsg.lexorder");

  for(SequenceLength j(0); j < maxarclen; ++j)
    {
      BWTPosition sourcebegin, sourceend;
      SequenceNumber destbegin, destend;
      BWTPosition labelbegin, labelend;
      while(arcsFiles[j]->read(reinterpret_cast<char*>(&destbegin), sizeof(BWTPosition))       &&
            arcsFiles[j]->read(reinterpret_cast<char*>(&destend), sizeof(BWTPosition))         &&
            arcsFiles[j]->read(reinterpret_cast<char*>(&sourcebegin), sizeof(SequenceNumber))  &&
            arcsFiles[j]->read(reinterpret_cast<char*>(&sourceend), sizeof(SequenceNumber))    &&
            labelFiles[j]->read(reinterpret_cast<char*>(&labelbegin), sizeof(BWTPosition))     &&
            labelFiles[j]->read(reinterpret_cast<char*>(&labelend), sizeof(BWTPosition)))
        {
          vector< SequenceNumber > d_v;
          vector< SequenceNumber > s_v;
          pmgr.get_elems(sourcebegin, sourceend - sourcebegin, s_v);
          pmgr.get_elems(destbegin,   destend   - destbegin,   d_v);
          for(vector< SequenceNumber >::size_type i =0; i < s_v.size(); ++i)
            for(vector< SequenceNumber >::size_type j =0; j < d_v.size(); ++j)
              {
                stretch(graph, std::max(s_v[i], d_v[j]));
                add_dest(graph[s_v[i]], graph[d_v[j]], QInterval(labelbegin, labelend), maxarclen);
              }
        }
    }

  for(vector< struct Node* >::size_type i =0; i < graph.size(); ++i)
      print_node(*graph[i]);

  for(vector< struct Node* >::size_type i =0; i < graph.size(); ++i)
      delete graph[i];

  for(vector< std::ifstream* >::size_type i =0; i < arcsFiles.size(); ++i)
    delete arcsFiles[i];

  for(vector< std::ifstream* >::size_type i =0; i < labelFiles.size(); ++i)
    delete labelFiles[i];

  return 0;
}
