// Copyright (C) 2011, 2012 Johan Oudinet <oudinet@lri.fr>
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
#ifndef QDFT_EXAMPLE_CC
# define QDFT_EXAMPLE_CC
// # include <vector>
# include <string>
# include <qdft/qdft.hh>

typedef qdft::data_managers<>	data_managers_t;

// Simulate a transfer per blocks of a fized size
// Transfer n from src to dst
void block_transfer(data_managers_t& dmanagers,
										const qdft::dname_type& d,
										const std::string& src, const std::string& dst,
										unsigned int n, unsigned int block_size = 512) {
	std::string tmp = boost::lexical_cast<std::string>(std::rand());
	while (n > block_size)
		{
			dmanagers.transfer (d, block_size, src, tmp);
			dmanagers.transfer (d, block_size, tmp, dst);
			n -= block_size;
		}
	if (n > 0)
		{
			dmanagers.transfer (d, n, src, tmp);
			dmanagers.transfer (d, n, tmp, dst);
		}
	dmanagers.truncate (d, tmp, 0);
}


int main() {
  data_managers_t dmanagers;

	std::cerr << "*** Test for reverting truncate ***" << std::endl;
	qdft::dname_type dt1 = dmanagers.new_data (50, qdft::last, "dt1");
  dmanagers.init (dt1, 50, "c1");
  dmanagers.truncate (dt1, "c1", 10);
  dmanagers.revert_last_action (dt1);

	std::cerr << "*** Test for reverting simplification rule 1 ***" << std::endl;
	qdft::dname_type ds1 = dmanagers.new_data (50, qdft::last, "ds1");
  dmanagers.init (ds1, 50, "c1");
  dmanagers.truncate (ds1, "c1", 0);
  dmanagers.revert_last_action (ds1);
  dmanagers.truncate (ds1, "c1", 0);

	std::cerr << "*** Test for reverting simplification rule 2 ***" << std::endl;
	qdft::dname_type ds2 = dmanagers.new_data (50, qdft::last, "ds2");
  dmanagers.init (ds2, 50, "c1");
  dmanagers.transfer (ds2, 10, "c1", "c2");
  dmanagers.transfer (ds2, 5, "c2", "c3");
  dmanagers.transfer (ds2, 5, "c2", "c3");
  dmanagers.revert_last_action (ds2);

	std::cerr << "*** Test for reverting simplification rule 3 ***" << std::endl;
	qdft::dname_type ds3 = dmanagers.new_data (50, qdft::last, "ds3");
  dmanagers.init (ds3, 50, "c1");
  dmanagers.transfer (ds3, 10, "c1", "c2");
  dmanagers.transfer (ds3, 20, "c2", "c3");
  dmanagers.transfer (ds3, 7, "c2", "c4");
  dmanagers.transfer (ds3, 5, "c1", "c2");
  dmanagers.transfer (ds3, 11, "c1", "c5");
  dmanagers.transfer (ds3, 8, "c5", "c2");
  dmanagers.revert_last_action (ds3);
  dmanagers.transfer (ds3, 8, "c5", "c2");

	std::cerr << "*** Test for reverting simplification rule 4 ***" << std::endl;
	qdft::dname_type ds4 = dmanagers.new_data (50, qdft::last, "ds4");
  dmanagers.init (ds4, 50, "c1");
  dmanagers.transfer (ds4, 10, "c1", "c2");
  dmanagers.truncate (ds4, "c1", 10);
  dmanagers.revert_last_action (ds4);
  dmanagers.transfer (ds4, 10, "c1", "c2");

	std::cerr << "*** Test for reverting simplification rule 5 ***" << std::endl;
	qdft::dname_type ds5 = dmanagers.new_data (50, qdft::last, "ds5");
  dmanagers.init (ds5, 50, "c1");
  dmanagers.transfer (ds5, 10, "c1", "c2");
  dmanagers.transfer (ds5, 10, "c2", "c3");
  dmanagers.transfer (ds5, 10, "c1", "c2");
  dmanagers.revert_last_action (ds5);
  dmanagers.transfer (ds5, 10, "c1", "c2");

	std::cerr << "*** Test for reverting several simplification rules in a row ***" << std::endl;
	qdft::dname_type dm1 = dmanagers.new_data (50, qdft::last, "dm1");
  dmanagers.init (dm1, 50, "c1");
  dmanagers.transfer (dm1, 10, "c1", "c2");
  dmanagers.transfer (dm1, 10, "c1", "c3");
  dmanagers.transfer (dm1, 10, "c2", "c4");
  dmanagers.transfer (dm1, 10, "c3", "c4");
  dmanagers.truncate (dm1, "c1", 9);
  dmanagers.truncate (dm1, "c2", 9);
  dmanagers.truncate (dm1, "c3", 9);
  dmanagers.truncate (dm1, "c4", 0);
  dmanagers.revert_last_action (dm1);
  dmanagers.truncate (dm1, "c4", 0);

	std::cerr << "*** Test for transferring again after reverting simplification rule 2 ***" << std::endl;
	qdft::dname_type dt2 = dmanagers.new_data (50, qdft::last, "dt2");
  dmanagers.init (dt2, 50, "c1");
  dmanagers.transfer (dt2, 10, "c1", "c2");
  dmanagers.transfer (dt2, 5, "c2", "c3");
  dmanagers.revert_last_action (dt2);
  dmanagers.transfer (dt2, 5, "c2", "c3");

	std::cerr << "*** Test for Enrico ***" << std::endl;
	qdft::dname_type en1 = dmanagers.new_data (50, qdft::last, "en1");
  dmanagers.init (en1, 50, "c1");
  dmanagers.transfer (en1, 20, "c1", "c2");
  dmanagers.transfer (en1, 20, "c1", "c3");
  // dmanagers.revert_last_action (en1);
  dmanagers.transfer (en1, 10, "c2", "c3");
  dmanagers.transfer (en1, 10, "c3", "c2");

	// std::cerr << "*** Test for DAG issue ***" << std::endl;
	// qdft::dname_type en2 = dmanagers.new_data (35000, qdft::last, "en2");
  // dmanagers.init (en2, 5000, "s1");
  // dmanagers.init (en2, 5000, "s2");
  // dmanagers.init (en2, 5000, "s3");
  // dmanagers.init (en2, 5000, "s4");
  // dmanagers.init (en2, 5000, "s5");
  // dmanagers.init (en2, 5000, "s6");
  // dmanagers.init (en2, 5000, "s7");
	// block_transfer (dmanagers, en2, "c8", "c3", 5145, 200);
	// block_transfer (dmanagers, en2, "c2", "c7", 848, 200);
	// block_transfer (dmanagers, en2, "s3", "c2", 2623, 200);
	// block_transfer (dmanagers, en2, "s3", "c6", 5487, 200);
	// block_transfer (dmanagers, en2, "s1", "c6", 5380, 200);
	// block_transfer (dmanagers, en2, "s5", "c8", 8830, 200);
	// block_transfer (dmanagers, en2, "c8", "c0", 5596, 200);
	// block_transfer (dmanagers, en2, "c4", "c0", 1569, 200);
	// block_transfer (dmanagers, en2, "s2", "c7", 8104, 200);
	// std::cerr << "Should apply rule 5" << std::endl;
	// block_transfer (dmanagers, en2, "s5", "c8", 6602, 200);
  // // dmanagers.transfer (en2, 200, "s5", "tmp");
  // // dmanagers.transfer (en2, 200, "tmp", "c8");
  // // dmanagers.transfer (en2, 200, "s5", "tmp");
  // // dmanagers.transfer (en2, 200, "tmp", "c8");
	// // dmanagers.truncate (en2, "tmp", 0);
	// std::cerr << "SO FAR, SO GOOD" << std::endl;
	// block_transfer (dmanagers, en2, "c0", "c8", 8939, 200);
  // // dmanagers.transfer (en2, 200, "c0", "tmp");
  // // dmanagers.transfer (en2, 200, "tmp", "c8");
	// block_transfer (dmanagers, en2, "c0", "c3", 2543, 200);
	// block_transfer (dmanagers, en2, "c5", "c7", 4869, 200);
	// block_transfer (dmanagers, en2, "s1", "c7", 2503, 200);
	// block_transfer (dmanagers, en2, "s5", "c5", 5685, 200);
	// block_transfer (dmanagers, en2, "c5", "c8", 8176, 200);

	std::cerr << "=== Test for multiple transfer issue ===" << std::endl;
	std::cerr << "========================================" << std::endl;
	qdft::dname_type db1 = dmanagers.new_data (500, qdft::last, "db1");
  dmanagers.init (db1, 50, "c1");
  dmanagers.transfer (db1, 10, "c1", "c2");
  dmanagers.transfer (db1, 4, "c1", "c2");
	assert (dmanagers.get_quantity (db1, "c2") == 14);
	dmanagers.show_graphs ();	
}

#endif // ! QDFT_EXAMPLE_CC
