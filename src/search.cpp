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
#include "search.h"

#include <stack>
#include <vector>
#include <ostream>

BWTPosition OccLT( vector< NucleoCounter >& occ, Nucleotide base )
{
  // Count occurrences of bases lexicographically less than base given occ
  BWTPosition p =0;

  for(unsigned int i( 0 ); i < (unsigned int) base; ++i )
    p += (BWTPosition) occ[ i ];

  return p;
}

#define _LOG_RECORD                                                     \
  DEBUG_LOG("  pos: " << p                                              \
            << "  sigma: " << NuclConv::ntoc((Nucleotide)Ci)            \
            << "  c_lcp: " << PRINT_SL(lcur)                            \
            << "  n_lcp: " << PRINT_SL(lnext)                           \
            << "  bwt: " << (use_bwt?*bwt:'.')                          \
            << "  gsa: (" <<  PRINT_SL((*gsa).sa) << ")"                \
            )


static void next_record(BWTIterator& bwt,
								LCPIterator& lcp,
								GSAIterator& gsa,
								BWTPosition& p,
								LCPValue& lcur,
								LCPValue& lnext,
								SequenceLength& max_len,
								const vector< NucleoCounter >& C,
								vector< NucleoCounter >::size_type& prev_Ci,
								vector< NucleoCounter >::size_type& Ci,
								const bool use_bwt
								) {
  ++p;    // Position
  ++gsa;  // Generalized Suffix Array
  if (gsa==GSAIterator::end())
	 return;
  max_len= std::max(max_len, (*gsa).sa);
  if (use_bwt) ++bwt;  // BWT (if used)

  // LCP (current and next)
  lcur= *lcp;
  ++lcp;
  lnext= (lcp == LCPIterator::end()) ? 0 : *lcp;

  // starting symbol
  while ((prev_Ci + 1 < C.size()) && (p > C[prev_Ci+1])) ++prev_Ci;
  while ((Ci + 1 < C.size()) && (p >= C[Ci+1])) ++Ci;

  // print to log
  _LOG_RECORD;
}

struct stack_e_elem_t {
  BWTPosition b;
  BWTPosition e;
  LCPValue k;
  SequenceNumber no_of_$;

  explicit stack_e_elem_t(const BWTPosition b_,
                          const BWTPosition e_,
                          const LCPValue k_,
                          const SequenceNumber no_of_$_)
      :b(b_), e(e_), k(k_), no_of_$(no_of_$_)
  {
  }

};

std::ostream& operator<<(std::ostream& os, const stack_e_elem_t& el) {
  os << "( [" << el.b << ", " << el.e << "), " << PRINT_SL(el.k) << ", " << el.no_of_$ << ")";
  return os;
}


