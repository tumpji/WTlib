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
#include "entropicky_koder.h"

// pouze uklada integery do souboru
// nedela zadne entropicke kodovani
class Ent_NIC : public Entropicky_koder {
public:
		Ent_NIC (OZNACENI_PASMA pasmo): Entropicky_koder(pasmo) {}
		virtual ~Ent_NIC () {}
		
		// koduje data podle predanych dat
		virtual void		encode (Environment* env, Shared_data data);
		// dekoduje podle predaneho neinicializovane sablony (resp. vyska, sirka
		virtual Shared_data decode (Environment* env, Shared_data data);

protected:
		// seznam vnitrnich stavu
};
