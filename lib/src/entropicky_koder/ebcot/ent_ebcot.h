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
#include "entropicky_koder.h"
#include "mqc.h"
#include "wt_inter.h"



// obecna konstrukce entropickeho koderu
class Ent_ebcot : public Entropicky_koder {
public:
		Ent_ebcot (OZNACENI_PASMA pasmo, KODOVACI_MOZNOST kodovat_jako_pasmo);
		virtual ~Ent_ebcot () {}
		
		// koduje data podle predanych dat
		virtual void		encode (Environment*, Shared_data);
		// dekoduje podle predaneho neinicializovane sablony (resp. vyska, sirka
		virtual Shared_data decode (Environment*, Shared_data);

protected:
		//MqcState mqc;
		KODOVACI_MOZNOST kodovat_jako_pasmo;
};
