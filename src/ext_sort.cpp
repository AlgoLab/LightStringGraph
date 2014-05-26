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
 * along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/
// #include <ext_sort.h>

// std::ifstream* ext_sort(const std::string& filename_in)
// {
//   std::ifstream      inf;
//   std::vector<std::ofstream*>  ouf;

//   EdgeInterval* eint;

//   rename(filename_in.c_str(), std::string("in0").c_str());

//   // Init out filenames
//   for(int i=0; i < pow(2, CHAR_BIT); ++i)
//     {
//       std::ostringstream fname;
//       fname << "out" << i;
//       std::ofstream* oufx = new std::ofstream(fname.str().c_str(), std::ios::binary);
//       ouf.push_back(oufx);
//     }

//   // Radix sort elements
//   for(unsigned short int offset=0; offset < BITS_OF_BWT_POSITION; offset+=CHAR_BIT)
//     {
//       for(short int value=1; value <= pow(2, CHAR_BIT); ++value)
//         {
//           std::ostringstream   filename;
//           filename << "in" << value -1;
//           inf.open(filename.str().c_str(), std::ios::binary);
//           while(inf >> &eint)
//             {
//               BWTPosition begin = eint->get_first_interval().get_begin();
//               *(ouf[PTM(begin, offset)]) << *eint;
//             }
//           inf.clear();
//           inf.close();
//         }
//       // Move forward one radix step
//       for(int i=0; i < pow(2, CHAR_BIT); ++i)
//         {
//           ouf[i]->clear();
//           ouf[i]->close();
//           delete ouf[i];
//         }
//       ouf.clear();

//       for(int i=0; i < pow(2, CHAR_BIT); ++i)
//         {
//           std::ostringstream   filename_out, new_filename_in;
//           filename_out << "out" << i;
//           new_filename_in << "in" << i;
//           if(remove(new_filename_in.str().c_str()) != 0)
//             {
//               std::cerr << "Error: Can't remove " << new_filename_in.str()
//                         << std::endl;
//               std::exit(-1);
//             }
//           if(rename(filename_out.str().c_str(), new_filename_in.str().c_str()) != 0)
//             {
//               std::cerr << "Error: Can't rename " << filename_out.str()
//                         << " to " << new_filename_in.str() << std::endl;
//               std::exit(-1);
//             }
//           std::ofstream* oufx = new std::ofstream(filename_out.str().c_str(), std::ios::binary);
//           ouf.push_back(oufx);
//         }
//     }

//   // Close output files and merge ordered intervals
//   std::ofstream  merged_output(MERGED_FILES.c_str( ), std::ios::binary);
//   for(int i=0; i < pow(2, CHAR_BIT); ++i)
//     {
//       ouf[i]->close();
//       delete ouf[i];
//       std::ostringstream   filename;
//       filename << "in" << i;
//       std::ifstream    fin(filename.str().c_str(), std::ios::binary);
//       merged_output << fin;
//       fin.clear();
//       fin.close();
//       remove(filename.str().c_str());
//     }
//   merged_output.flush();
//   merged_output.close();

//   std::ifstream* estream = new std::ifstream( MERGED_FILES.c_str(), std::ios::binary );
//   return estream;
// }

// // std::ifstream* ext_sort( size_t elem_num, std::string filename_in )
// // {
// //   // Radix sort the files

// //   std::string  in_fn[ 2 ]   = { "in0",  "in1"  };
// //   std::string  out_fn[ 2 ]  = { "out0", "out1" };
// //   BWTPosition mask = 0b01;
// //   EdgeInterval* eint;
// //   std::ifstream einF( filename_in.c_str(), std::ios::binary );
// //
// //   if( einF.fail( ) )
// //     {
// //       std::cerr << "ERROR: Can't open file : " << filename_in << std::endl
// //     << "Aborting..." << std::endl;
// //       std::exit( -1 );
// //     }
// //
// //   std::ofstream outF0( in_fn[ 0 ].c_str(), std::ios::binary );
// //   std::ofstream outF1( in_fn[ 1 ].c_str(), std::ios::binary );

// // #ifdef DEBUG_VERBOSE
// //   std::cerr << "BITS_OF_BWT_POSITION = " << BITS_OF_BWT_POSITION << std::endl;
// // #endif

