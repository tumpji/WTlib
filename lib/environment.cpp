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
#include "main.h"

#include "barevna_transformace.h"
#include "dekompozice.h"
#include "predictor.h"

// ENCODER
Environment::Environment ( WTlib::WTencoder* encoder, std::string cesta)
	:   WTio::IO_thread(SMER::KODER, cesta),
	    Environment_stacks(),
		nastaveni(encoder),
		smer(SMER::KODER)
{
}

// DEKODER
Environment::Environment ( WTlib::WTdecoder* dekoder, std::string cesta)
	:	WTio::IO_thread(SMER::DEKODER, cesta),
		Environment_stacks(),
		nastaveni(dekoder),
		smer(SMER::DEKODER)
{
}

void Environment::start() {
	std::vector<Shared_data> tmp; // ulozeni rozdelanych
	auto &vstupni_pasma = nastaveni->tmp_data_vec;
	
	if ( smer == SMER::KODER ) {
		provest_barevnou_transformaci( smer, nastaveni->barevny_prostor, vstupni_pasma );

		auto prediktor = get_predictor(this);
		for ( auto &p : vstupni_pasma )
			prediktor->forward_predict(p);
	}

	// dokud je co kodovat / dekodovat 
	while (nastaveni->tmp_data_vec.size()) {
		data_stack.push( vstupni_pasma.back() );
		vstupni_pasma.pop_back();

		// vlozim WT ktera vola entropicky koder sama
		push_command(vyber_dekompozice(this));

		// vsechny prikazy se provedou v naplanovanem poradi
		while (command_stack.size()) {
			auto top = get_command();
			top->call(this);
		}
		
		// neni nic na stacku pro zpracovana data
		assert(data_stack.size() == 0);
		assert(command_stack.size() == 0);

		if (smer == SMER::DEKODER) {
			assert(data_stack_recon.size() == 1);
			tmp.push_back( data_stack_recon.top() );
			data_stack_recon.pop(); // obracene poradi !
		}
	}

	assert(command_stack.size() == 0);
	assert(data_stack.size() == 0);
	assert(data_stack_recon.size() == 0);
	assert(nastaveni->tmp_data_vec.size() == 0);

	if ( smer == SMER::DEKODER ) {
		vstupni_pasma.insert( vstupni_pasma.begin(),
				tmp.rbegin(), tmp.rend() ); // obracene

		auto prediktor = get_predictor(this);
		for ( auto &p : vstupni_pasma )
			prediktor->reverse_predict(p);

		provest_barevnou_transformaci(smer, nastaveni->barevny_prostor, vstupni_pasma);
	}


	/* pro generovani vlny
	if ( nastaveni->verbose ) {
		RB_obrazek( this );
	}
	*/
}
