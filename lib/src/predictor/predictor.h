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
#ifndef PREDICTOR_HEADER_INCLUDED
#define PREDICTOR_HEADER_INCLUDED

#include <memory>

#include "main.h"


namespace Predictor {

class Predictor {
public:
	Predictor () {}
	virtual ~Predictor () {}

	virtual void forward_predict (Shared_data) = 0;
	virtual void reverse_predict (Shared_data) = 0;
};

using Shared = std::shared_ptr<Predictor>;
}

// vraci ten spravny prediktor pro danou 
Predictor::Shared  get_predictor  ( Environment * env, OZNACENI_PASMA pasmo );

// preidktor pro puvodni pasmo
Predictor::Shared  get_predictor  ( Environment * env );






#endif // PREDICTOR_HEADER_INCLUDED
