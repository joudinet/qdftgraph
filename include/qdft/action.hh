// Copyright (C) 2012 Johan Oudinet <oudinet@cs.tum.edu>
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
#ifndef QDFT_ACTION_HH
# define QDFT_ACTION_HH
# include <boost/variant.hpp>
# include <boost/graph/adjacency_iterator.hpp>
# include <boost/graph/iteration_macros.hpp>
# include <qdft/types.hh>
# include <qdft/graph.hh>

namespace qdft {
	namespace action {
		struct init {};

		struct transfer
		{
			transfer (const cname_type& c1, const cname_type& c2): c_src(c1), c_dst(c2) {}
			cname_type c_src, c_dst;
		};

		struct truncate
		{
			truncate (const cname_type& u,
								const quantity_type& qold,
								const quantity_type& qnew):
				u_(u), qold_(qold), qnew_(qnew) {}
			cname_type u_;
			quantity_type qold_, qnew_;
		};

		struct simplify1
		{
			typedef std::pair<cname_type, quantity_type> fakeparent_type;
			typedef std::map<fakeparent_type, quantity_type> fakeparents_type;
			typedef std::vector<std::pair <cname_type, quantity_type> > realparents_type;
			simplify1 (const cname_type& ui,
								 const quantity_type& qi,
								 const realparents_type& real_parentsi,
								 const fakeparents_type& fake_parentsi):
				u(ui), q(qi), realparents(real_parentsi), fakeparents(fake_parentsi) {}
			cname_type u;
			quantity_type q;
			realparents_type realparents;
			fakeparents_type fakeparents;
		};

		struct simplify2
		{
			simplify2 (const cname_type& c1,
								 const cname_type& c2,
								 const cname_type& u,
								 const quantity_type& q,
								 const quantity_type& n1,
								 const quantity_type& n2,
								 const quantity_type& n3):
				src(c1), dst(c2), u_name(u), u_q(q), e1(n1), e2(n2), e3(n3) {}
			cname_type src, dst;
			cname_type u_name;
			quantity_type u_q;
			quantity_type e1, e2, e3;
		};

		struct simplify3
		{
			typedef std::vector< std::pair<cname_type, quantity_type> > parents_type;
			typedef std::vector< std::pair<quantity_type, quantity_type> > capacities_type;
			simplify3 (const cname_type& ci,
								 const cname_type& ui,
								 const quantity_type& qi,
								 const parents_type& parentsi,
								 const capacities_type& capacitiesi):
				c(ci), u(ui), q(qi), parents(parentsi), capacities(capacitiesi) {}
			cname_type c, u;
			quantity_type q;
			parents_type parents;
			capacities_type capacities;
		};

		struct simplify4
		{
			typedef std::pair<cname_type, quantity_type> fakechild_type;
			typedef std::map<fakechild_type,
											 std::pair<quantity_type, quantity_type> > fakechildren_type;
			typedef std::map<cname_type,
											 std::pair<quantity_type, quantity_type> > realchildren_type;
			typedef boost::graph_traits<qdft_graph>::vertex_descriptor vertex_descriptor;
			simplify4(vertex_descriptor s,
								const quantity_type& x,
								const cname_type& u,
								const quantity_type& q,
								const realchildren_type& realchildren,
								const fakechildren_type& fakechildren):
				s_(s), x_(x), u_(u), q_(q), realchildren_(realchildren), fakechildren_(fakechildren) {}
			vertex_descriptor s_;
			quantity_type x_;
			cname_type u_;
			quantity_type q_;
			realchildren_type realchildren_;
			fakechildren_type fakechildren_;
		};

		struct simplify5
		{
			typedef boost::graph_traits<qdft_graph>::vertex_descriptor vertex_descriptor;
			simplify5 (const vertex_descriptor& u2i,
								 const cname_type& ui,
								 const quantity_type& qi,
								 const quantity_type& n1,
								 const quantity_type& n2):
				u2(u2i), u_n(ui), u_q(qi), e1(n1), e2(n2) {}
			vertex_descriptor u2;
			cname_type u_n;
			quantity_type u_q;
			quantity_type e1, e2;
		};

