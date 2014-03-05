#include <ext_sort.h>

std::ifstream* ext_sort( size_t elem_num, string filename_in )
{
  // Radix sort the files

  string  in_fn[ 2 ]   = { "in0",  "in1"  };
  string  out_fn[ 2 ]  = { "out0", "out1" };
  BWTPosition mask = 0b01;
  EdgeInterval* eint;
  std::ifstream einF( filename_in.c_str(), std::ios::binary );

  if( einF.fail( ) )
    {
      std::cerr << "ERROR: Can't open file : " << filename_in << std::endl \
		<< "Aborting..." << std::endl;
      std::exit( -1 );
    }

  std::ofstream outF0( in_fn[ 0 ].c_str(), std::ios::binary );
  std::ofstream outF1( in_fn[ 1 ].c_str(), std::ios::binary );

  // Splice the elements on the last bit
  while(( einF >> &eint ))
    {
      if( (eint->get_first_interval().get_begin()) & mask )
	  outF1 << *eint;
      else
	  outF0 << *eint;
      delete eint;
    }
  outF0.flush();
  outF1.flush();
  outF0.close();
  outF1.close();
  einF.close();

  for( unsigned int i=1; i < BITS_OF_BWT_POSITION; ++i )
    {
      mask = 0b01 << i;
      std::ofstream  outF0( out_fn[ 0 ].c_str(), std::ios::binary );
      std::ofstream  outF1( out_fn[ 1 ].c_str(), std::ios::binary );
      std::ifstream  inF0(   in_fn[ 0 ].c_str(), std::ios::binary );
      while((inF0 >> &eint))
	{
	  if( (eint->get_first_interval().get_begin()) & mask )
	    outF1 << *eint;
	  else
	    outF0 << *eint;
	  delete eint;
	}
      outF0.flush();
      outF1.flush();
      inF0.close();
      std::ifstream  inF1(   in_fn[ 1 ].c_str(), std::ios::binary );
      while((inF1 >> &eint))
	{
	  if( (eint->get_first_interval().get_begin()) & mask )
	    outF1 << *eint;
	  else
	    outF0 << *eint;
	  delete eint;
	}
      inF1.close();
      outF0.flush();
      outF1.flush();
      outF0.close();
      outF1.close();
      remove( in_fn[ 0 ].c_str() );
      remove( in_fn[ 1 ].c_str() );
      rename( out_fn[ 0 ].c_str(), in_fn[ 0 ].c_str() );
      rename( out_fn[ 1 ].c_str(), in_fn[ 1 ].c_str() );
    }

  // Merge the files
  std::ifstream zero_sect    ( in_fn[ 0 ].c_str(),    std::ios::binary );
  std::ifstream one_sect     ( in_fn[ 1 ].c_str(),    std::ios::binary );

  std::ofstream merged_output( MERGED_FILES.c_str( ), std::ios::binary );

  merged_output << zero_sect.rdbuf() << one_sect.rdbuf();
  merged_output.flush();
  zero_sect.close();
  one_sect.close();
  merged_output.close();

  remove( in_fn[ 0 ].c_str() );
  remove( in_fn[ 1 ].c_str() );
  std::ifstream* estream = new std::ifstream( MERGED_FILES.c_str(), std::ios::binary );
  return estream;
}
