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
#include <memory>

#include "wt.h"
#include "main.h"

#include "predictor.h"
#include "predictor_inter_all.h"


std::shared_ptr<Predictor::Predictor>
	get_predictor ( WTlib::PREDIKTOR prediktor )
{
	using WTlib::PREDIKTOR;
	using namespace Predictor;

	switch ( prediktor ) {
		case PREDIKTOR::NIC:
			return std::make_shared<Nic>();

		case PREDIKTOR::LEFT:
			return std::make_shared<Left>();

		case PREDIKTOR::UP:
			return std::make_shared<Up>();

		case PREDIKTOR::DIAGONAL:
			return std::make_shared<Diag>();

		case PREDIKTOR::MED:
			return std::make_shared<Med>();

		case PREDIKTOR::PAETH:
			return std::make_shared<Paeth>();

		case PREDIKTOR::GRADIENT_ADJUSTED:
			return std::make_shared<Gradient_adjusted>();

		default:
			throw std::invalid_argument("Coudnt find implementation for this predictor");
			break;
	}
	return nullptr;
}

std::shared_ptr<Predictor::Predictor>
	get_predictor ( Environment *env )
{
	return get_predictor(env->nastaveni->prediktor_pred);
}


std::shared_ptr<Predictor::Predictor>
	get_predictor ( Environment *env, 
					OZNACENI_PASMA pasmo )
{
	auto &mapa = env->nastaveni->prediktor_pasmo;

	auto t = mapa.find(pasmo);
	if ( t == mapa.end() )
		return get_predictor(env->nastaveni->imp_prediktor_rozlozeneho_pasma);
	else 
		return get_predictor(t->second);
}
