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

