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
#ifndef QDFT_TYPES_HH
# define QDFT_TYPES_HH
# include <string>

namespace qdft {

  typedef size_t					quantity_type; // value type for amount of data
  typedef std::string			cname_type; // Container's name type
  typedef std::string			dname_type; // Data's name type

	enum mode_type {unknown, max, last}; // Mode to update the amount of data

} // end namespace qdft
#endif // ! QDFT_TYPES_HH
