#include "vlnka_internal.h"

namespace CDF_9_7_F {


void Vlnka::lifting_predict( ROZHRANI_VLNKOVE_FUNKCE_PLNE ) {
	assert( (stop-start)%step == 0 );
	assert( (act-start )%step == 0 );
	assert(index_step < 2);


	int add = 1 << ( predict_left_shift(index_step) - 1 );

	switch ( index_step ) {
		case 0:
			for ( int i = act;   i <= stop;   i += step * 2) {
				int value = 203*(ROZSIRENI(i-step) + ROZSIRENI(i+step));
				value += add;

				if ( divide_and_round )
					if ( reversed )
						data[i] += value >> predict_left_shift(0);
					else 
						data[i] -= value >> predict_left_shift(0);
				else 
					data[i] += value;
			}
			break;
		case 1:
			for ( int i = act;   i <= stop;   i += step * 2) {
				int value = -113*(ROZSIRENI(i-step) + ROZSIRENI(i+step));
				value += -add;

				if ( divide_and_round )
					if ( reversed )
						data[i] += value >> predict_left_shift(1);
					else 
						data[i] -= value >> predict_left_shift(1);
				else 
					data[i] += value;
			}
			break;
		default:
			assert(false);
			break;
	}
}

void Vlnka::lifting_update( ROZHRANI_VLNKOVE_FUNKCE_PLNE ) {
	assert( (stop-start)%step == 0 );
	assert( (act-start )%step == 0 );
	assert(index_step < 2);

	int add = 1 << ( update_left_shift(index_step) - 1 );

	switch( index_step ) {
		case 0:
			for ( int i = act;   i <= stop;   i += step * 2) {
				int value = -217*(ROZSIRENI(i-step) + ROZSIRENI(i+step));
				value += add;

				if ( divide_and_round )
					if ( reversed )
						data[i] -= value >> update_left_shift(0);
					else 
						data[i] += value >> update_left_shift(0);
				else 
					data[i] += value;
			}
			break;
		case 1:
			for ( int i = act;   i <= stop;   i += step * 2) {
				int value = 1817*(ROZSIRENI(i-step) + ROZSIRENI(i+step));
				value += add;

				if ( divide_and_round )
					if ( reversed )
						data[i] -= (value + add) >> update_left_shift(1);
					else 
						data[i] += (value + add) >> update_left_shift(1);
				else 
					data[i] += value;
			}
			break;
		default:
			assert(false);
			break;
	}

}

} // end namespace CDF_9_7_F
