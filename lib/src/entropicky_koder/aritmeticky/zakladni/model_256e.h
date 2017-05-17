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

#ifndef MODEL256_HEADER_INCLUDED
#define MODEL256_HEADER_INCLUDED

#include "std_macro.h"
#include "models.h"
#include "aritmeticky.h"

namespace ValueModel {

	struct ValueModel_base {
		virtual void encode ( Aritmeticky_koder&, unsigned value ) = 0;
		virtual unsigned decode ( Aritmeticky_koder& ) = 0;

		virtual unsigned get_e_value () const = 0;
		virtual unsigned get_bits () const = 0;
	protected:
		unsigned int soucet;
	};

	enum Type { T16, T32, T64, T128, T256, T512, T1024 };

	ValueModel_base * get_value_model_by_type ( Type type );

	void init_value_models ();
}



#endif // MODEL256_HEADER_INCLUDED
