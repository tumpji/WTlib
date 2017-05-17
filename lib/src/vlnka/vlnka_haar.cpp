#include "vlnka_internal.h"

namespace HAAR {

void Vlnka::lifting_predict( ROZHRANI_VLNKOVE_FUNKCE_PLNE ) {
	assert( (stop-start)%step == 0 );
	assert( (act-start )%step == 0 );
	assert(index_step < 2);

	for ( int i = act;   i <= stop;   i += step * 2) {
		int value = ROZSIRENI(i-step);
		if ( divide_and_round )
			if ( reversed )
				data[i] += value;
			else 
				data[i] -= value;
		else 
			data[i] += value;
	}
}

void Vlnka::lifting_update( ROZHRANI_VLNKOVE_FUNKCE_PLNE ) {
	assert( (stop-start)%step == 0 );
	assert( (act-start )%step == 0 );
	assert(index_step < 2);

	for ( int i = act;   i <= stop;   i += step * 2) {
		int value = ROZSIRENI(i+step);
		value -= 1;

		if ( divide_and_round )
			if ( reversed )
				data[i] -= value >> 1;
			else 
				data[i] += value >> 1;
		else 
			data[i] += value;
	}
}


}
