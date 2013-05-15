#ifndef SEARCH_H
#define SEARCH_H

#include <algorithm>
#include <deque>

#include "BWTReader.h"
#include "joined_q_int_manager.h"
#include "joined_q_interval.h"
#include "interval_manager.h"
#include "edge_joined_interval.h"
#include "types.h"
#include "util.h"

using std::deque;

void build_tau_intervals( BWTReader&, JoinedQIntervalManager&, vector< NucleoCounter >&, int);

deque< EdgeInterval* >* search_step_left( BWTReader&, JoinedQIntervalManager&, 
					  vector< NucleoCounter >& );

deque< EdgeInterval* >* search_step_right( BWTReader&, EdgeJoinedQIntervalManager&, 
					   vector< NucleoCounter >&, deque< EdgeInterval* >* );

BWTPosition OccLT( vector< NucleoCounter >&, Nucleotide );
			
bool CompareJoinedQInterval( JoinedQInterval*, JoinedQInterval*);

bool CompareEdgeInterval( EdgeInterval*, EdgeInterval* );

bool EqualFirstEdgeInterval( EdgeInterval*, EdgeInterval* );

bool EqualSecondEdgeInterval( EdgeInterval* , EdgeInterval* );

bool int_overlap( const QInterval&, const QInterval& );

#endif
