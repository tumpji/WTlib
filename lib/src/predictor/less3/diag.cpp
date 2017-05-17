
#include <cassert>

#include "predictor_inter_all.h"
#include "std_macro.h"

namespace Predictor {
	void Diag::forward_predict(Shared_data data) {

		const auto rozmer_x = data->rozmer_x;
		const auto rozmer_y = data->rozmer_y;

		auto &raw_data = data->data;
		
		int index = rozmer_x*rozmer_y -1; // posledni validni

		for( int radek = rozmer_y - 1; radek > 0; --radek )
		{
			// normalni
			for ( int sloupec = rozmer_x - 1; sloupec > 0; --sloupec, --index ) {
				auto& A = raw_data[index-rozmer_x-1];
				int predikce = A;
				raw_data[index] -= predikce;
			}

			// prvni na sloupci
			assert( index % rozmer_x == 0 );
			assert( index );

			raw_data[index] -= raw_data[index - rozmer_x];
			--index;

			assert( index % rozmer_x == rozmer_x - 1 );
		}

		// posledni radek = left
		for ( int sloupec = rozmer_x - 1; sloupec != 0; --sloupec, --index ) {
			auto& A = raw_data[index-1];
			int predikce = A;
			raw_data[index] -= predikce;
			assert(index);
		}

		assert( index == 0 );
	}



	void Diag::reverse_predict(Shared_data data) {

		const auto rozmer_x = data->rozmer_x;
		const auto rozmer_y = data->rozmer_y;
		auto &raw_data = data->data;

		int index = 1;
		int predikce = raw_data[0];

		// prvni radek
		for( ; index < rozmer_x; ++index ) {
			raw_data[index] += predikce;
			predikce = raw_data[index];
		}

		assert( index == rozmer_x );

		while( index < rozmer_x*rozmer_y )
		{
			// prvni na radku
			assert( index % rozmer_x == 0 );
			assert( index );

			raw_data[index] += raw_data[index - rozmer_x];
			++index;

			assert( index % rozmer_x == 1 );

			// vsechny ostatni
			for ( int sloupec = 1; sloupec < rozmer_x; ++sloupec, ++index )
			{
				auto& A = raw_data[index-rozmer_x-1];
				int predikce = A;

				raw_data[index] += predikce;
			}
		}
		assert( index == rozmer_x*rozmer_y );
	}



}
