#include "vlnka_internal.h"

namespace CDF_9_7_M {

void Vlnka::lifting_predict( ROZHRANI_VLNKOVE_FUNKCE_PLNE ) {
	assert( (stop-start)%step == 0 );
	assert( (act-start )%step == 0 );
	assert(index_step < 2);

	int add = 1 << ( predict_left_shift(0) - 1 );

	for ( int i = act;   i <= stop;   i += step * 2) {
		int value = -1*( ROZSIRENI(i+3*step) + ROZSIRENI(i-3*step) ) + 
					 9*( ROZSIRENI(i+step) + ROZSIRENI(i-step) );
		value -= add;

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
	assert( (stop-start)%step == 0 );
	assert( (act-start )%step == 0 );
	assert(index_step < 2);

	int add = 1 << ( update_left_shift(0) - 1 );

	for ( int i = act;   i <= stop;   i += step * 2) {
		int value = ROZSIRENI(i+step) + ROZSIRENI(i-step);
		value += add;

		if ( divide_and_round )
			if ( reversed )
				data[i] -= value >> update_left_shift(0);
			else 
				data[i] += value >> update_left_shift(0);
		else 
			data[i] += value;
	}
}


}
