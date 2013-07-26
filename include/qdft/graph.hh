// Copyright (C) 2011 Johan Oudinet <oudinet@lri.fr>
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
#ifndef QDFT_GRAPH_HH
# define QDFT_GRAPH_HH
# include <boost/graph/adjacency_list.hpp>
# include <qdft/types.hh>

namespace qdft {

  namespace detail {
    // node type
    struct node {
      node (const size_t& i = 0,
						const quantity_type& q = quantity_type (),
						const cname_type& n = cname_type (),
						const bool& f = true)
				: id(i), name(n), amount(q), fake(f) {}
      size_t		id;
      quantity_type	amount;
      cname_type	name;
      bool		fake;
    };

    // transition type
    struct transition {
      transition (const quantity_type& qi = 0): transfered(qi), residual(0) {}
      quantity_type	transfered;
      quantity_type	residual;
    };
  } // end namespace detail

  typedef boost::adjacency_list<
    boost::vecS,
    boost::listS,
    boost::bidirectionalS,
    detail::node,
    detail::transition>	qdft_graph;

} // end namespace qdft
#endif // ! QDFT_GRAPH_HH