SequenceLength build_basic_arc_intervals( BWTIterator& bwt,
                                          LCPIterator& lcp,
                                          GSAIterator& gsa,
                                          const SequenceLength& read_length,
                                          const SequenceLength& tau,
                                          const vector< NucleoCounter >& C,
                                          BasicArcIntervalManager& baimgr)
{
  typedef std::stack<stack_e_elem_t, std::vector<stack_e_elem_t> > stack_e_t;

  const bool use_bwt= (read_length == 0);

  stack_e_t stack_e;

  BWTPosition p= 0;  // current position (on LCP/BWT/...)

  LCPValue lcur= *lcp;
  ++lcp;
  LCPValue lnext= (lcp == LCPIterator::end()) ? 0 : *lcp;

  vector< NucleoCounter >::size_type Ci= 0;
  vector< NucleoCounter >::size_type prev_Ci= 0;

  SequenceLength max_len = 0; // Max length found

  _LOG_RECORD;

  bool opening_block= false;
  BWTPosition ob_b= 0, ob_e= 0;
  LCPValue suff_len= 0;
  SequenceNumber no_of_$= 0;
  SequenceNumber no_of_$_at_block_begin= 0;
  while (gsa != GSAIterator::end()) {
    if (opening_block && (((*gsa).sa != suff_len) || (lcur != suff_len))) {
      opening_block= false;
      ob_e= p;
      _FAIL_IF(ob_b >= ob_e);
      stack_e.push(stack_e_elem_t(ob_b, ob_e, suff_len, no_of_$_at_block_begin));
      DEBUG_LOG("  Added element " << stack_e.top()
                << " to stack_e(" << stack_e.size() << ").");
    }

// Closing some superblocks
	 while(!stack_e.empty() && stack_e.top().k > lcur ) {
		DEBUG_LOG("Closing a " << PRINT_SL(stack_e.top().k) << "-superblock.");
		if (stack_e.top().no_of_$ < no_of_$) {
		  DEBUG_LOG("Adding the basic_arc_interval "
						<< "( [ " << stack_e.top().b << ", " << stack_e.top().e << "), "
						<< "[ " << stack_e.top().no_of_$ << ", " << no_of_$ << "), "
						<< "0) to file "
						<< "E^0( " << NuclConv::ntoc((Nucleotide)prev_Ci) << ", " << (stack_e.top().k+1) << ").");
// Add the basic_arc_interval to the interval manager with sigma=prev_Ci and Length=stack_e.top().k+1
// (Note: '+1' is because '$' was not considered.)
        baimgr[stack_e.top().k+1].                                      \
          add_interval(ArcInterval(QInterval(stack_e.top().b, stack_e.top().e),
                                   0,
                                   SeedInterval(stack_e.top().no_of_$,
                                                no_of_$-stack_e.top().no_of_$)),
                       (Nucleotide)prev_Ci);
		}
		stack_e.pop();
	 }

	 // Found a new prefix
    if (//NOTE: condition '!stack-e.empty()' has been removed since we want that
        //the "prefix"-file contain the lexicographical order of ALL the reads, and not only
        //the ones "involved" in some superblocks.
        //(!stack_e.empty()) &&         // some supeblock is active AND
        (use_bwt && *bwt == '$') ||  // ( using bwt and the bwt symbol is $  OR
        (((*gsa).sa == read_length) && (p >= C[BASE_A])) //   elem of GSA is a "complete read" )
        ) {
      ++no_of_$;
      DEBUG_LOG("  Found the " << no_of_$ << "-th read of the lexicographic order.");
    }


// Opening a superblock
	 if ((lcur<lnext) && (lnext >= tau) && ((*gsa).sa == lnext)) {
		DEBUG_LOG("Possibly opening a " << PRINT_SL(lnext) << "-superblock at position " << p << ".");
		opening_block= true;
		ob_b= p;
		suff_len= lnext;
		no_of_$_at_block_begin= no_of_$;
	 }

	 next_record(bwt, lcp, gsa, p, lcur, lnext, max_len, C, prev_Ci, Ci, use_bwt);
  }

// Closing remaining superblocks
  while(!stack_e.empty()) {
    DEBUG_LOG("Closing a " << PRINT_SL(stack_e.top().k) << "-superblock.");
    if (stack_e.top().no_of_$ < no_of_$) {
      DEBUG_LOG("Adding the basic_arc_interval "
                << "( [ " << stack_e.top().b << ", " << stack_e.top().e << "), "
                << "[ " << stack_e.top().no_of_$ << ", " << no_of_$ << "), "
                << "0) to file "
                << "E^0( " << NuclConv::ntoc((Nucleotide)prev_Ci) << ", " << (stack_e.top().k+1) << ").");
// Add the basic_arc_interval to the interval manager with sigma=prev_Ci and Length=stack_e.top().k+1
// (Note: '+1' is because '$' was not considered.)
      baimgr[stack_e.top().k+1].                                        \
        add_interval(ArcInterval(QInterval(stack_e.top().b, stack_e.top().e),
                                 0,
                                 SeedInterval(stack_e.top().no_of_$,
                                              no_of_$-stack_e.top().no_of_$)),
                     (Nucleotide)prev_Ci);
    }
    stack_e.pop();
  }
  baimgr.swap_all_files();

  return max_len;
}

