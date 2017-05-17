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

#include "dekompozice_internal.h"


Shared_command 
vyber_dekompozice (Environment* env) 
{
	using namespace Dekompozice;
	const auto &max_level_dekompozice = env->nastaveni->dekompozice_max;

	switch( env->nastaveni->dekompozice ) {

		case WTlib::DEKOMPOZICE::PYRAMIDOVA:
			return std::make_shared<Dek_pyramidova>(max_level_dekompozice);
			break;

		case WTlib::DEKOMPOZICE::RED_BLACK:
			return std::make_shared<Dek_red_black>(max_level_dekompozice);
			break;

		default:
			throw std::invalid_argument(
					"Tato dekompozice neni implementovana");
			break;
	}
}
