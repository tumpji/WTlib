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
#include "vlnka_internal.h"

namespace HAAR {

void Vlnka::lifting_predict( ROZHRANI_VLNKOVE_FUNKCE_PLNE ) {
    assert( (stop-start)%step == 0 );
    assert( (act-start )%step == 0 );
    assert(index_step < 2);

    for ( int i = act;   i <= stop;   i += step * 2) {
        int value = ROZSIRENI(i-step);
        if ( divide_and_round )
            if ( reversed )
                data[i] += value;
            else 
                data[i] -= value;
        else 
            data[i] += value;
    }
}

void Vlnka::lifting_update( ROZHRANI_VLNKOVE_FUNKCE_PLNE ) {
    assert( (stop-start)%step == 0 );
    assert( (act-start )%step == 0 );
    assert(index_step < 2);

    for ( int i = act;   i <= stop;   i += step * 2) {
        int value = ROZSIRENI(i+step);
        value -= 1;

        if ( divide_and_round )
            if ( reversed )
                data[i] -= value >> 1;
            else 
                data[i] += value >> 1;
        else 
            data[i] += value;
    }
}


}