// //   // Splice the elements on the last bit
// //   while(( einF >> &eint ))
// //     {
// //       if( (eint->get_first_interval().get_begin()) & mask )
// //     outF1 << *eint;
// //       else
// //     outF0 << *eint;
// //       delete eint;
// //     }
// //   outF0.flush();
// //   outF1.flush();
// //   outF0.close();
// //   outF1.close();
// //   einF.close();

// //   for( BWTPosition i=1; i < BITS_OF_BWT_POSITION; ++i )
// //     {
// //       mask = 0b01 << i;
// // #ifdef DEBUG_VERBOSE
// //       std::cerr << std::endl << "In ext_sort : i = " << i << std::endl;
// //       std::cerr << std::endl << "mask = " << extsmask[i] << std::endl;
// //       std::cerr << "Expression = " << std::bitset<BITS_OF_BWT_POSITION>(extsmask[i]) << std::endl;
// // #endif
// //       std::ofstream  outF0( out_fn[ 0 ].c_str(), std::ios::binary );
// //       std::ofstream  outF1( out_fn[ 1 ].c_str(), std::ios::binary );
// //       std::ifstream  inF0(   in_fn[ 0 ].c_str(), std::ios::binary );
// //       while((inF0 >> &eint))
// //   {
// // #ifdef DEBUG_VERBOSE
// //     std::cerr << "Element =" << eint->get_first_interval().get_begin() << ", "
// //         << eint->get_first_interval().get_end() << std::endl;
// //     std::cerr << "Expression = " << std::bitset<BITS_OF_BWT_POSITION>(eint->get_first_interval().get_begin())
// //         << std::endl;
// // #endif
// //     if( (eint->get_first_interval().get_begin()) & extsmask[i] )
// //       outF1 << *eint;
// //     else
// //       outF0 << *eint;
// //     delete eint;
// //   }
// //       outF0.flush();
// //       outF1.flush();
// //       inF0.close();
// //       std::ifstream  inF1(   in_fn[ 1 ].c_str(), std::ios::binary );
// //       while((inF1 >> &eint))
// //   {
// // #ifdef DEBUG_VERBOSE
// //     std::cerr << "Element =" << eint->get_first_interval().get_begin() << ", "
// //         << eint->get_first_interval().get_end() << std::endl;
// //     std::cerr << "Expression = " << std::bitset<BITS_OF_BWT_POSITION>(eint->get_first_interval().get_begin())
// //         << std::endl;
// // #endif
// //     if( (eint->get_first_interval().get_begin()) & extsmask[i] )
// //       outF1 << *eint;
// //     else
// //       outF0 << *eint;
// //     delete eint;
// //   }
// //       inF1.close();
// //       outF0.flush();
// //       outF1.flush();
// //       outF0.close();
// //       outF1.close();
// //       remove( in_fn[ 0 ].c_str() );
// //       remove( in_fn[ 1 ].c_str() );
// //       rename( out_fn[ 0 ].c_str(), in_fn[ 0 ].c_str() );
// //       rename( out_fn[ 1 ].c_str(), in_fn[ 1 ].c_str() );
// //     }

// //   // Merge the files
// //   std::ifstream zero_sect    ( in_fn[ 0 ].c_str(),    std::ios::binary );
// //   std::ifstream one_sect     ( in_fn[ 1 ].c_str(),    std::ios::binary );

// //   std::ofstream merged_output( MERGED_FILES.c_str( ), std::ios::binary );

// //   merged_output << zero_sect.rdbuf() << one_sect.rdbuf();
// //   merged_output.flush();
// //   zero_sect.close();
// //   one_sect.close();
// //   merged_output.close();

// //   remove( in_fn[ 0 ].c_str() );
// //   remove( in_fn[ 1 ].c_str() );
// //   std::ifstream* estream = new std::ifstream( MERGED_FILES.c_str(), std::ios::binary );

// // #ifdef DEBUG_VERBOSE
// //   std::cerr << "Ordered intervals.." << std::endl;
// //   while(((*estream) >> &eint))
// //     {
// //       std::cerr << "begin  : " << eint->get_first_interval().get_begin() << std::endl;
// //       std::cerr << "end    : " << eint->get_first_interval().get_end()   << std::endl;
// //       std::cerr << "Expression = " << std::bitset<BITS_OF_BWT_POSITION>(eint->get_first_interval().get_begin())
// //     << std::endl;
// //     }
// //   estream->clear();
// //   estream->seekg(0);
// // #endif

// //   return estream;
// // }
