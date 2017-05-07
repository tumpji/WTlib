#include "avrg_to_zero.h"


int provest_pasmo_avrg_to_zero ( Shared_data data ) {
	int64_t suma_data = 0;
	int64_t pocet = 0;
	for ( const auto & i : data->data ) {
		suma_data += i;
		pocet += 1;
	}

	int shift = static_cast<int>( suma_data / pocet );

	for ( auto & i : data->data ) {
		i -= shift;
	}
	return shift;
}

void opravit_pasmo_avrg_to_zero ( Shared_data data, int shift ) {
	if ( shift == 0 ) return;
	for ( auto & i : data->data ) {
		i += shift;
	}
}

