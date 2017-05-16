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
#include <cassert>

#include "predictor_inter_all.h"
#include "std_macro.h"


namespace Predictor {
	void Gradient_adjusted::forward_predict(Shared_data data) {

		const auto rozmer_x = data->rozmer_x;
		const auto rozmer_y = data->rozmer_y;
		auto &raw_data = data->data;
		
		int index = rozmer_x*rozmer_y -1; // posledni validni


		for( int radek = rozmer_y - 1; radek != 0; --radek )
		{
			for ( int sloupec = rozmer_x - 1; sloupec != 0; --sloupec, --index ) {
				auto& WW	= raw_data[index - 1];
				auto& W		= raw_data[index - ((sloupec>1)?2:1) ];

				auto& NW	= raw_data[index - rozmer_x - 1];
				auto& N		= raw_data[index - rozmer_x    ];
				auto& NE	= raw_data[index - rozmer_x + ((sloupec != rozmer_x-1)?1:0) ];

				const int nahore_k_dispozici = rozmer_x*((radek > 1)?2:1);
				auto& NN	= raw_data[index - nahore_k_dispozici ];
				auto& NNE	= raw_data[index - nahore_k_dispozici + ((sloupec != rozmer_x-1)?1:0) ];

				int dh    = ABS(W-WW) + ABS(N-NW) + ABS(N-NE),
					dv    = ABS(W-NW) + ABS(N-NN) + ABS(NE-NNE),
					predikce = (W+N)/2 + (NE+NW)/4;

				if		( dv - dh >  80 )
					predikce = W;
				else if ( dv - dh < -80 )
					predikce = N;
				else {
					if		( dv - dh > 32 ) 
						predikce = (  predikce + W)/2;
					else if ( dv - dh > 8  )
						predikce = (3*predikce + W)/4;
					else if ( dv - dh < -32 )
						predikce = (  predikce + N)/2;
					else if ( dv - dh < -8  )
						predikce = (3*predikce + N)/4;
				}

				raw_data[index] -= predikce;
			}

			// prvni ve sloupci (prediktor = up)
			assert( index % rozmer_x == 0 );
			assert( index );

			raw_data[index] -= raw_data[index-rozmer_x];
			index -= 1;
		}

		assert( index == rozmer_x - 1 );

		// na prvnim radku
		while( index ) {
			raw_data[index] -= raw_data[index-1];
			index -= 1;
		}

		assert(index == 0);
	}

	void Gradient_adjusted::reverse_predict(Shared_data data) {

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

		int radek = 0;

		while( index < rozmer_x*rozmer_y )
		{
			// prvni na radku
			assert( index % rozmer_x == 0 );
			assert( index );

			radek += 1;

			raw_data[index] = raw_data[index] + raw_data[index-rozmer_x];
			index += 1;

			// vsechny ostatni
			for ( int sloupec = 1; sloupec < rozmer_x; ++sloupec, ++index )
			{
				const auto& WW = raw_data[index - 1];
				const auto& W  = raw_data[index - ((sloupec>1)?2:1) ];

				const auto& NW = raw_data[index - rozmer_x - 1];
				const auto& N  = raw_data[index - rozmer_x    ];
				const auto& NE = raw_data[index - rozmer_x + ((sloupec != rozmer_x-1)?1:0) ];

				const int nahore_k_dispozici = rozmer_x*((radek > 1)?2:1);
				const auto& NN  = raw_data[index - nahore_k_dispozici ];
				const auto& NNE = raw_data[index - nahore_k_dispozici + ((sloupec != rozmer_x-1)?1:0) ];

				int dh    = ABS(W-WW) + ABS(N-NW) + ABS(N-NE),
					dv    = ABS(W-NW) + ABS(N-NN) + ABS(NE-NNE),
					predikce = (W+N)/2 + (NE+NW)/4;

				if		( dv - dh >  80 )
					predikce = W;
				else if ( dv - dh < -80 )
					predikce = N;
				else {
					if		( dv - dh > 32 ) 
						predikce = (  predikce + W)/2;
					else if ( dv - dh > 8  )
						predikce = (3*predikce + W)/4;
					else if ( dv - dh < -32 )
						predikce = (  predikce + N)/2;
					else if ( dv - dh < -8  )
						predikce = (3*predikce + N)/4;
				}

				raw_data[index] += predikce;
			}
		}
		assert( index == rozmer_x*rozmer_y );
	}
}
