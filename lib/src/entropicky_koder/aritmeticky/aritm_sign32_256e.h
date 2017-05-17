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
#ifndef ARITM_MODEL_SIGN32_256E_HEADER_DEFINED
#define ARITM_MODEL_SIGN32_256E_HEADER_DEFINED

#include "entropicky_koder.h"
#include "model_sign.h"
#include "model_256e.h"

template<SignModel::Type st, ValueModel::Type vt>
struct Valxe_signx: public Entropicky_koder {
	Valxe_signx( OZNACENI_PASMA pasmo ) : 
		Entropicky_koder( pasmo ), 
		sign_model( SignModel::get_sign_model_by_type(st) ),
		value_model( ValueModel::get_value_model_by_type(vt) )
	{};

	virtual ~Valxe_signx () {};

	// koduje data podle predanych dat
	virtual void		encode (Environment* env, Shared_data data);
	// dekoduje podle predaneho neinicializovane sablony (resp. vyska, sirka
	virtual Shared_data decode (Environment* env, Shared_data data);
private:

	SignModel::SignModel_base   * sign_model;
	ValueModel::ValueModel_base * value_model;
};


#endif // ARITM_MODEL_SIGN32_256E_HEADER_DEFINED
