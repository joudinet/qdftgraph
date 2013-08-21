// Copyright (C) 2011, 2012, 2013 Johan Oudinet <oudinet@cs.tum.edu>
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
#ifndef QDFT_MANAGER_HH
# define QDFT_MANAGER_HH
# include <iostream>
# include <limits>
# include <stack>
# include <boost/unordered_map.hpp>
# include <boost/variant.hpp>
# include <qdft/graph.hh>
# include <qdft/action.hh>

namespace qdft {
  namespace detail {
    class simple_counter {
    public:
      simple_counter(): n_(0) {}
      size_t operator()() { assert(n_ != std::numeric_limits<size_t>::max()); return n_++; }
      void reset() { n_ = 0; }
    private:
      size_t n_;
    };
  } // end namespace detail

  template <class Graph = qdft_graph>
  class data_manager {
  public:
    typedef boost::graph_traits<Graph> Traits;
    typedef typename Traits::vertex_descriptor vertex_descriptor;
    typedef typename Traits::edge_descriptor edge_descriptor;
    typedef typename Traits::in_edge_iterator in_edge_iterator;
    typedef typename Traits::out_edge_iterator out_edge_iterator;
    typedef boost::unordered_map<cname_type, vertex_descriptor> c2v_type;

		/** 
		 * Create a new data manager with q amount of data
		 * 
		 * @param q the amount of data
		 * @param m the mode for further updates (dyn or static)
		 */
    data_manager (const quantity_type& q = 0,
									const mode_type& m = unknown,
									const dname_type& d = "");

		/** 
		* Set the amount of data
		* 
		* @param q the actual amount of data
		*/
    void set_data_amount (const quantity_type& q);

		/** 
		* Initialize the container c with qi amount of data
		* 
		* @param qi the initial amount of data
		* @param c the container's name
		*/
    void init (const quantity_type& qi, const cname_type& c);

		/** 
		 * Transfer q amount of data from c_src to c_dst
		 * 
		 * @param qa the transfered size that may be greater than q
		 * @param c_src source container of the data transfer (ignore the
		 * transfer if c_src does not exist)
		 * @param c_dst destination container of the data transfer (might
		 * be new but must be different from c_src)
		 * 
		 * @return the old capacity of the edge c_src -> c_dst
		 */
    quantity_type
    transfer (const quantity_type& qa,
							const cname_type& c_src,
							const cname_type& c_dst);

		/** 
		 * Truncate the amount of data in c to n
		 * if n is greater than the amount of data
		 * or c does not exist just do nothing
		 * 
		 * @param c container's name
		 * @param n new size of c
		 */
    void
    truncate (const cname_type& c, const quantity_type& n);

		/** 
		 * Set the transfered edge value to a specific quantity. This is
		 * usefull to revert a previous transfer.
		 * 
		 * @param q amount of data to set as the edge quantity
		 * @param c_src source container
		 * @param c_dst destination container
		 */
    void
    set_edge_quantity (const quantity_type& q,
											 const cname_type& c_src,
											 const cname_type& c_dst);

		/** 
		 * Revert the effect of the last action to the graph.
		 */
    void
    revert_last_action ();

		/** 
		 * Get amount of data in c
		 * 
		 * @param c container's name
		 * 
		 * @return the amount of data in c (0 if c is not in the graph)
		 */
    quantity_type
    get_quantity (const cname_type& c);  //const; // lazy evaluation may modify the graph here

		/** 
		 * Get the total amount of data
		 * 
		 * @return the total amount of data
		 */
    quantity_type
    get_data_amount () const;

		/** 
		 * Print out the graph in GraphViz format
		 * 
		 * @param out the ouput stream (default is std::out)
		 */
    void show_graph (std::ostream& out = std::cout) const;

  private:
    mutable Graph		g_;
    c2v_type			c2v_; // Mapping from container's name to vertex descriptor
    detail::simple_counter	next_id_; // Used to maintain the vertex indexes
		vertex_descriptor src_;			// source node for max flow algorithm
		vertex_descriptor current_src_; // change when updating the amount of data
		mode_type m_;								// Mode for updating the amount of data
		typedef boost::variant<
			action::init,
			action::transfer,
			action::truncate,
			action::simplify1,
			action::simplify2,
			action::simplify3,
			action::simplify4,
			action::simplify5
			> action_type;
		std::stack<action_type > last_actions_;

		/** 
		 * Remove vertex u from the internal graph and update the vertex indexes
		 * 
		 * @param u the vertex to remove
		 */
		void remove_vertex_ (vertex_descriptor u);

		/** 
		 * Apply simplfification rules on vertex u and continues recursively
		 * 
		 * @param u the vertex to remove
		 */
		void apply_simplification_rules_ (vertex_descriptor u);

		/** 
		 * Update the amount of data in u
		 * 
		 * @param u the vertex to update
		 */
		void update_data_of_ (vertex_descriptor u);

  };
} // end namespace qdft

# include "manager.hxx"

#endif // ! QDFT_MANAGER_HH
