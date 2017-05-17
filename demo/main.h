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
#ifndef MAIN_HADER_INCLUDED
#define MAIN_HADER_INCLUDED

#include <string>
#include <vector>

#include "wt.h"     // moje knihovna

typedef struct Parse_opts {
	// dekoder => true | koder => false
	bool				decoder_switch;
	// it is test
	bool				test_switch;
	// only opens the picture and save it as a png to different file 
	bool				opensave_switch;
	// affter compression it tries to open encrypted file and check if content match
	bool				check_switch;

	// method used in compression
	WTlib::WTcommon		metoda;

} Parse_opts;

typedef std::vector<WTlib::WTdata> Obrazek;





#endif // MAIN_HADER_INCLUDED