#undef _LOG_RECORD

bool equalFirstInterval( const QInterval& a, const QInterval& b )
{
  return (a.get_begin() == b.get_begin() &&
          a.get_end() == b.get_end() );
}

void extend_arc_intervals( const int length,
                           const vector< NucleoCounter >& C,
                           BWTReader& br,
                           SameLengthArcIntervalManager& qmgr,
                           SameLengthArcIntervalManager& newqmgr,
                           ExtendSymbolPile& extsym_p,
                           EdgeLabelIntervalManager& edlblmgr,
                           EndPosManager& endpos_mgr,
                           OutputMultiFileManager& arcsOut )
{
  unsigned long int nwintc =0;     // New produced intervals
  unsigned long int nwarcs  =0;    // New arcs
  unsigned long int nwintmgr=0;    // Intrvals read from qmgr
  unsigned long int nwintnwmgr =0; // Intervals from neqmgr

  ArcInterval *qint, *newqint;
  qint = qmgr.get_next_interval( );
  newqint = newqmgr.get_next_interval( );

  QInterval lastInterval(0,0);

  vector< NucleoCounter > PI;
  vector< NucleoCounter > pi;

  bool $_extension = false;

  while((qint != NULL) || (newqint != NULL))
    {
      ArcInterval         *currentInterval;
      bool                 is_new_interval = false;
      bool                 from_qmgr = true;
      vector< Nucleotide > extendsymbols;

      if((newqint == NULL && qint != NULL) ||
         (qint != NULL && qint->es_interval.get_begin() < newqint->es_interval.get_begin()))
        {
          currentInterval = qint;
          from_qmgr = true;
          ++nwintmgr;
        }
      else
        {
          currentInterval = newqint;
          is_new_interval = true;
          from_qmgr = false;
          ++nwintnwmgr;
        }

      DEBUG_LOG("Read Interval [" << currentInterval->es_interval.get_begin()
                << "," << currentInterval->es_interval.get_end() << ") from_qmgr?" << from_qmgr);

      if(currentInterval->es_interval != lastInterval)
        {
          // New qinterval
          lastInterval = currentInterval->es_interval;
          $_extension = false;
          for(BWTPosition i(br.get_position()); i <= currentInterval->es_interval.get_begin(); ++i)
              br.move_to( i );

          PI = vector< NucleoCounter >( br.get_Pi() );

          for(BWTPosition currentPosition=currentInterval->es_interval.get_begin();
              currentPosition < currentInterval->es_interval.get_end();
              ++currentPosition)
            {
              br.move_to(currentPosition);
              if(br.get_current_nucleotide() == BASE_$)
                $_extension = true;
            }
          br.move_to(currentInterval->es_interval.get_end());
        }

      pi = vector< NucleoCounter >( br.get_Pi() );

      if($_extension && currentInterval->ext_len>0)
        {
          const SequenceNumber begin$pos = (SequenceNumber)PI[BASE_$],
            end$pos = (SequenceNumber)pi[BASE_$];

          arcsOut[currentInterval->ext_len].write(reinterpret_cast<const char*>(&begin$pos),
                                                  sizeof(SequenceNumber));
          arcsOut[currentInterval->ext_len].write(reinterpret_cast<const char*>(&end$pos),
                                                  sizeof(SequenceNumber));
          arcsOut[currentInterval->ext_len].write(reinterpret_cast<const char*>(&(currentInterval->seed_int.begin)),
                                                  sizeof(SequenceNumber));
          const SequenceNumber seedend = currentInterval->seed_int.begin + currentInterval->seed_int.end;
          arcsOut[currentInterval->ext_len].write(reinterpret_cast<const char*>(&(seedend)),
                                                  sizeof(SequenceNumber));
          ++nwarcs;
          DEBUG_LOG("Add BASE_$");
          extendsymbols.push_back( BASE_$ );
        }
      for(int base(BASE_A); base < ALPHABET_SIZE; ++base)
        {
          BWTPosition new_begin = C[ base ] + PI[ base ];
          BWTPosition new_end   = C[ base ] + pi[ base ];
          if(new_end > new_begin)
            {
              ++nwintc;
              DEBUG_LOG("Add BASE_" << NuclConv::ntoc((Nucleotide)base));
              extendsymbols.push_back( (Nucleotide) base );
              if(is_new_interval)
                {
                  // Add intervals of length 0 in a meaninfull way
                  QInterval whole_bwt(0, br.size( ));
                  EdgeLabelInterval void_interval(whole_bwt, whole_bwt);
                  // BASE_A acts as a void base if we add every possible intervals to that file
                  edlblmgr.add_edge_interval( void_interval, 0, BASE_A );
                  // Add the interval only once
                  is_new_interval = false;
                  DEBUG_LOG("Add interval [0," << br.size() << ") to edgelabelmanager");
                }
              QInterval new_q_interval(new_begin, new_end);
              ArcInterval new_arc_interval(new_q_interval,
                                           currentInterval->ext_len +1,
                                           currentInterval->seed_int);
              DEBUG_LOG("Add interval [" << new_begin << "," << new_end
                        << ") to qmgr with ext_len = "
                        << currentInterval->ext_len +1);
              qmgr.add_interval(new_arc_interval, (Nucleotide)base);
            }
        }

      if(extendsymbols.size() > 0)
          extsym_p.add_extend_symbol(extendsymbols, currentInterval->ext_len);

      from_qmgr ? qint = qmgr.get_next_interval() : newqint = newqmgr.get_next_interval();
    }
  DEBUG_LOG( "--> Got " << nwintmgr << " new intervals from qmgr" );
  DEBUG_LOG( "--> Got " << nwintnwmgr << " new intervals from nwqmgr" );
  std::cerr << "--> Generated " << nwintc << " new intervals" << std::endl;
  std::cerr << "--> Produced " << nwarcs << " new edges (still not labeled)" << std::endl;
}

