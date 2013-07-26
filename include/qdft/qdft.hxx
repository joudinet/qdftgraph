// Copyright (C) 2011, 2012 Johan Oudinet <oudinet@kit.edu>
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
#ifndef QDFT_QDFT_HXX
# define QDFT_QDFT_HXX
# include <stdexcept>
# include <iostream>
# include <fstream>
# include <boost/foreach.hpp>
# include "qdft.hh"

namespace qdft {

	// Create a new data of size q (and set mode m for further updates)
	// Make use of UUID library
  template <class Graph>
	dname_type
	data_managers<Graph>::new_data (const quantity_type& q, const mode_type& m,
																	const dname_type& dname)
	{
		typename data_managers_t::iterator it = dm_.find (dname);
		if (it != dm_.end ()) // dname must be new
			{
				throw std::invalid_argument ("data_managers::new_data: dname must be new");
			}
		data_manager_ptr pdm (new data_manager_t (q, m, dname));
		dm_[dname] = pdm;
		return dname;
	}

	// Set the amount of data d
  template <class Graph>
	void
	data_managers<Graph>::set_data_amount (const dname_type& d,
																				 const quantity_type& q)
	{
		typename data_managers_t::iterator it = dm_.find (d);
		if (it == dm_.end ()) // New data
			{
				// d must exist
				throw std::invalid_argument ("data_managers::set_data_amount: d must exist");
			}
		else // Update data
			{
				it->second->set_data_amount (q);
			}
	}

	// Initialize the container c with qi amount of d
  template <class Graph>
	void
	data_managers<Graph>::init (const dname_type& d,
															const quantity_type& qi, const cname_type& c)
	{
		typename data_managers_t::iterator it = dm_.find (d);
		if (it == dm_.end ()) // New data
			{
				// d must exist
				throw std::invalid_argument ("data_managers::init: d must exist");
			}
		else
			{
				it->second->init (qi, c);
			}
	}

	// Transfer q amount of data d from c_src to c_dst
  template <class Graph>
	quantity_type
	data_managers<Graph>::transfer (const dname_type& d,
																	const quantity_type& qa,
																	const cname_type& c_src,
																	const cname_type& c_dst)
	{
		typename data_managers_t::iterator it = dm_.find (d);
		if (it == dm_.end ())
			{
				// d must exist
				throw std::invalid_argument ("data_managers::transfer: d must exist");
			}
		return it->second->transfer (qa, c_src, c_dst);
	}

	// truncate amount of data d in c to n
  template <class Graph>
	void
	data_managers<Graph>::truncate (const dname_type& d,
																	const cname_type& c,
																	const quantity_type& n)
	{
		typename data_managers_t::iterator it = dm_.find (d);
		if (it == dm_.end ())
			{
				// d must exist
				throw std::invalid_argument ("data_managers::transfer: d must exist");
			}
		it->second->truncate (c, n);		
	}

	// // Remove q amount of data d from c
  // template <class Graph>
	// void
	// data_managers<Graph>::remove (const dname_type& d,
	// 															const quantity_type& q,
	// 															const cname_type& c)
	// {
	// 	typename data_managers_t::iterator it = dm_.find (d);
	// 	if (it == dm_.end ())
	// 		{
	// 			// d must exist
	// 			throw std::invalid_argument ("data_managers::transfer: d must exist");
	// 		}
	// 	it->second->remove (q, c);		
	// }

	// Set the transfered edge value to a specific quantity. This is
  template <class Graph>
	void
	data_managers<Graph>::set_edge_quantity (const dname_type& d,
																					 const quantity_type& q,
																					 const cname_type& c_src,
																					 const cname_type& c_dst)
	{
		typename data_managers_t::iterator it = dm_.find (d);
		if (it == dm_.end ())
			{
				// d must exist
				throw std::invalid_argument ("data_managers::transfer: d must exist");
			}
		it->second->set_edge_quantity (q, c_src, c_dst);
	}

	// Revert the effect of the last action to the graph.
  template <class Graph>
	void
	data_managers<Graph>::revert_last_action (const dname_type& d)
	{
		typename data_managers_t::iterator it = dm_.find (d);
		if (it == dm_.end ())
			{
				// d must exist
				throw std::invalid_argument ("data_managers::transfer: d must exist");
			}
		it->second->revert_last_action ();
	}

	// Get amount of data d in c
  template <class Graph>
	quantity_type
	data_managers<Graph>::get_quantity (const dname_type& d,
																			const cname_type& c) const
	{
		typename data_managers_t::const_iterator it = dm_.find (d);
		if (it == dm_.end ())
			{
				// d must exist
				throw std::invalid_argument ("data_managers::transfer: d must exist");
			}
		return it->second->get_quantity (c);
	}

	// Get the total amount of data d
  template <class Graph>
	quantity_type
	data_managers<Graph>::get_data_amount (const dname_type& d) const
	{
		typename data_managers_t::const_iterator it = dm_.find (d);
		if (it == dm_.end ())
			{
				// d must exist
				throw std::invalid_argument ("data_managers::transfer: d must exist");
			}
		return it->second->get_data_amount ();
	}


	// Print out each data graph in GraphViz format
  template <class Graph>
	void
	data_managers<Graph>::show_graphs (std::ostream& out) const
	{
		BOOST_FOREACH(typename data_managers_t::value_type i, dm_) {
			i.second->show_graph (out);
		}
	}

	// Save graphs in fname file for pretty printing them	
  template <class Graph>
	void
	data_managers<Graph>::save (const std::string& fname) const
	{
		std::fstream fs;
		fs.open (fname.c_str (), std::fstream::out);
		show_graphs (fs);
		fs.close ();
	}

} // end namespace qdft

#endif // ! QDFT_QDFT_HXX
