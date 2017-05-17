/*
 *  This file is part of WTlib.
 *
 *  WTlib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  WTlib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with WTlib.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cassert>

#include "main.h"

#include "dekompozice_internal.h"
#include "rozlozeni.h"

#include "vlnka.h"

#include "std_macro.h"
#include "zpracovani_pasma.h"


namespace {

	struct PasmaRB {
		PasmaRB () {}
		PasmaRB( int rozmer_x, int rozmer_y, WTdata::Allocation_options alloc ) {
			assert(rozmer_x >= 2);
			assert(rozmer_y >= 2);
			//black = std::make_shared<WTdata>( rozmer_x * rozmer_y / 2, 1, alloc );
			black_blue	= std::make_shared<WTdata>( rozmer_x/2		, (rozmer_y+1)/2, alloc);
			black_yellow= std::make_shared<WTdata>( (rozmer_x+1)/2	, rozmer_y/2	, alloc);

			yellow		= std::make_shared<WTdata>( rozmer_x/2		, rozmer_y/2	, alloc );
			blue		= std::make_shared<WTdata>( (rozmer_x+1)/2	, (rozmer_y+1)/2, alloc );
		}
		
		Shared_data black_blue;   // 0 radek
		Shared_data black_yellow; // 1 radek
		Shared_data yellow;
		Shared_data blue;
	};

#define PREDICT (not update)
#define UPDATE  ( update   )

// definice pro asserty
#define GET_ROW(X)			( X/rozmer_x )
#define GET_COL(X)			( X%rozmer_x )
#define IS_RED(X)			( (GET_ROW(X)+GET_COL(X))%2 == 0 )
#define IS_BLACK(X)			( not IS_RED(X) )
#define IS_RED_BLUE(X)		( GET_ROW(X)%2 == 0 and IS_RED(X) )
#define IS_RED_YELLOW(X)	( GET_ROW(X)%2 == 1 and IS_RED(X) )
#define IS_BLACK_BLUE(X)	( GET_ROW(X)%2 == 0 and IS_BLACK(X) )
#define IS_BLACK_YELLOW(X)	( GET_ROW(X)%2 == 1 and IS_BLACK(X) )

#define NAHORE_VLEVO		( 0 )
#define NAHORE_VPRAVO		( rozmer_x - 1 )
#define DOLE_VLEVO			( rozmer_x*(rozmer_y - 1) )
#define DOLE_VPRAVO			( rozmer_x*rozmer_y - 1 )
	
	// horizontalni predict & update -----
	void horizontalni_vertikalni (
		Environment* env, int i_lifting, bool update,  bool reversed, Shared_data data) 
	{
		std::shared_ptr<Vlnka> vlnka = get_vlnka(env);

		int deleni_dolu = 1;
		if ( PREDICT )	deleni_dolu += vlnka->predict_left_shift(i_lifting);
		else			deleni_dolu += vlnka->update_left_shift(i_lifting);

		// vyber predict / update
		Vlnka::Lift_ptr funkce = ( PREDICT )?
				( &Vlnka::lifting_predict ):( &Vlnka::lifting_update );

		int rozmer_x = data->rozmer_x,
			rozmer_y = data->rozmer_y;
		auto &raw_data = data->data;
		
		std::vector<int> puvodni_hodnoty; 
		puvodni_hodnoty.reserve(rozmer_x * rozmer_y / 2 + 20 );

		// uprava cernych/cervenych na 0 a ulozeni hodnoty do "puvodni_hodnoty"
		for ( int i_radek = 0;  i_radek < rozmer_y; ++i_radek )
		{
			int i_zacatek_radku = i_radek * rozmer_x;
			int i_konec_radku   = i_zacatek_radku + rozmer_x - 1;
			int i_first = i_zacatek_radku + (((i_radek % 2 == 0) xor UPDATE)?(1):(0)) ;

			for ( int i = i_first; i <= i_konec_radku ; i += 2 ) {
				auto& cor_data = raw_data[i];
				puvodni_hodnoty.push_back(cor_data);
				cor_data = 0;
			}
		}

		// vertikalni |   |   |   |
		for ( int i_sloupec = 0;  i_sloupec < rozmer_x;  ++i_sloupec )
		{
			int i_zacatek_radku = i_sloupec;
			int i_konec_radku   = rozmer_x * (rozmer_y - 1) + i_sloupec;
			int i_first = i_zacatek_radku + ( ((i_sloupec%2==0) xor UPDATE)?(rozmer_x):(0) );

			assert( (IS_RED(i_first) and UPDATE) or ( IS_BLACK(i_first) and PREDICT ) );

			(vlnka.get()->*funkce)(data->data, i_lifting, reversed, false, 
					i_zacatek_radku, i_first, i_konec_radku, rozmer_x );
		}

		const bool plus = update xor reversed; // bude se pricitat vypoctena hodnota (pred,upd) ?
		int i_puvodni = 0;

		// horizontalni predict ---------------
		for ( int i_radek = 0;  i_radek < rozmer_y; ++i_radek )
		{
			int i_zacatek_radku = i_radek * rozmer_x;
			int i_konec_radku   = i_zacatek_radku + rozmer_x - 1;
			int i_first; 
			if ( PREDICT ) i_first = ((i_radek % 2)?(0):(1)) + i_zacatek_radku;
			else              i_first = ((i_radek % 2)?(1):(0)) + i_zacatek_radku;

			assert( (IS_RED(i_first) and UPDATE) or (IS_BLACK(i_first) and PREDICT) );

			(vlnka.get()->*funkce)(data->data, i_lifting, reversed, false,
					i_zacatek_radku, i_first, i_konec_radku, 1);

			// zaverecne deleni dat
			for ( int i = i_first; i <= i_konec_radku ; i += 2 ) {
				auto& cor_data = data->data[i];
				cor_data >>= deleni_dolu;

				assert( static_cast<unsigned>(i_puvodni) < puvodni_hodnoty.size() );
				cor_data = puvodni_hodnoty[i_puvodni++] + ((plus)?(cor_data):(-cor_data));
			}
		}
		assert( static_cast<unsigned>(i_puvodni) == puvodni_hodnoty.size() );
	}


	void diagonalni_red (
			Environment* env, int i_lifting, bool update, bool reversed, Shared_data blue_copy) 
	{
		std::shared_ptr<Vlnka> vlnka = get_vlnka(env);

		int deleni_dolu = 1;
		if ( PREDICT ) deleni_dolu += vlnka->predict_left_shift(i_lifting);
		else              deleni_dolu += vlnka->update_left_shift(i_lifting);

		// vyber predict / update
		Vlnka::Lift_ptr funkce = ( PREDICT )?
				( &Vlnka::lifting_predict ):( &Vlnka::lifting_update );

		const auto& rozmer_x = blue_copy->rozmer_x;
		const auto& rozmer_y = blue_copy->rozmer_y;
		auto& raw_data = blue_copy->data;

		std::vector<int> puvodni_hodnoty;  puvodni_hodnoty.reserve(rozmer_x*rozmer_y/4+20);

		// init nastaveni hodnot na 0
		for (  int i_radek = ((PREDICT)?(1):(0)); i_radek < rozmer_y; i_radek += 2 ) {
			int i_zacatek_radku = i_radek * rozmer_x + ((PREDICT)?(1):(0));
			int i_konec_radku   = i_zacatek_radku + rozmer_x - 1 + ((PREDICT)?(-1):(0));

			assert( (IS_RED_BLUE(i_zacatek_radku) and UPDATE) or
					(IS_RED_YELLOW(i_zacatek_radku) and PREDICT) );

			for ( int i = i_zacatek_radku; i <= i_konec_radku ; i += 2 ) {
				auto& cor_data = raw_data[i];
				puvodni_hodnoty.push_back(cor_data);
				cor_data = 0;
			}
		}

		// diagonalni do dolniho praveho rohou
		{
			int step = rozmer_x + 1;
			// z vrchu
			for ( int i = 0; i < rozmer_x - 1; i += 2 )
			{   //                    pocet klesani    posledni radek
				int dolni_odhad = i + (rozmer_y - 1) + rozmer_x*(rozmer_y - 1);
				int pravy_odhad = i + (rozmer_x - 1 - i)*step;
				int i_zacatek_diag = i;
				int i_first = i + ((PREDICT)?(step):(0));
				int i_konec_diag = MIN(dolni_odhad, pravy_odhad);

				assert( IS_RED(i_zacatek_diag) and IS_RED(i_konec_diag) );
				assert( (IS_RED_BLUE(i_first) and UPDATE) or
						(IS_RED_YELLOW(i_first) and PREDICT) );

				((vlnka.get())->*funkce)( raw_data, i_lifting, reversed, false, 
						i_zacatek_diag, i_first, i_konec_diag, step );

			}
			// z leveho boku
			for ( int i = 2; i < rozmer_y - 1; i+= 2 )
			{
				int i_zacatek_diag = i * rozmer_x;
				int i_first = i_zacatek_diag + ((PREDICT)?(step):(0));
				int pravy_odhad = i_zacatek_diag + step*(rozmer_x-1);
				int dolni_odhad = i_zacatek_diag + step*(rozmer_y-1-i);
				int i_konec_diag   = MIN(pravy_odhad, dolni_odhad);

				assert( IS_RED(i_zacatek_diag) and IS_RED(i_konec_diag) );
				assert( (IS_RED_BLUE(i_first) and UPDATE) or
						(IS_RED_YELLOW(i_first) and PREDICT) );

				((vlnka.get())->*funkce)( raw_data, i_lifting, reversed, false,
						i_zacatek_diag, i_first, i_konec_diag, step );
			}
		}

		// diagonalni do dolniho leveho rohu
		{
			int step = rozmer_x - 1;
			bool sude_x = (rozmer_x % 2) == 0;

			// nahore do leveho dolniho rohu
			for ( int i = 2; i < rozmer_x; i += 2 )
			{
				int i_zacatek_diag	= i; 
				int i_first         = i_zacatek_diag + ((PREDICT)?(step):(0));
				int levy_odhad		= i_zacatek_diag + step * (i);
				int dolni_odhad     = i_zacatek_diag + step * (rozmer_y - 1);
				int i_konec_diag    = MIN(levy_odhad, dolni_odhad);

				assert( IS_RED(i_zacatek_diag) and IS_RED(i_konec_diag) );
				assert( (IS_RED_BLUE(i_first) and UPDATE) or
						(IS_RED_YELLOW(i_first) and PREDICT) );

				((vlnka.get())->*funkce)( raw_data, i_lifting, reversed, false,
					i_zacatek_diag, i_first, i_konec_diag, step);
			}

			// z praveho boku do leveho dolniho rohu
			for ( int i = ((sude_x)?(1):(2)); i < rozmer_y - 1; i += 2 )
			{
				int i_zacatek_diag = i * rozmer_x + rozmer_x - 1;
				int i_first        = i_zacatek_diag + ((sude_x xor update)?(0):(step));
				int dolni_odhad    = i_zacatek_diag + step * (rozmer_y - 1 - i);
				int levy_odhad     = i_zacatek_diag + step * (rozmer_x - 1);
				int i_konec_diag   = MIN(levy_odhad, dolni_odhad);

				assert( IS_RED(i_zacatek_diag) and IS_RED(i_konec_diag) );
				assert( (IS_RED_BLUE(i_first) and UPDATE) or
						(IS_RED_YELLOW(i_first) and PREDICT) );

				((vlnka.get())->*funkce)( raw_data, i_lifting, reversed, false,
					i_zacatek_diag, i_first, i_konec_diag, step);
			}
		}

		// nektere hodnoty byli pouze 1x liftovany (rohy)
		if ( UPDATE ) { 
			// nahore vlevo (vzdy)
			raw_data[NAHORE_VLEVO] <<= 1;
			// dole vlevo 
			if ( rozmer_y % 2 == 1 ) {
				assert( IS_RED_BLUE(DOLE_VLEVO) );
				raw_data[DOLE_VLEVO] <<= 1; 
			}
			// nahore vpravo
			if ( rozmer_x % 2 == 1 ) { 
				assert( IS_RED_BLUE(NAHORE_VPRAVO) );
				raw_data[NAHORE_VPRAVO] <<= 1;
			}
			// dole vpravo
			if ( rozmer_x % 2 == 1  && rozmer_y % 2 == 1) {
				assert( IS_RED_BLUE(DOLE_VPRAVO) );
				raw_data[DOLE_VPRAVO] <<= 1;
			}

		} else { // predict
			// dole vpravo
			if ( rozmer_x % 2 == 0 && rozmer_y % 2 == 0 ) {
				assert( IS_RED_YELLOW(DOLE_VPRAVO) );
				raw_data[DOLE_VPRAVO] <<= 1;
			}
		}

		// zaverecny vypocet
		const bool plus = update xor reversed;
		int i_puvodni = 0; // index ktera hodnota je na rade pro rekonstrukci (po nulovani)

		for (  int i_radek = ((PREDICT)?(1):(0)); i_radek < rozmer_y; i_radek += 2 ) {
			int i_zacatek_radku = i_radek * rozmer_x + ((PREDICT)?(1):(0));
			int i_konec_radku   = i_zacatek_radku + rozmer_x - 1 + ((PREDICT)?(-1):(0));

			assert( (IS_RED_BLUE(i_zacatek_radku) and UPDATE) or
					(IS_RED_YELLOW(i_zacatek_radku) and PREDICT) );

			for ( int i = i_zacatek_radku; i <= i_konec_radku; i += 2 ) {
				auto& cor_data = raw_data[i];
				cor_data >>= deleni_dolu;

				assert(static_cast<unsigned>(i_puvodni) < puvodni_hodnoty.size());
				cor_data = puvodni_hodnoty[i_puvodni++] + ((plus)?(cor_data):(-cor_data));
			}
		}
		assert( static_cast<unsigned>(i_puvodni) == puvodni_hodnoty.size() );
	}

	void diagonalni_black (
			Environment* env, int i_lifting, bool update, bool reversed, Shared_data black_copy) 
	{
		std::shared_ptr<Vlnka> vlnka = get_vlnka(env);

		int deleni_dolu = 1;
		if ( PREDICT ) deleni_dolu += vlnka->predict_left_shift(i_lifting);
		else              deleni_dolu += vlnka->update_left_shift(i_lifting);

		// vyber predict / update
		Vlnka::Lift_ptr funkce = ( PREDICT )?
				( &Vlnka::lifting_predict ):( &Vlnka::lifting_update );

		const auto& rozmer_x = black_copy->rozmer_x;
		const auto& rozmer_y = black_copy->rozmer_y;
		auto& raw_data = black_copy->data;

		std::vector<int> puvodni_hodnoty;  puvodni_hodnoty.reserve(rozmer_x*rozmer_y/4 + 20);

		// init nastaveni hodnot na 0
		for (  int i_radek = ((PREDICT)?(1):(0)); i_radek < rozmer_y; i_radek += 2 ) {
			int i_zacatek_radku = i_radek * rozmer_x + ((UPDATE)?(1):(0));
			int i_konec_radku   = i_zacatek_radku + rozmer_x - 1 + ((UPDATE)?(-1):(0));
			assert( (IS_BLACK_BLUE(i_zacatek_radku) and UPDATE) or
					(IS_BLACK_YELLOW(i_zacatek_radku) and PREDICT) );
			for ( int i = i_zacatek_radku; i <= i_konec_radku; i += 2 ) {
				auto& cor_data = raw_data[i];
				puvodni_hodnoty.push_back(cor_data);
				cor_data = 0;
			}
		}

		// diagonalni do dolniho praveho rohou
		{
			const int step = rozmer_x + 1;
			// z vrchu
			for ( int i = 1; i < rozmer_x-1; i += 2 )
			{   //                    pocet klesani    posledni radek
				int dolni_odhad = i + (rozmer_y - 1) + rozmer_x*(rozmer_y - 1);
				int pravy_odhad = i + (rozmer_x - 1 - i)*step;
				int i_zacatek_diag = i;
				int i_first = i + ((PREDICT)?(step):(0));
				int i_konec_diag = MIN(dolni_odhad, pravy_odhad);

				assert( IS_BLACK(i_zacatek_diag) and IS_BLACK(i_konec_diag) );
				assert( (IS_BLACK_BLUE(i_first) and UPDATE) or
						(IS_BLACK_YELLOW(i_first) and PREDICT) );

				((vlnka.get())->*funkce)( raw_data, i_lifting, reversed, false, 
						i_zacatek_diag, i_first, i_konec_diag, step );
			}

			// z leveho boku
			for ( int i = 1; i < rozmer_y - 1; i+= 2 )
			{
				int i_zacatek_diag = i * rozmer_x;
				int i_first = i_zacatek_diag + ((UPDATE)?(step):(0));
				int pravy_odhad = i_zacatek_diag + step*(rozmer_x-1);
				int dolni_odhad = i_zacatek_diag + step*(rozmer_y-1-i);
				int i_konec_diag   = MIN(pravy_odhad, dolni_odhad);

				assert( IS_BLACK(i_zacatek_diag) and IS_BLACK(i_konec_diag) );
				assert( (IS_BLACK_BLUE(i_first) and UPDATE) or
						(IS_BLACK_YELLOW(i_first) and PREDICT) );

				((vlnka.get())->*funkce)( raw_data, i_lifting, reversed, false,
						i_zacatek_diag, i_first, i_konec_diag, step );
			}
		}

		// diagonalni do dolniho leveho rohu
		{
			const int step = rozmer_x - 1;
			const bool sude_x = (rozmer_x % 2) == 0;

			// nahore
			for ( int i = 1; i < rozmer_x; i += 2 )
			{
				int i_zacatek_diag	= i; 
				int i_first         = i_zacatek_diag + ((PREDICT)?(step):(0));
				//int levy_odhad		= i_zacatek_diag + step * (i - 1);
				int levy_odhad		= i_zacatek_diag + step * (i);
				int dolni_odhad     = i_zacatek_diag + step * (rozmer_y - 1);
				int i_konec_diag    = MIN(levy_odhad, dolni_odhad);

				assert( IS_BLACK(i_zacatek_diag) and IS_BLACK(i_konec_diag) );
				assert( (IS_BLACK_BLUE(i_first) and UPDATE) or
						(IS_BLACK_YELLOW(i_first) and PREDICT) );

				((vlnka.get())->*funkce)( raw_data, i_lifting, reversed, false,
					i_zacatek_diag, i_first, i_konec_diag, step);
			}

			// z praveho boku dolu
			for ( int i = ((sude_x)?(2):(1)); i < rozmer_y - 1; i += 2 )
			{   //                    radek          na konec  
				int i_zacatek_diag = i*rozmer_x + (rozmer_x - 1);
				int i_first        = i_zacatek_diag + ((sude_x xor UPDATE)?(step):(0));
				int dolni_odhad    = i_zacatek_diag + step * (rozmer_y - 1 - i);
				int levy_odhad     = i_zacatek_diag + step * (rozmer_x - 1);
				int i_konec_diag   = MIN(levy_odhad, dolni_odhad);

				assert( IS_BLACK(i_zacatek_diag) and IS_BLACK(i_konec_diag) );
				assert( (IS_BLACK_BLUE(i_first) and UPDATE) or
						(IS_BLACK_YELLOW(i_first) and PREDICT) );

				((vlnka.get())->*funkce)( raw_data, i_lifting, reversed, false,
					i_zacatek_diag, i_first, i_konec_diag, step);
			}
		}

		// nahore vlevo
		if ( PREDICT ) { 
			// dole vlevo
			if ( rozmer_y % 2 == 0 ) { 
				assert(IS_BLACK_YELLOW(DOLE_VLEVO));
				raw_data[DOLE_VLEVO] <<= 1; 
			}

			// dole vpravo
			if ( rozmer_x % 2 == 1  && rozmer_y % 2 == 0) {
				assert( IS_BLACK_YELLOW(DOLE_VPRAVO) );
				raw_data[DOLE_VPRAVO] <<= 1;
			}
		}
		else { // UPDATE 
			// dole vpravo
			if ( rozmer_x % 2 == 0 && rozmer_y % 2 == 1 ) {
				assert( IS_BLACK_BLUE(DOLE_VPRAVO) );
				raw_data[DOLE_VPRAVO] <<= 1;
			}
			// nahore vpravo
			if ( rozmer_x % 2 == 0 ) {
				assert( IS_BLACK_BLUE(NAHORE_VPRAVO) );
				raw_data[NAHORE_VPRAVO] <<= 1;
			}
		}

		// zaverecny vypocet
		const bool plus = UPDATE xor reversed;
		int i_puvodni = 0; // index pro rekonstrukci po nulovani

		for (  int i_radek = ((PREDICT)?(1):(0)); i_radek < rozmer_y; i_radek += 2 ) {
			int i_zacatek_radku = i_radek * rozmer_x + ((UPDATE)?(1):(0));
			int i_konec_radku   = i_zacatek_radku + rozmer_x - 1 + ((UPDATE)?(-1):(0));
			assert( (IS_BLACK_BLUE(i_zacatek_radku) and UPDATE) or
					(IS_BLACK_YELLOW(i_zacatek_radku) and PREDICT) );
			for ( int i = i_zacatek_radku; i <= i_konec_radku; i += 2 ) {
				auto& cor_data = raw_data[i];
				cor_data >>= deleni_dolu;

				assert(static_cast<unsigned>(i_puvodni) < puvodni_hodnoty.size());
				cor_data = puvodni_hodnoty[i_puvodni++] + ((plus)?(cor_data):(-cor_data));
			}
		}

		assert( static_cast<unsigned>(i_puvodni) == puvodni_hodnoty.size() );
	}

}


namespace {

	// rozdeli pasmo na podpasma (Black ..., yellow, blue) ...
	PasmaRB rozdelit (Shared_data src)
	{
		int rozmer_x = src->rozmer_x,
			rozmer_y = src->rozmer_y;

		PasmaRB return_val (rozmer_x, rozmer_y, WTdata::Allocation_options::ALLOCATE);
		auto& blue				= return_val.blue;
		auto& yellow			= return_val.yellow;
		auto& black_blue		= return_val.black_blue;
		auto& black_yellow		= return_val.black_yellow;

		unsigned int index_black_blue	= 0;
		unsigned int index_black_yellow	= 0;
		unsigned int index_yellow		= 0;
		unsigned int index_blue			= 0;

		//assert(yellow->rozmer_x >= 0);

		for ( int i_radek = 0; i_radek < rozmer_y; ++i_radek ) {
			int i_zacatek_radku = i_radek * rozmer_x;
			int i_konec_radku   = i_zacatek_radku + rozmer_x - 1;
			//assert(yellow->rozmer_x >= 0);

			if ( i_radek % 2 == 0 )
			{
				for ( int i = i_zacatek_radku; i <= i_konec_radku; i += 2 ) {
					assert( index_blue < blue->data.size() );
					blue->data[index_blue++] = src->data[i]; 
				}
				for ( int i = i_zacatek_radku + 1; i <= i_konec_radku; i += 2 ) {
					assert(index_black_blue < black_blue->data.size());
					black_blue->data[index_black_blue++] = src->data[i];
				}
			}
			else {
				for ( int i = i_zacatek_radku; i <= i_konec_radku; i += 2 )
				{
					assert(index_black_yellow < black_yellow->data.size());
					black_yellow->data[index_black_yellow++] = src->data[i];
				}
				for ( int i = i_zacatek_radku + 1; i <= i_konec_radku; i+= 2 ) {
					assert(index_yellow < yellow->data.size() );
					yellow->data[index_yellow++] = src->data[i];
				}
			}
		}
		assert(return_val.yellow->rozmer_x >= 0);
		return return_val;
	}

	// spoji Black, ... , Blue do jednoho pasma
	Shared_data spojit (PasmaRB pasma)
	{
		int rozmer_x = (pasma.blue->rozmer_x*2 + pasma.yellow->rozmer_x*2)/2,
			rozmer_y = (pasma.blue->rozmer_y*2 + pasma.yellow->rozmer_y*2)/2;

		Shared_data dst = std::make_shared<WTdata>(
				rozmer_x, rozmer_y, WTdata::Allocation_options::ALLOCATE);

		auto& blue				= pasma.blue;
		auto& yellow			= pasma.yellow;
		auto& black_blue		= pasma.black_blue;
		auto& black_yellow		= pasma.black_yellow;

		int index_black_blue	= 0;
		int index_black_yellow	= 0;
		int index_yellow		= 0;
		int index_blue			= 0;

		for ( int i_radek = 0; i_radek < rozmer_y; ++i_radek ) {
			int i_zacatek_radku = i_radek * rozmer_x;
			int i_konec_radku   = i_zacatek_radku + rozmer_x - 1;

			if ( i_radek % 2 == 0 )
			{
				for ( int i = i_zacatek_radku; i <= i_konec_radku; i += 2 )
					 dst->data[i] = blue->data[index_blue++]; 
				for ( int i = i_zacatek_radku + 1; i <= i_konec_radku; i += 2 )
					 dst->data[i] = black_blue->data[index_black_blue++];
			}
			else {
				for ( int i = i_zacatek_radku; i <= i_konec_radku; i += 2 )
					dst->data[i] = black_yellow->data[index_black_yellow++];
				for ( int i = i_zacatek_radku + 1; i <= i_konec_radku; i += 2 )
					dst->data[i] = yellow->data[index_yellow++];
			}
		}
		return dst;
	}

	// rozkoduje pasmo
	PasmaRB red_black_forward (Environment* env, Shared_data data, bool liftred, bool liftblack) {
		std::shared_ptr<Vlnka> vlnka = get_vlnka(env);
		assert( MIN(data->rozmer_x, data->rozmer_y) >= 2 );

		bool update = true, predict = false;

		for ( int i_lifting = 0; i_lifting < vlnka->get_number_lifting_steps(); ++i_lifting )
		{
			::horizontalni_vertikalni(env, i_lifting, predict, false, data);
			::horizontalni_vertikalni(env, i_lifting, update , false, data);
		}
		for ( int i_lifting = 0; i_lifting < vlnka->get_number_lifting_steps(); ++i_lifting )
		{
			if ( liftred ) {
			::diagonalni_red         (env, i_lifting, predict, false, data);
			::diagonalni_red         (env, i_lifting, update , false, data);
			}
			if ( liftblack ) {
				::diagonalni_black       (env, i_lifting, predict, false, data);
				::diagonalni_black       (env, i_lifting, update , false, data);
			}
		}

		return ::rozdelit(data);
	}
	

	// spojuje 
	Shared_data red_black_reverse (Environment* env, PasmaRB pasma, bool liftred, bool liftblack ) 
	{
		Shared_data data = ::spojit(pasma);
		std::shared_ptr<Vlnka> vlnka = get_vlnka(env);
		assert( MIN(data->rozmer_x, data->rozmer_y) >= 2 );

		bool update = true, predict = false;

		for ( int i_lifting = vlnka->get_number_lifting_steps() - 1; i_lifting >= 0; --i_lifting )
		{
			if ( liftred ) {
			::diagonalni_red         (env, i_lifting, update , true, data);
			::diagonalni_red         (env, i_lifting, predict, true, data);
			}
			if ( liftblack ) { 
				::diagonalni_black       (env, i_lifting, update , true, data);
				::diagonalni_black       (env, i_lifting, predict, true, data);
			}
		}
		for ( int i_lifting = vlnka->get_number_lifting_steps() - 1; i_lifting >= 0; --i_lifting )
		{
			::horizontalni_vertikalni(env, i_lifting, update , true, data);
			::horizontalni_vertikalni(env, i_lifting, predict, true, data);
		}
		return data;
	}

} // end unnamed namespace



namespace Dekompozice::RedBlack {

	class Rozlozeni_rb : public Rozlozeni {
	public:
		Rozlozeni_rb(bool sablona, bool liftred, bool liftblack) : 
			Rozlozeni(sablona), liftblack(liftblack), liftred(liftred) {}
		~Rozlozeni_rb () {}
	
		virtual void call (Environment* env);
	private:
		bool liftblack, liftred;

	};

	void Rozlozeni_rb::call (Environment* env)
	{
		if ( IS_KODER ) {
			auto data = get_data(env);
			//auto p    = ::rozdelit(data);
			auto p    = ::red_black_forward(env, data, liftred, liftblack);
			push_data(env, p.black_yellow );
			push_data(env, p.black_blue);
			push_data(env, p.yellow);
			push_data(env, p.blue);
		} else if ( sablona ) {
			auto data = get_data(env);
			auto p    = PasmaRB(data->rozmer_x, data->rozmer_y, 
					WTdata::Allocation_options::DO_NOT_ALLOCATE);
			push_data(env, p.black_yellow);
			push_data(env, p.black_blue);
			push_data(env, p.yellow);
			push_data(env, p.blue);
		} else { // rekonstrukce
			PasmaRB pasma;
			pasma.black_yellow = get_data(env);
			pasma.black_blue = get_data(env);
			pasma.yellow = get_data(env);
			pasma.blue = get_data(env);
			//auto d = ::spojit(pasma);
			auto d = ::red_black_reverse(env, pasma, liftred, liftblack);
			push_data(env, d);
		}
	}

	bool splnuje_kriteria (Environment *env, Shared_data pasmo ) {
		const int t = MIN(pasmo->rozmer_x, pasmo->rozmer_y);
		return t >= env->nastaveni->dekompozice_min_rozmer;
	}
} // end namespace Dekompozice::RedBlack


namespace Dekompozice {
void Dek_red_black::call (Environment* env)
{
	using Pasmo = OZNACENI_PASMA;
	assert( level_rozlozeni >= 0 );
	const int side_level = env->nastaveni->dekompozice_side_level;


	if ( not RedBlack::splnuje_kriteria(env, env->data_stack.top()) || not level_rozlozeni ) {
		push_zpracovat_hotove_pasmo(env, pasmo);
		//push_zpracovat_hotove_pasmo(env, Pasmo::BLUE);
	}
	else {
		const int act_side   = MIN(level_rozlozeni - 1, side_level);
		const int after_side = MAX(0,act_side - 1);

		const bool liftred   = level_rozlozeni >= 2; // do pasma BY nebo jen R-B
		const bool liftblack = act_side >= 1;        // jestli se bude rozkladat Black

		assert( not(liftblack and not liftred) );

		// tohle je slozeni RB
		if ( IS_DEKODER ) { 
			COMMAND_PUSH<RedBlack::Rozlozeni_rb>(env, false, liftred, liftblack);
		}

		// act_side == side_level -1
		// side_level == 0 => pasmo BlackY a BlackB je to same bez liftingu
		// side_level == 1 => pasma BlackY a BlackB jsou odlisna pasma, probehl lifting
		// side_level == 2 => act_side == 1 => probehne jeste 1 rozlozeni R|B
		// side_level == 3 => act_side == 2 => Y|Blue + BlackY|BlackBlue
		COMMAND_PUSH<Dek_red_black>(env, after_side, Pasmo::BLACK_YELLOW);
		COMMAND_PUSH<Dek_red_black>(env, after_side, Pasmo::BLACK_BLUE);
		COMMAND_PUSH<Dek_red_black>(env, after_side, Pasmo::YELLOW);

		// blue bylo rozkladano 2x => R|B  a  Y|B
		COMMAND_PUSH<Dek_red_black>(env, MAX(0,level_rozlozeni - 2), Pasmo::BLUE );


		// tohle jsou originalni data / sablona
		COMMAND_PUSH<RedBlack::Rozlozeni_rb>(env, true, liftred, liftblack);
	}



}
}


/*
 * generovat vlnu
void RB_obrazek ( Environment * env ) {
	std::ofstream file;
	file.open("rb_vlna.pgm");

	int init_rozmer = 7;
	Shared_data data = std::make_shared<WTlib::WTdata>(init_rozmer,init_rozmer);
	//data->data[ (init_rozmer*init_rozmer)/2 ] = 1000; // nastaveni aprox na 1
	data->data[ (init_rozmer*init_rozmer)/2 ] = 1024 * 4; // nastaveni aprox na 1

	for ( int kolikrat = 0; kolikrat < 4; ++kolikrat )
	{
		assert( data->rozmer_x == data->rozmer_y );
		// alokace novych
		std::cout << "Jsem na : " << data->rozmer_x << std::endl;
		PasmaRB tmp ( data->rozmer_x * 2, data->rozmer_y * 2, WTdata::Allocation_options::ALLOCATE );

		// dosadi se stare
		if ( kolikrat )
			tmp.blue = data;
		else
			tmp.yellow = data;
			//tmp.black_blue = data;

		data = red_black_reverse ( env, tmp, true, false );
	}

	int maxval = 0;
	int minval = 0;
	for ( unsigned x = 0; x < data->data.size(); ++x )
	{
		const auto & tet = data->data[x];
		if ( maxval < tet ) maxval = tet;
		if ( minval > tet ) minval = tet;
	}

	std::cout << "MAXVAL = " << maxval << std::endl;
	std::cout << "MINVAL = " << minval << std::endl;
	std::cout << "Rozsah = " << maxval - minval << std::endl;

	int rozmer_obrazku = data->rozmer_x;
	file << "P2\n" << 
		rozmer_obrazku << " " << rozmer_obrazku << "\n" << 
		maxval - minval << "\n";

	for ( unsigned x = 0; x < data->data.size(); ++x )
	{
		const auto & tet = data->data[x];

		file << tet - minval << std::endl;
	}

	file.close();
	std::cout << "VYPIS rb_vlna.pgm kompletni" << std::endl;
}
*/

