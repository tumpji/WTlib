/*
 *  This file is part of WTlib.
 *
 *  WTlib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  WTlib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with WTlib.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PICS_CONVERTER_HEADER_INCLUDED
#define PICS_CONVERTER_HEADER_INCLUDED

#include <vector>
#include <memory>

#include "wt.h"





// otevre obrazek v .png
std::vector<WTlib::WTdata> 
load_obrazek (std::string cesta);

// ulozi obrazek do .png
void 
save_obrazek (std::string cesta, std::vector<WTlib::WTdata> src);


// vraci false pro shodne data
bool 
check_obrazek( const Obrazek& , const Obrazek& );


#endif // PICS_CONVERTER_HEADER_INCLUDED
