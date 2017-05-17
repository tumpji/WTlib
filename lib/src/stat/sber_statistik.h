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
#ifndef SBER_STATISTIK_HEADER_INCLUDED
#define SBER_STATISTIK_HEADER_INCLUDED
#include "main.h"
#include "entropicky_koder.h"


namespace stat {

void vlozit_velikost_zakodovaneho_pasma(
       Environment* env,
       OZNACENI_PASMA pasmo,
       int velikost );     

// human-machine readeble stat. o pasmech v cerr
// #0 <jemno>:<cislo>            
//      oznacuje celkovou velikost pasma (<cislo>B) pojmenovaneho <jemno>
// #1 <cislo>:<cislo2>
//      oznacuje celkovou velikost urovne (<cislo2>B) urovne <cislo> (0 = LL/B/...)
void tisk_statistiky (Environment* env);

}


#endif // SBER_STATISTIK_HEADER_INCLUDED