		template <class Graph, class Map>
		class revert : public boost::static_visitor<void>
		{
			typedef boost::graph_traits<Graph> Traits;
			typedef typename Traits::vertex_descriptor vertex_descriptor;
			typedef typename Traits::edge_descriptor edge_descriptor;
			// typedef typename Traits::in_edge_iterator in_edge_iterator;
			// typedef typename boost::inv_adjacency_iterator_generator<Graph, vertex_descriptor, in_edge_iterator>::type inv_adjacency_iterator;
		public:
			revert (Graph& g, Map& m, vertex_descriptor src): g_(g), c2v_(m), src_(src) {}

			void operator() (const init& action) const
			{
				assert (false);
			}

			void operator() (const transfer& action) const
			{
				vertex_descriptor u, v, vold;
				bool vold_ok = false;
				std::cerr << "[QDFTgraph] Reverting last transfer from " << action.c_src
									<< " to " << action.c_dst << std::endl;
				typename Map::const_iterator it = c2v_.find (action.c_src);
				typename Map::iterator jt = c2v_.find (action.c_dst);
				assert (it != c2v_.end ());
				assert (jt != c2v_.end ());
				u = it->second;
				v = jt->second;
				// look for vold from v
				BGL_FORALL_INEDGES_T(v, e, g_, Graph)
					{
						if (g_[e].transfered > 0)
							{
								vold = source (e, g_);
								if (g_[vold].fake && (g_[vold].name == g_[v].name))
									{
										vold_ok = true;
										break;
									}
							}
					}
				if (vold_ok)
					{
						g_[vold].fake = false;
						jt->second = vold;
					}
				else
					{
						c2v_.erase(jt);
					}
				clear_vertex (v, g_);
				remove_vertex (v, g_);
			}

			void operator() (const truncate& action) const
			{
				std::cerr << "[QDFTgraph] Reverting last truncate of " << action.u_
									<< " (old=" << action.qold_ << ", new=" << action.qnew_ << ")"
									<< std::endl;
				if (action.qnew_ > 0)
					{
						vertex_descriptor u, uold;
						typename Map::iterator it = c2v_.find (action.u_);
						assert (it != c2v_.end ());
						u = it->second;
						// look for uold from u
						BGL_FORALL_INEDGES_T(u, e, g_, Graph)
							{
								if (g_[e].transfered == action.qnew_ && g_[u].name == g_[source (e, g_)].name)
									{
										uold = source (e, g_);
										g_[uold].fake = false;
										it->second = uold;
										break;
									}
							}
						clear_vertex (u, g_);
						remove_vertex (u, g_);
					}
				else
					{ // container has been deleted
						BGL_FORALL_VERTICES_T(u, g_, Graph)
							{
								if (g_[u].fake && (g_[u].name == action.u_) && (g_[u].amount == action.qold_))
									{
										g_[u].fake = false;
										c2v_[action.u_] = u;
										break;
									}
							}
					}
			}

