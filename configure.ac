# Copyright (C) KIT, Johan Oudinet <oudinet@kit.edu> - 2011
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
AC_PREREQ(2.61)
AC_INIT([qdftgraph], [0.3], [oudinet@kit.edu])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.10 -Wall -Werror tar-ustar no-define dist-bzip2 nostdinc])
LT_INIT([disable-static])

# Checks for programs.
AC_PROG_CXX

# Documentation
AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
	 then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi

# Attempt to use strict language conformance checking.
AC_CXX_FLAGS

# Use libtool
AC_PROG_LIBTOOL
AC_LIBTOOL_WIN32_DLL

# Test for BOOST graph library using Tsuna's boost.m4 macro file
BOOST_REQUIRE([1.47.0])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([doc/Doxyfile])])

AC_CONFIG_FILES([
	Makefile
	include/Makefile
	src/Makefile
	doc/Makefile
])

AC_OUTPUT
