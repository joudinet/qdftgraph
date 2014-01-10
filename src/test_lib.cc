// Copyright (C) 2011, 2012, 2014 Johan Oudinet <oudinet@lri.fr>
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//  
#ifndef QDFT_TEST_LIB_CC
# define QDFT_TEST_LIB_CC
# include <vector>
# include <string>
// # define NO_SIMPLIFY
# include <qdft/qdft.hh>
#include <iostream>
#include <fstream>
#include <ctime>
#include <boost/date_time/posix_time/posix_time.hpp>

// Return used memory by pid (in MiB)
unsigned long proc_mem_usage (int pid = getpid())
{
  unsigned long mem = 0;
  std::string filename = "/proc/" +
			  boost::lexical_cast<std::string>(pid) +
			  "/status";
  std::ifstream ifs (filename.c_str ());

  while (ifs.good ())
    {
      std::string line;
      ifs >> line;
      if (line == "VmSize:")
	{
	  ifs >> mem;
	  ifs >> line;
	  if (line == "MB")
	    mem *= 1024;
	}
    }
  return mem / 1024;
}

#define NS (7)
#define NC (10)

using namespace boost::posix_time;

typedef qdft::data_managers<>	data_managers_t;
typedef data_managers_t::vertices_size_type vertices_size_type;
typedef data_managers_t::edges_size_type edges_size_type;

int overwrite=0;
std::ofstream ftime;

// Simulate a transfer per blocks of a fized size
// Transfer n from src to dst
void block_transfer(data_managers_t& dmanagers,
		    const qdft::dname_type& d,
		    const std::string& src, const std::string& dst,
		    unsigned int n, unsigned int block_size = 512, bool btime = false, unsigned i = 0) {
    std::string tmp = boost::lexical_cast<std::string>(std::rand());
    if (overwrite==1) dmanagers.truncate (d, dst, 0);
    while (n > block_size)
    {
	ptime before (microsec_clock::local_time());
	// transfer src -> tmp
	dmanagers.transfer (d, block_size, src, tmp);
	ptime after (microsec_clock::local_time());
	time_duration td = after - before;
	if (btime) ftime << i << ": " << td.total_microseconds () << std::endl;
	before = microsec_clock::local_time();
	// transfer tmp -> dst
	dmanagers.transfer (d, block_size, tmp, dst);
	after = microsec_clock::local_time();
	td = after - before;
	if (btime) ftime << i << ": " << td.total_microseconds () << std::endl;
	n -= block_size;
    }
    if (n > 0)
    {
	ptime before (microsec_clock::local_time());
	// transfer src -> tmp
	dmanagers.transfer (d, n, src, tmp);
	ptime after (microsec_clock::local_time());
	time_duration td = after - before;
	if (btime) ftime << i << ": " << td.total_microseconds () << std::endl;
	before = microsec_clock::local_time();
	// transfer tmp -> dst
	dmanagers.transfer (d, n, tmp, dst);
	after = microsec_clock::local_time();
	td = after - before;
	if (btime) ftime << i << ": " << td.total_microseconds () << std::endl;
    }
    ptime before (microsec_clock::local_time());
    // truncate tmp
    dmanagers.truncate (d, tmp, 0);
    ptime after (microsec_clock::local_time());
    time_duration td = after - before;
    if (btime) ftime << i << ": " << td.total_microseconds () << std::endl;
}

