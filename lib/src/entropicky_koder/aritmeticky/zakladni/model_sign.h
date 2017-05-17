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

#ifndef SIGN_MODEL_HEADER_INCLUDED
#define SIGN_MODEL_HEADER_INCLUDED

#include <vector>
#include "aritmeticky.h"

namespace SignModel {

// bazova trida, ktera slouzi pro ulozeni znamenka z libovolnych dat
struct SignModel_base {
    virtual void 
        encode ( Environment*, Aritmeticky_koder &, Shared_data data, OZNACENI_PASMA pasmo ) = 0;
    virtual std::vector<bool> 
        decode ( Environment*, Aritmeticky_koder &, Shared_data data, OZNACENI_PASMA pasmo ) = 0;
};

enum Type { T32 };

SignModel_base* get_sign_model_by_type ( Type type );

// inicializace - provadi se pred kodovanim a dekodovanim
void init_sign_models ();

}



#endif // SIGN_MODEL_HEADER_INCLUDED

