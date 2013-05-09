#ifndef SEARCH_H
#define SEARCH_H

#include "BWTReader.h"
#include "joined_q_int_manager.h"
#include "types.h"
#include "util.h"

void search_step_left( BWTReader&, JoinedQIntervalManager&, vector< NucleoCounter >& );
BWTPosition OccLT( vector< NucleoCounter >&, Nucleotide );

#endif
