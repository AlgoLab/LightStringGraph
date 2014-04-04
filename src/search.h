#ifndef SEARCH_H
#define SEARCH_H

#include <algorithm>
#include <vector>
#include <deque>
#include <stxxl.h>

#include "BWTReader.h"
#include "joined_q_int_manager.h"
#include "joined_q_interval.h"
#include "interval_manager.h"
#include "edge_joined_interval.h"
#include "types.h"
#include "util.h"

using std::vector;
using std::deque;

void build_tau_intervals( BWTReader& b, QIntervalManager& jqmgr, vector< NucleoCounter >& C, int T );

// Build all the Q-intervals such that |Q| = T
void build_tau_intervals( BWTReader& b, JoinedQIntervalManager& jqmgr, vector< NucleoCounter >& C, int T );

// Left step search.  
// Given the Q-intervals in jqmgr returns a deque of tuple of intervals
// ( $Q-interval{begin, end}, Q-interval{begin, end} ).
size_t search_step_left( BWTReader& b, JoinedQIntervalManager& jqmgr,
			 vector< NucleoCounter >& C, stxxl::vector<PODEdgeInterval> &v );

// Right step search
// Given the Q-intervals in imgr and in LT extends them and returns a deque of
// tuples of intervals ( suffix-int{begin end}, prefix-int{begin, end} ) such
// that the first interval identifies the reads that have Q as suffix and the
// second the reads that have Q as prefix.
// IT DELETES THE CONTENT OF LT
// vector< EdgeInterval* >*
void search_step_right( BWTReader& b, EdgeJoinedQIntervalManager& imgr,
			vector< NucleoCounter >& C, stxxl::vector<PODEdgeInterval> &v );

// Count the occurrences of characters lexicographically smaller than base.
BWTPosition OccLT( vector< NucleoCounter >& occ, Nucleotide base );

// Returns true if a.reverse comes first than b.reverse, false otherwise
bool CompareJoinedQInterval( JoinedQInterval* a, JoinedQInterval* b );

// Returns true if a.first comes first than b.first, false otherwise
bool CompareEdgeInterval( EdgeInterval* a, EdgeInterval* b );

// Returns false if a.first comes first than b.first, false otherwise
bool CompareEdgeIntervalReverse( EdgeInterval* a, EdgeInterval* b );

// Returns true if a.first is the same as b.first, false otherwise
bool EqualFirstEdgeInterval( EdgeInterval* a, EdgeInterval* b );

// Returns true if a.second is the same as b.second, false otherwise
bool EqualSecondEdgeInterval( EdgeInterval* a, EdgeInterval* b );

// Legacy. Don't you dare using it.
bool int_overlap( const QInterval& a, const QInterval& b );

#endif
