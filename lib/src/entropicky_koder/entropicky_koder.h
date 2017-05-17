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
#ifndef ENT_HEADER_INC
#define ENT_HEADER_INC

#include <memory>

#include "wt_inter.h"
#include "main.h"


// obecna konstrukce entropickeho koderu
class Entropicky_koder {
public:
        
        // koduje data podle predanych dat
        virtual void        encode (Environment* env, Shared_data data) = 0;
        // dekoduje podle predaneho neinicializovane sablony (resp. vyska, sirka
        virtual Shared_data decode (Environment* env, Shared_data data) = 0;

protected:
        Entropicky_koder (OZNACENI_PASMA pasmo): pasmo(pasmo) {}
        virtual ~Entropicky_koder () {}

        // o jake pasmo se jedna (napr. pro statistiku)
        OZNACENI_PASMA pasmo;
};

// inicializace entropickeho koderu
std::shared_ptr<Entropicky_koder> 
    get_entropicky_koder (Environment*, OZNACENI_PASMA pasmo);


void init_all_entropy_coders ();











#endif // ENT_HEADER_INC
