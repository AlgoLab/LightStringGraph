// -*- c++ -*-

#ifndef SEARCH_H
#define SEARCH_H

#include <algorithm>
#include <vector>
#include <deque>

#ifdef USESTXXL
#include <stxxl.h>
#endif

#include "types.h"
#include "util.h"
#include "BWTReader.h"
#include "GSAReader.h"
#include "LCPReader.h"
#include "BWTIterator.h"
#include "GSAIterator.h"
#include "LCPIterator.h"
#include "PrefixManager.h"
#include "joined_q_int_manager.h"
#include "joined_q_interval.h"
#include "interval_manager.h"
#include "edge_joined_interval.h"
#include "arcInterval.h"
#include "extend_symbol_pile.h"
#include "edgeLabelIntervalManager.h"
#include "MultiIntervalManager.h"

using std::vector;
using std::deque;

// Builds every possibile Q-Intervals of length T in BWT and saves the in qmgr.
// This procedure filters out the Q-intervals for string Q that doesn't occur as
// suffix in any read (using the GSA).
void build_tau_intervals( BWTReader& b, QIntervalManager& qmgr, GSAReader& grdr,
                          vector< NucleoCounter >& C, int T );

// Extend the intervals in qmgr one step on the left
// This procedure filters out the Q-interals for string Q that doesn't occur as
// suffix in any read (using th GSA).
void left_step( BWTReader& b, QIntervalManager& qmgr, GSAReader& grdr,
                vector< NucleoCounter >& C, int iteration );
// Count the occurrences of characters lexicographically smaller than base.
BWTPosition OccLT( vector< NucleoCounter >& occ, Nucleotide base );


// Build all the basic arc intervals of length t > \tau with a single pass
// over the LCP file.
// Input: BWT, LCP, GSA, \tau
// Output: basic arc intervals grouped by starting symbols $\sigma$ and
//    seed length $l$ (via $max{l}$ "QIntervalManager"s)
SequenceLength build_basic_arc_intervals( BWTIterator& bwt,
                                          LCPIterator& lcp,
                                          GSAIterator& gsa,
                                          PrefixManager& pref_mgr,
                                          const SequenceLength& read_length,
                                          const SequenceLength& tau,
                                          const vector< NucleoCounter >& C,
                                          BasicArcIntervalManager& baimgr);

// Extend arc intervals and save extension symbols to an ExtensionSymbolPile
void extend_arc_intervals( const int length,
                           const vector< NucleoCounter >& C,
                           BWTReader& br,
                           GSAIterator& gsait,
                           SameLengthArcIntervalManager& qmgr,
                           SameLengthArcIntervalManager& newqmgr,
                           ExtendSymbolPile& extsym_p,
                           EdgeLabelIntervalManager& arcmgr);

void extend_arc_labels( EdgeLabelIntervalManager& edgemgr,
                        ExtendSymbolPile& extsym_p,
                        const vector< NucleoCounter >& C,
                        BWTReader& br,
                        GSAIterator& gsait,
                        LCPIterator& lcpit,
                        const SequenceLength max_len);

bool equalFirstInterval( const QInterval&, const QInterval& );

// Legacy. Don't you dare using it.
// bool int_overlap( const QInterval& a, const QInterval& b );

// Build all the Q-intervals such that |Q| = T
// void build_tau_intervals( BWTReader& b, JoinedQIntervalManager& jqmgr,
//                           vector< NucleoCounter >& C, int T );

// Left step search.
// Given the Q-intervals in jqmgr returns a deque of tuple of intervals
// ( $Q-interval{begin, end}, Q-interval{begin, end} ).
// size_t search_step_left( BWTReader& b, JoinedQIntervalManager& jqmgr,
//         vector< NucleoCounter >& C, stxxl::vector<PODEdgeInterval> &v );

// Right step search
// Given the Q-intervals in imgr and in LT extends them and returns a deque of
// tuples of intervals ( suffix-int{begin end}, prefix-int{begin, end} ) such
// that the first interval identifies the reads that have Q as suffix and the
// second the reads that have Q as prefix.
// IT DELETES THE CONTENT OF LT
// vector< EdgeInterval* >*
// void search_step_right( BWTReader& b, EdgeJoinedQIntervalManager& imgr,
//      vector< NucleoCounter >& C, stxxl::vector<PODEdgeInterval> &v );

// Returns true if a.reverse comes first than b.reverse, false otherwise
// bool CompareJoinedQInterval( JoinedQInterval* a, JoinedQInterval* b );

// Returns true if a.first comes first than b.first, false otherwise
// bool CompareEdgeInterval( EdgeInterval* a, EdgeInterval* b );

// Returns false if a.first comes first than b.first, false otherwise
// bool CompareEdgeIntervalReverse( EdgeInterval* a, EdgeInterval* b );

// Returns true if a.first is the same as b.first, false otherwise
// bool EqualFirstEdgeInterval( EdgeInterval* a, EdgeInterval* b );

// Returns true if a.second is the same as b.second, false otherwise
// bool EqualSecondEdgeInterval( EdgeInterval* a, EdgeInterval* b );

#endif
