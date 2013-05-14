#ifndef SEARCH_H
#define SEARCH_H

#include <algorithm>

#include "BWTReader.h"
#include "joined_q_int_manager.h"
#include "joined_q_interval.h"
#include "interval_manager.h"
#include "types.h"
#include "util.h"

void build_tau_intervals( BWTReader&, JoinedQIntervalManager&, vector< NucleoCounter >&,
			  int);

vector< JoinedQInterval* >* search_step_left( BWTReader&, JoinedQIntervalManager&, vector< NucleoCounter >& );
BWTPosition OccLT( vector< NucleoCounter >&, Nucleotide );

bool CompareJoinedQInterval( JoinedQInterval*, JoinedQInterval*);

#endif
