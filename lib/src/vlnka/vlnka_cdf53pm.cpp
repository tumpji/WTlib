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
#include "vlnka_internal.h"

namespace CDF_5_3pm {

void Vlnka::lifting_predict( ROZHRANI_VLNKOVE_FUNKCE_PLNE ) {
	(void)index_step;
	assert( (stop-start)%step == 0 );
	assert( (act-start )%step == 0 );
	assert(index_step < 1);

	int add = 1 << ( predict_left_shift(0) - 1 );

	for ( int i = act;   i <= stop;   i += step * 2) {
		int value = ROZSIRENI(i-step) + ROZSIRENI(i+step);
		value += add;
				
		if ( divide_and_round )
			if ( reversed )
				data[i] += value >> predict_left_shift(0);
			else 
				data[i] -= value >> predict_left_shift(0);
		else 
			data[i] += value;
	}
}

void Vlnka::lifting_update( ROZHRANI_VLNKOVE_FUNKCE_PLNE ) {
	(void)index_step;
	assert( (stop-start)%step == 0 );
	assert( (act-start )%step == 0 );
	assert(index_step < 1);

	int add = 1 << ( update_left_shift(0) - 1 );

	for ( int i = act;   i <= stop;   i += step * 2) {
		int value = ROZSIRENI(i-step) + ROZSIRENI(i+step);
		value -= add;

		if ( divide_and_round )
			if ( reversed )
				data[i] -= value >> update_left_shift(0);
			else 
				data[i] += value >> update_left_shift(0);
		else 
			data[i] += value;
	}
}


} // end namespace CDF_5_3

