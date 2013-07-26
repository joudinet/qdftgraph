# Copyright (C) 2011, 2012 Johan Oudinet <oudinet@cs.tum.edu>
#  
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#  
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#  
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#  

AM_CPPFLAGS = -I$(srcdir)/../include $(BOOST_CPPFLAGS)

lib_LTLIBRARIES = libqdftgraph.la

libqdftgraph_la_SOURCES =												\
qdft/action.hh																	\
qdft/graph.hh																		\
qdft/manager.hh																	\
qdft/manager.hxx																\
qdft/qdft.hh																		\
qdft/qdft.hxx																		\
qdft/types.hh																		\
qdft/qdft_wrap.cxx

libqdftgraph_la_LDFLAGS = -no-undefined

nobase_include_HEADERS =												\
qdft/qdft.i																			\
qdft/libqdftgraph.cs														\
qdft/libqdftgraphPINVOKE.cs											\
qdft/dataManagers.cs														\
qdft/mode_type.cs																\
qdft/SWIGTYPE_p_std__ostream.cs