			void operator() (const simplify1& action) const
			{
				// From s,s'
				// To s,s'-> u
				std::cerr << "[QDFTgraph] Reverting last simplify1 of " << action.u
									<< " (" << action.q << ")" << std::endl;
				vertex_descriptor s, u;
				u = add_vertex (detail::node (41, // must be updated afterward
																			action.q, action.u, true), g_);
				// Reconnect u to its (real) parents
				for (simplify1::realparents_type::const_iterator rp = action.realparents.begin ();
						 rp != action.realparents.end (); ++rp)
					{
						typename Map::const_iterator it = c2v_.find (rp->first);
						if (it != c2v_.end ())
							s = it->second;
						else
							{
								assert (rp->first == "src_");
								s = src_;
							}
						add_edge (s, u, detail::transition (rp->second), g_);
						add_edge (u, s, g_); // fake edge for max_flow algo
					}
				// Reconnect u to its (fake) parents
				if (!action.fakeparents.empty ())
					{
						// make a mutable copy of action.fakeparents
						simplify1::fakeparents_type fakeparents (action.fakeparents);
						BGL_FORALL_VERTICES_T(v, g_, Graph)
							{
								if (g_[v].fake)
									{
										simplify1::fakeparents_type::iterator fp
											= fakeparents.find (std::make_pair (g_[v].name,
																													g_[v].amount));
										if (fp != fakeparents.end ())
											{
												// v is one of the fakeparents of u
												add_edge (v, u, detail::transition (fp->second), g_);
												add_edge (u, v, g_); // fake edge for max_flow algo
												fakeparents.erase (fp);
											}
									}
							}
					}
			}

			void operator() (const simplify2& action) const
			{
				// from s --x-->t
				// to s --e1-> u --e2-> t (and may also have s --e3-> t)
				// s could be equal to t!
				vertex_descriptor s, t, u;
				bool s_ok = false;
				std::cerr << "[QDFTgraph] Reverting last simplify2 from " << action.src
									<< " to " << action.dst << std::endl;
				typename Map::const_iterator jt = c2v_.find (action.dst);
				assert (jt != c2v_.end ());
				t = jt->second;
				u = add_vertex (detail::node (42, // must be updated afterward
																			action.u_q, action.u_name, true), g_);
				// look for s from t
				BGL_FORALL_INEDGES_T(t, e, g_, Graph)
					{
						if (g_[e].transfered > 0)
							{
								s = source (e, g_);
								if (g_[s].name == action.src)
									{
										s_ok = true;
										break;
									}
							}
					}
				if (!s_ok)
					{
						assert(action.src == action.dst);
						assert(action.e3 == 0);
						s = t;
					}
				add_edge (s, u, detail::transition (action.e1), g_);
				add_edge (u, s, g_); // fake edge for max_flow algo
				add_edge (u, t, detail::transition (action.e2), g_);
				add_edge (t, u, g_); // fake edge for max_flow algo
				edge_descriptor e3, e3_back;
				bool ok;
				boost::tie(e3, ok) = edge (s, t, g_);
				assert (ok);
				boost::tie(e3_back, ok) = edge (t, s, g_);
				assert (ok);
				g_[e3].transfered = action.e3;
				if ((action.e3 == 0) && (g_[e3_back].transfered == 0))
					{
						remove_edge(e3, g_);
						remove_edge(e3_back, g_);
					}
			}

			void operator() (const simplify3& action) const
			{
				// From s,s' --> c
				// To s,s' --> u --> c
				std::cerr << "[QDFTgraph] Reverting rule3: ["
									<< action.c << ", " << action.u << ", "
									<< action.q;
				simplify3::parents_type::const_iterator
					it = action.parents.begin ();
				for (size_t i = 0; it != action.parents.end (); ++it, ++i)
					std::cerr << ", "  << "(" << it->first << ", " << it->second
										<< ", " << action.capacities[i].first
										<< ", " << action.capacities[i].second
										<< ")";
				std::cerr << "]" << std::endl;
				vertex_descriptor s, t, u;
				typename Map::const_iterator jt = c2v_.find (action.c);
				assert (jt != c2v_.end ());
				t = jt->second;
				u = add_vertex (detail::node (43, // must be updated afterward
																			action.q, action.u, true), g_);
				BGL_FORALL_INEDGES_T(t, e, g_, Graph)
					{
						if (g_[e].transfered > 0)
							{
								s = source (e, g_);
								// look for s in the parent list
								size_t i = 0;
								simplify3::parents_type::value_type
									v = std::make_pair (g_[s].name, g_[s].amount);
								simplify3::parents_type::const_iterator
									it = action.parents.begin ();
								for (; (it != action.parents.end()) && (*it != v); ++it)
									++i;
								if (it != action.parents.end ())
									{
										add_edge (s, u, detail::transition (action.capacities[i].first), g_);
										add_edge (u, s, g_); // fake edge for max_flow algo
										g_[e].transfered = action.capacities[i].second;
										// may remove e but this would invalidate the current edge iterator
									}
							}
					}
				add_edge (u, t, detail::transition (action.q), g_);
				add_edge (t, u, g_); // fake edge for max_flow algo
			}

