// -*- c++ -*-

#ifndef ARC_INTERVAL_H
#define ARC_INTERVAL_H

#include "types.h"
#include "q_interval.h"

struct SeedInterval
{
  const SequenceNumber begin;
  const SequenceNumber end;

  SeedInterval(const SequenceNumber& begin_,
					const SequenceNumber& end_)
		:begin(begin_), end(end_)
  {}

  const size_t size() const {
	 return end-begin;
  }
  
};

struct ArcInterval
{
  const QInterval es_interval;
  const SequenceLength ext_len;
  const SeedInterval seed_int;

  // Constructor
  ArcInterval(const QInterval& es_interval_,
				  const SequenceLength& ext_len_,
				  const SeedInterval& seed_int_)
		:es_interval(es_interval_), ext_len(ext_len_), seed_int(seed_int_)
  {}

};

#endif
