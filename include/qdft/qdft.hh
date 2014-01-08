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
#ifndef QDFT_QDFT_HH
# define QDFT_QDFT_HH
# include <boost/shared_ptr.hpp>
# include <boost/unordered_map.hpp>
# include <boost/uuid/uuid.hpp>
# include <boost/uuid/uuid_generators.hpp>
# include <boost/uuid/uuid_io.hpp>
# include <qdft/types.hh>
# include <qdft/manager.hh>

namespace qdft {

	/**
	 * @file   qdft.hh
	 * @author Johan Oudinet <oudinet@kit.edu>
	 * @date   Thu Nov 10 12:00:56 2011
	 * 
	 * @brief  This file contains the main interface for the QDFTgraph library
	 * 
	 * 
	 */

  template <class Graph = qdft_graph>
  class data_managers {
		typedef qdft::data_manager<Graph>						data_manager_t;
		typedef boost::shared_ptr< data_manager_t >	data_manager_ptr;
	public:
		typedef boost::unordered_map<
			dname_type, data_manager_ptr>							data_managers_t;
    typedef boost::graph_traits<Graph> Traits;
    typedef typename Traits::vertices_size_type	vertices_size_type;
    typedef typename Traits::edges_size_type	edges_size_type;

		/** 
		 * Create a new data of size q (and set mode m for further updates)
		 * 
  	 * @param q the amount of data
		 * @param m the mode for further updates (dyn or static)
		 * @param dname a unique name to identify the new data
		 * 
		 * @return a unique name to identify the newly created data
		 */
		dname_type new_data (const quantity_type& q, const mode_type& m,
												 const dname_type& dname =
												 boost::lexical_cast<dname_type> (boost::uuids::random_generator()()));

		/** 
		* Set the amount of data d
		* 
		* @param d the data name
		* @param q the actual amount of data
		*/
    void set_data_amount (const dname_type& d, const quantity_type& q);

		/** 
		* Initialize the container c with qi amount of d
		* 
		* @param d the data name
		* @param qi the initial amount of data in c
		* @param c the container's name
		*/
    void init (const dname_type& d,
							 const quantity_type& qi, const cname_type& c);

		/** 
		 * Transfer q amount of data d from c_src to c_dst
		 * 
 		 * @param d the data name

		 * @param qa the transfered size that may be greater than q
		 * @param c_src source container of the data transfer (ignore the
		 * transfer if c_src does not exist)
		 * @param c_dst destination container of the data transfer (might be new)
		 * 
		 * @return the old capacity of the edge c_src -> c_dst
		 */
    quantity_type
    transfer (const dname_type& d,
							const quantity_type& qa,
							const cname_type& c_src,
							const cname_type& c_dst);

		/** 
		 * Truncate the amount of data d in c to n
		 * if n is greater than the amount of data in c
		 * or c does not exist just do nothing
		 * 
 		 * @param d the data name
		 * @param c container's name
		 * @param n new size of c
		 */
    void
    truncate (const dname_type& d, const cname_type& c, const quantity_type& n);

		/** 
		 * Set the transfered edge value to a specific quantity. This is
		 * usefull to revert a previous transfer.
		 * 
 		 * @param d the data name
		 * @param q amount of data to set as the edge quantity
		 * @param c_src source container
		 * @param c_dst destination container
		 */
    void
    set_edge_quantity (const dname_type& d,
											 const quantity_type& q,
											 const cname_type& c_src,
											 const cname_type& c_dst);

		/** 
		 * Revert the effect of the last action to the graph.
		 * 
 		 * @param d the data name
		 */
    void
    revert_last_action (const dname_type& d);

		/** 
		 * Get amount of data d in c
		 * 
 		 * @param d the data name
		 * @param c container's name
		 * 
		 * @return the amount of data d in c (0 if c is not in the graph)
		 */
    quantity_type
    get_quantity (const dname_type& d, const cname_type& c) const;

		/** 
		 * Get the total amount of data d
		 * 
 		 * @param d the data name
		 * 
		 * @return the amount of data d
		 */
    quantity_type
    get_data_amount (const dname_type& d) const;

		/** 
		 * Print out each data graph in GraphViz format
		 * 
		 * @param out the ouput stream (default is std::out)
		 */
    void show_graphs (std::ostream& out = std::cout) const;

		/** 
		 * Save graphs in fname file for pretty printing them
		 * 
		 * @param fname file name
		 */
    void save (const std::string& fname) const;

		/** 
		 * Get the total number of nodes and edges for all graphs
		 */
    std::pair<vertices_size_type, edges_size_type> get_graphs_size () const;

	private:
		data_managers_t dm_;
	};
} // end namespace qdft

# include "qdft.hxx"

#endif // ! QDFT_QDFT_HH
