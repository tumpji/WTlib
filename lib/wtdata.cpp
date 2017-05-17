/*
 *  This file is part of WTlib.
 *
 *  WTlib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  WTlib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with WTlib.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "wt.h"

namespace WTlib {

WTdata::WTdata 
(int rozmer_x, int rozmer_y, Allocation_options alloc): 
	rozmer_x(rozmer_x), rozmer_y(rozmer_y) 
{
	assert(rozmer_x > 0);
	assert(rozmer_y > 0);
	if ( alloc == Allocation_options::ALLOCATE )
		this->data.resize(rozmer_x*rozmer_y, 0);
}

WTdata::WTdata 
( std::string oznaceni, int rozmer_x, int rozmer_y, Allocation_options alloc )
	: WTdata(rozmer_x, rozmer_y, alloc )
{
	this->oznaceni = oznaceni;
}

} // end namespace WTlib
