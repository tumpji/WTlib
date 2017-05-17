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
#include <cstdint>

#include "aritmeticky.h"
#include "model_sign.h"
#include "model_256e.h"


class BitPlane {
	BitPlane (int rozmer_x, int rozmer_y) : rozmer_x(rozmer_x), rozmer_y(rozmer_y) {
		data.resize( rozmer_x*rozmer_y, false);
		suma_sloupcu.resize( rozmer_x, 0 );
		suma_radku.resize( rozmer_y, 0 );
	}	

	void spocitat_sumy ( ) {
		unsigned index = 0;
		for ( unsigned y = 0; y < rozmer_y; ++y )
		for ( unsigned x = 0; x < rozmer_x; ++x )
		{

			index += 1;
		}
	}

private:
	std::vector<bool> data;
	std::vector<unsigned> suma_sloupcu, suma_radku; 
};

void zjistit_


class Pruchod {
	Pruchod ( 

};
