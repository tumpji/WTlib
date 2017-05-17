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
#include<iostream>
using namespace std;

#include "wt_inter.h"
#include "main.h"
#include "io.h"

#include "aritm_sign32_256e.h"
#include "model_sign.h"
#include "entropicky_koder.h"
#include "ent_nic.h"
#include "ent_ebcot.h"

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////   DEFINICE RUZNYCH EK  //////////////////////////////////////////// 


namespace {

KODOVACI_MOZNOST ebcot_implicit (OZNACENI_PASMA pasmo) {
	switch(pasmo) {
		case OZNACENI_PASMA::LL:
			return KODOVACI_MOZNOST::EBCOT_LL;
		case OZNACENI_PASMA::LH:
			return KODOVACI_MOZNOST::EBCOT_LH;
		case OZNACENI_PASMA::HL:
			return KODOVACI_MOZNOST::EBCOT_HL;
		case OZNACENI_PASMA::HH:
			return KODOVACI_MOZNOST::EBCOT_HH;

		default:
			throw std::invalid_argument(
					"Cannot determine EBCOT default behaviour for " +
					WTlib::to_string(pasmo) + " band" );
	}
}

std::shared_ptr<Entropicky_koder>  vytvor_podle_moznosti
		(
			OZNACENI_PASMA pasmo,
			KODOVACI_MOZNOST moznost 
		) 
{
	switch(moznost) {
		case KODOVACI_MOZNOST::EBCOT_HH:
		case KODOVACI_MOZNOST::EBCOT_HL:
		case KODOVACI_MOZNOST::EBCOT_LH:
		case KODOVACI_MOZNOST::EBCOT_LL:
			return std::make_shared<Ent_ebcot>(pasmo, moznost);

		default:
			throw std::invalid_argument("Neznam implementaci moznosti entropickeho koderu");
	}
}


} // end unnamed namespace


std::shared_ptr<Entropicky_koder> 
	get_entropicky_koder (Environment* env, const OZNACENI_PASMA pasmo) 
{
	const auto &mapa_vyjimek = env->nastaveni->vyjimka_koder;
	auto t = mapa_vyjimek.find(pasmo);

	KODER jak_defaultne_kodovat = env->nastaveni->koder;


	if ( t != mapa_vyjimek.end() ) {
		switch ( t->second ) {
			case KODOVACI_MOZNOST::EBCOT:
				jak_defaultne_kodovat = KODER::EBCOT;
				break;
			case KODOVACI_MOZNOST::RAW: //return std::make_shared<Ent_NIC>();
				jak_defaultne_kodovat = KODER::RAW;
				break;

			case KODOVACI_MOZNOST::ARITM_256E:
				jak_defaultne_kodovat = KODER::ARITM_256E;
			case KODOVACI_MOZNOST::ARITM_512E:
				jak_defaultne_kodovat = KODER::ARITM_512E;
			case KODOVACI_MOZNOST::ARITM_1024E:
				jak_defaultne_kodovat = KODER::ARITM_1024E;
				break;

			default: // specificke pasmo je jiz vytvoreno
				return vytvor_podle_moznosti(pasmo, t->second);
		}
	}

	// defaultni hodnoty
	switch(jak_defaultne_kodovat) {
		case KODER::RAW:
			return std::make_shared<Ent_NIC>(pasmo);

		case KODER::EBCOT:
			return vytvor_podle_moznosti(pasmo, ::ebcot_implicit(pasmo));

		case KODER::ARITM_256E:
			return std::make_shared<Valxe_signx<SignModel::Type::T32,ValueModel::Type::T256>>(pasmo);
		case KODER::ARITM_512E:
			return std::make_shared<Valxe_signx<SignModel::Type::T32,ValueModel::Type::T512>>(pasmo);
		case KODER::ARITM_1024E:
			return std::make_shared<Valxe_signx<SignModel::Type::T32,ValueModel::Type::T1024>>(pasmo);

		default:
			throw std::invalid_argument("Neznam implementaci entropickeho koderu");
			break;
	}
}

void init_all_entropy_coders () {
	ValueModel::init_value_models();
	SignModel::init_sign_models();
}