			void operator() (const simplify4& action) const
			{
				std::cerr << "[QDFTgraph] Reverting rule4: ("
									<< g_[action.s_].name << ", " << action.x_ << ", "
									<< action.u_ << ", " << action.q_ << ", ...)"
									<< std::endl;
				vertex_descriptor s = action.s_;
				vertex_descriptor u =  add_vertex
					(detail::node (44, // must be updated afterward
												 action.q_, action.u_, true), g_);
				vertex_descriptor t;
				BGL_FORALL_OUTEDGES_T(s, e, g_, Graph)
					{
						if (g_[e].transfered > 0)
							{
								t = target (e, g_);
								quantity_type q1, q2;
								// look for t in the (fake/real) child list
								if (g_[t].fake)
									{
										simplify4::fakechildren_type::const_iterator it
											= action.fakechildren_.find
											(std::make_pair (g_[t].name, g_[t].amount));
										assert (it != action.fakechildren_.end ());
										boost::tie (q1, q2) = it->second;
									}
								else
									{
										simplify4::realchildren_type::const_iterator it
											= action.realchildren_.find (g_[t].name);
										assert (it != action.realchildren_.end ());
 										boost::tie (q1, q2) = it->second;
									}
								// Add link from u to t
								add_edge (u, t, detail::transition (q1), g_);
								add_edge (t, u, g_); // fake edge for max_flow algo
								g_[e].transfered = q2;
							}
					}
				add_edge (s, u, detail::transition (action.x_), g_);
				add_edge (u, s, g_); // fake edge for max_flow algo
			}

			void operator() (const simplify5& action) const
			{
				// from s --> u2 --b-> t
				// to s --e1-> u --b-> t (and s --e2-> u2)
				// \bug s could be equal to t!
				vertex_descriptor s, u, u2, t;
				quantity_type b = 0;
				u2 = action.u2;
				u =  add_vertex
					(detail::node (45, // must be updated afterward
												 action.u_q, action.u_n, true), g_);
				// assert (detail::real_out_degree (u2, g_) == 1);
				// assert (detail::real_in_degree (u2, g_) == 1);
				BGL_FORALL_OUTEDGES_T(u2, e, g_, Graph)
					{
						if (g_[e].transfered > 0)
							{
								t = target(e, g_);
								b = g_[e].transfered;
								break;
							}
					}
				BGL_FORALL_INEDGES_T(u2, e, g_, Graph)
					{
						if (g_[e].transfered > 0)
							{
								s = source(e, g_);
								break;
							}
					}
				add_edge (s, u, detail::transition (action.e1), g_);
				add_edge (u, s, g_); // fake edge for max_flow algo
				add_edge (u, u2, detail::transition (action.u_q), g_);
				add_edge (u2, u, g_); // fake edge for max_flow algo
				add_edge (u, t, detail::transition (b), g_);
				add_edge (t, u, g_); // fake edge for max_flow algo
				edge_descriptor e3, e3_back;
				bool ok;
				boost::tie(e3, ok) = edge (u2, t, g_);
				assert (ok);
				boost::tie(e3_back, ok) = edge (t, u2, g_);
				assert (ok && (g_[e3_back].transfered == 0));
				remove_edge(e3, g_);
				remove_edge(e3_back, g_);
			}
		private:
			Graph& g_;
			Map& c2v_;
			vertex_descriptor src_;
		};
	}
}

#endif // ! QDFT_ACTION_HH