void extend_arc_labels( EdgeLabelIntervalManager& edgemgr,
                        ExtendSymbolPile& extsym_p,
                        const vector< NucleoCounter >& C,
                        BWTReader& br,
                        LCPIterator& lcp,
                        const SequenceLength max_len,
                        OutputMultiFileManager& edgeOut )
{
  struct EdgeStackElem_t
  {
    EdgeLabelEntry _elem;
    long           _mult;

    EdgeStackElem_t(EdgeLabelEntry& e)
    {
      _elem = e;
      _mult = 1;
    }

    EdgeStackElem_t& operator++()
    {
      ++_mult;
      return *this;
    }
  };

  unsigned long nwtermlbc =0;                // New terminal labels counter
  unsigned long nwlbc      =0;               // New labels counter

  struct EPI_t EPI(max_len + 1);             // Stack to store PI values
  vector< int > EPI_p(max_len +1, -1);       // Position on PI in EPI for len i
                                             // If len(label) = i then EPI_p[label.len] = i : EPI[i] = OcC(label.begin)
  EPI_p[0] = 0;                              // Labels of length 0 starts at 0;
  vector< NucleoCounter > pi(ALPHABET_SIZE);

  struct EdgeLabelEntry currentEdge;
  std::stack<EdgeStackElem_t> edgestack;
  br.move_to(0);

  bool edgeread = false;

  while((edgeread = edgemgr.get_next_interval(currentEdge)) || edgestack.size() != 0)
    {
      DEBUG_LOG("position       : " << br.get_position());
      if(edgeread)
        DEBUG_LOG("currentInterval: " << currentEdge._interval.get_label().get_begin()
                  << ", " << currentEdge._interval.get_label().get_end());
      while(edgestack.size() > 0 &&
            ((edgeread && currentEdge._interval.get_label().get_begin() >= edgestack.top()._elem._interval.get_label().get_end()) ||
             (!edgeread)))
        {
          // Move to edgestack.top().end()
          br.move_to(edgestack.top()._elem._interval.get_label().get_end());
          const vector< Nucleotide >& extend_symbols = extsym_p.get_next_symbol(edgestack.top()._elem._len);
          std::copy(br.get_Pi().begin(), br.get_Pi().end(), pi.begin());
          // Extend edgestack.top
          for(vector< Nucleotide >::const_iterator nucl_i = extend_symbols.begin();
              nucl_i != extend_symbols.end(); ++nucl_i)
            {
              DEBUG_LOG("Extending with " << NuclConv::ntoc(*nucl_i) << " (code: " << *nucl_i << ")");
              _FAIL_IF(*nucl_i > BASE_T);
              Nucleotide extension = *nucl_i;
              // Output if end of arc
              if(extension == BASE_$)
                {
                  BWTPosition labbegin = edgestack.top()._elem._interval.get_reverse_label().get_begin();
                  BWTPosition labend   = edgestack.top()._elem._interval.get_reverse_label().get_end();
                  edgeOut[edgestack.top()._elem._len].write(reinterpret_cast<char*>(&labbegin),
                                                  sizeof(BWTPosition));
                  edgeOut[edgestack.top()._elem._len].write(reinterpret_cast<char*>(&labend),
                                                  sizeof(BWTPosition));
                  ++nwtermlbc;
                }
              // Extend otherwise
              else
                {
                  BWTPosition new_begin = C[ extension ] + EPI._occs[ EPI_p[ edgestack.top()._elem._len ] ][extension];
                  BWTPosition new_end   = C[ extension ] + pi[ extension ];
                  BWTPosition old_rev_begin = edgestack.top()._elem._interval.get_reverse_label().get_begin();
                  BWTPosition new_rev_begin, new_rev_end;
                  if(edgestack.top()._elem._len == 0)
                    {
                      new_rev_begin = C[ extension ];
                      new_rev_end   = C[ extension +1 ];
                    }
                  else
                    {
                      new_rev_begin = old_rev_begin + OccLT(pi, extension)
                        - OccLT( EPI._occs[ EPI_p[ edgestack.top()._elem._len ] ], extension );
                      new_rev_end   = new_rev_begin + pi[ extension ]
                        - EPI._occs[ EPI_p[ edgestack.top()._elem._len ] ][ extension ];
                    }
                  EdgeLabelInterval new_interval(QInterval(new_begin, new_end), QInterval(new_rev_begin, new_rev_end));
                  ++nwlbc;
                  DEBUG_LOG("Extended to [" << new_begin << "," << new_end << ")" << " and the reverse interval is ["
                            << new_rev_begin << "," << new_rev_end << ")");
                  edgemgr.add_edge_interval(new_interval, edgestack.top()._elem._len +1, extension);
                }
            } // ~for(extsymbol)
          // Decrease edgestack.top()._mult
          // If _mult == 0 remove top()
          if(--edgestack.top()._mult == 0)
            {
              DEBUG_LOG("Clear EPI[" << PRINT_SL(edgestack.top()._elem._len) << "]");
              EPI_p[edgestack.top()._elem._len] = -1;
              --EPI._next;
              edgestack.pop();
            }
        }

      if(edgeread)
        {
          if(edgestack.size() > 0 && currentEdge._interval == edgestack.top()._elem._interval)
            edgestack.top()._mult++;
          else
            {
              br.move_to(currentEdge._interval.get_label().get_begin());
              std::copy(br.get_Pi().begin(), br.get_Pi().end(), EPI._occs[EPI._next].begin());
              EPI_p[currentEdge._len] = EPI._next;
              ++EPI._next;
              struct EdgeStackElem_t x(currentEdge);
              edgestack.push(x);
            }
        }
    }
  std::cerr << "--> Generated " << nwlbc << " new labels for arcs." << std::endl;
  std::cerr << "--> Labeled " << nwtermlbc << " new arcs." << std::endl;
}
