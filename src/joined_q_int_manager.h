//
//
// LEGACY
//
//

/* #ifndef Q_INT_MANAGER_H */
/* #define Q_INT_MANAGER_H */

/* /\* #include "interval_manager.h" *\/ */
/* /\* #include "joined_q_interval.h" *\/ */

/* /\* typedef IntervalManager< JoinedQInterval > JoinedQIntervalManager; *\/ */

/* /\* #include <vector> *\/ */
/* /\* #include <string> *\/ */
/* /\* #include <cstdlib> *\/ */
/* /\* #include <iostream> *\/ */
/* /\* #include <sstream> *\/ */
/* /\* #include <cstdio> *\/ */
/* /\* #include <fstream> *\/ */

/* /\* #include <joined_q_interval.h> *\/ */
/* /\* #include <types.h> *\/ */
/* /\* #include <util.h> *\/ */

/* /\* using std::vector; *\/ */
/* /\* using std::string; *\/ */

/* /\* class JoinedQIntervalManager *\/ */
/* /\* { *\/ */
/* /\* private: *\/ */
/* /\*   vector< string > _filenames; // Input filenames *\/ */
/* /\*   unsigned short _nextInputFile; *\/ */
/* /\*   std::ifstream* _inputFile; *\/ */
/* /\*   vector< std::ofstream* > _outputFiles; // new Q-intervals will be *\/ */
/* /\*             // written here *\/ */
/* /\*   vector< JoinedQInterval* > _buffer; *\/ */
/* /\*   vector< JoinedQInterval* >::iterator _nextInterval; *\/ */

/* /\* public: *\/ */
/* /\*   // Instantiate a Manager over filenames files *\/ */
/* /\*   // TODO: create a new constructor that accept a prefix (string) and *\/ */
/* /\*   // an int and automagically creates the vector *\/ */
/* /\*   JoinedQIntervalManager ( vector< string >& filenames ); *\/ */

/* /\*   // Move next q-intervals to current q-intervals (so we can read *\/ */
/* /\*   // them) and create new output files *\/ */
/* /\*   void swap_files ( ); *\/ */

/* /\*   // Get next interval. If no intervals are left, return NULL *\/ */
/* /\*   JoinedQInterval* get_next_interval ( ); *\/ */

/* /\*   // Append i to outputfile[ n ] *\/ */
/* /\*   bool add_q_interval ( JoinedQInterval& i, Nucleotide n ); *\/ */

/* /\*   // Destructor *\/ */
/* /\*   ~JoinedQIntervalManager ( ); *\/ */

/* /\* private: *\/ */

/* /\*   // Add BUFFERSIZE joined_q_intervals to _buffer *\/ */
/* /\*   // BUFFERSIZE should be defined in types.h *\/ */
/* /\*   void _populate_buffer ( ); *\/ */

/* /\*   // Create new outputfiles (filename + "_next") *\/ */
/* /\*   void _init_new_outputfiles ( ); *\/ */


/* /\*   JoinedQIntervalManager ( )  *\/ */
/* /\*   { } *\/ */
/* /\*   JoinedQIntervalManager ( JoinedQIntervalManager& other ) *\/ */
/* /\*   { } *\/ */
/* /\*   JoinedQIntervalManager& operator= ( JoinedQIntervalManager& other ) *\/ */
/* /\*   { return *this; } *\/ */
/* /\* }; *\/ */

/* #endif */
