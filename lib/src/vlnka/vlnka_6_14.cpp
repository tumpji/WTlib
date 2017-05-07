#include "vlnka_internal.h"

namespace V_6_14 {

void Vlnka::lifting_predict( ROZHRANI_VLNKOVE_FUNKCE_PLNE ) {
	assert( (stop-start)%step == 0 );
	assert( (act-start )%step == 0 );
	assert(index_step < 2);

	int add = 1 << ( predict_left_shift(index_step) - 1 );

	switch ( index_step ) {
		case 0:
			for ( int i = act;   i <= stop;   i += step * 2) {
				int value = Ui(0);

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
				int value = - Ui(2) + Ui(-2) - 6*( -Ui(1) + Ui(-1) );
				value -= add;

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
				int value = Di(-1) - Di(1) + 8*Di(0);
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
			// DONE
			break;
		default:
			assert(false);
			break;
	}

}

} 
