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
#ifndef WT_BAREVNA_TRANSFORMACE_INCLUDED
#define WT_BAREVNA_TRANSFORMACE_INCLUDED

#include <vector>
#include <memory>

#include "main.h"
#include "wt.h"

// provede barevnou transformaci 
// samo si hleda smer
void provest_barevnou_transformaci (
		SMER,
		WTlib::BAREVNY_PROSTOR,
		std::vector<Shared_data>& );


#endif // WT_BAREVNA_TRANSFORMACE_INCLUDED
