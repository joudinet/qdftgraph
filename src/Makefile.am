# Copyright (C) 2011, 2013 Johan Oudinet <oudinet@kit.edu>
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

bin_PROGRAMS = example testlib wintraceparser

example_SOURCES = example.cc
example_LDADD = ../include/libqdftgraph.la

testlib_SOURCES = test_lib.cc
testlib_LDADD = ../include/libqdftgraph.la

wintraceparser_SOURCES = wintraceparser.cc
wintraceparser_LDADD = ../include/libqdftgraph.la
