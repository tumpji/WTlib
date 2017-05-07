#include "vlnka_internal.h"

namespace LINEAR {


namespace v3 {
	void Vlnka::lifting_predict( ROZHRANI_VLNKOVE_FUNKCE_PLNE ) {
		assert( (stop-start)%step == 0 );
		assert( (act-start )%step == 0 );
		assert(index_step < 2);

		for ( int i = act;   i <= stop;   i += step * 2) {
			int value = ROZSIRENI(i-step) + ROZSIRENI(i+step);
			if ( divide_and_round )
				if ( reversed )
					data[i] += value >> predict_left_shift(0);
				else 
					data[i] -= value >> predict_left_shift(0);
			else 
				data[i] += value;
		}
	}
}
namespace v3p {
	void Vlnka::lifting_predict( ROZHRANI_VLNKOVE_FUNKCE_PLNE ) {
		assert( (stop-start)%step == 0 );
		assert( (act-start )%step == 0 );
		assert(index_step < 2);

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
}
namespace v7 {
	void Vlnka::lifting_predict( ROZHRANI_VLNKOVE_FUNKCE_PLNE ) {
		assert( (stop-start)%step == 0 );
		assert( (act-start )%step == 0 );
		assert(index_step < 2);

		int add = 1 << ( predict_left_shift(0) - 1 );

		for ( int i = act;   i <= stop;   i += step * 2) {
			int value = -(Ui(2) + Ui(-1)) + 9*( Ui(1) + Ui(0) );
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
}

namespace v7m {
	void Vlnka::lifting_predict( ROZHRANI_VLNKOVE_FUNKCE_PLNE ) {
		assert( (stop-start)%step == 0 );
		assert( (act-start )%step == 0 );
		assert(index_step < 2);

		int add = 1 << ( predict_left_shift(0) - 1 );

		for ( int i = act;   i <= stop;   i += step * 2) {
			int value = -(Ui(2) + Ui(-1)) + 9*( Ui(1) + Ui(0) );
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
}


} // end namespace LINEAR