int main(int argc, char* argv[]) {

    if ((argc < 5)||(argc > 11))
    {
#ifndef NO_SIMPLIFY
	std::cerr << "Simplification rules: activated\n";
#else
	std::cerr << "Simplification rules: deactivated\n";
#endif
	
	std::cerr << "Usage: " << argv[0] << " n block_size p_src p_new [seed] [sensitive_size] [public_size] [overwrite]\n"
		  << "\t n               \t" << "number of transfers\n"
		  << "\t block_size      \t" << "size of atomic transfers\n"
		  << "\t p_src           \t" << "percentage of transfer sources that are initial containers\n"
		  << "\t p_new           \t" << "percentage of transfer destinations that are new reports\n"
		  << "\t seed            \t" << "seed for randomize\n"
		  << "\t sensitive_size  \t" << "size of sensitive source (default:500)\n"
		  << "\t public_size     \t" << "maximal initial containers size, public content (default:100)\n"
		  << "\t overwrite       \t" << "overwrite report upon update 0=NO, 1=YES (default:0)\n" 
		  << "\t time            \t" << "save time measurement to this file (default: no time measurement)\n" 
		  << "\t memory          \t" << "save memory usage to this file (default: no memory usage)\n" 
		  << std::endl;
	exit (1);
    }

  unsigned sensitive_size=500;
  unsigned public_size=100;
	std::ofstream fmem;
  if (argc>6) sensitive_size = boost::lexical_cast<unsigned>(argv[6]);
  if (argc>7) public_size = boost::lexical_cast<unsigned>(argv[7]);
  if (argc>8) overwrite = boost::lexical_cast<unsigned>(argv[8]);
  if (argc>9) ftime.open (argv[9]);
  if (argc>10) fmem.open (argv[10]);

  data_managers_t dmanagers;
  qdft::dname_type d = dmanagers.new_data (NS * sensitive_size, qdft::unknown, "phones");

  typedef std::vector<std::string>	strings_t;
  strings_t sources (NS);
  strings_t containers (NC);

  int i=0;
  for (strings_t::iterator it = sources.begin(); it != sources.end(); ++it)
  {
      *it = std::string("s") + boost::lexical_cast<std::string>(i++);
      dmanagers.init (d, sensitive_size, *it);
  }

  i=0;
  for (strings_t::iterator it = containers.begin(); it != containers.end(); ++it)
  {
      *it = std::string("c") + boost::lexical_cast<std::string>(i++);
  }

  if (argc>=6) 
      std::srand(boost::lexical_cast<unsigned>(boost::lexical_cast<unsigned>(argv[5]))); 
  else 
      std::srand(boost::lexical_cast<unsigned>(std::time(NULL)));
  unsigned n = boost::lexical_cast<unsigned>(argv[1]);
  unsigned block_size = boost::lexical_cast<unsigned>(argv[2]);
  unsigned p_src = boost::lexical_cast<unsigned>(argv[3]);
  unsigned p_new = boost::lexical_cast<unsigned>(argv[4]);

  for (unsigned i = 0; i < n; ++i)
  {
      std::string c1;
      if (std::rand() % 100 < p_src)
	  c1 = sources[std::rand() % NS];
      else
	  c1 = containers[std::rand() % containers.size ()];
      std::string c2;
      if (std::rand() % 100 < p_new)
      {
	  c2 = std::string("c") + boost::lexical_cast<std::string>(containers.size ());
	  containers.push_back (c2);
      }
      else
      {
	  do {
	      c2 = containers[std::rand() % containers.size ()];
	  } while (c2 == c1);
      }
      unsigned r = (std::rand() % public_size) + 1;
      std::cerr << i << ": transferring " << r << " from " << c1 << " to " << c2 << std::endl;
      ptime before (microsec_clock::local_time());
      // dmanagers.transfer (d, r, c1, c2);
      block_transfer (dmanagers, d, c1, c2, r, block_size, (argc > 9), i);
      ptime after (microsec_clock::local_time());
      time_duration td = after - before;
      if (argc>9) ftime << i << ": " << td.total_microseconds () << std::endl;
  }
  if (argc>9) ftime.close ();
  dmanagers.show_graphs ();
  if (argc>10) {
		vertices_size_type n;
		edges_size_type m;
		boost::tie(n, m) = dmanagers.get_graphs_size ();
    fmem << "GraphNodes: " << n << std::endl;
    fmem << "GraphEdges: " << m << std::endl;
    fmem << "Mem: " << proc_mem_usage () << " MiB" << std::endl;
		fmem.close ();
	}
}

#endif // ! QDFT_TEST_LIB_CC
