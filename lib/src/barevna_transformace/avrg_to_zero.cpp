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
#include "avrg_to_zero.h"


int provest_pasmo_avrg_to_zero ( Shared_data data ) {
    int64_t suma_data = 0;
    int64_t pocet = 0;
    for ( const auto & i : data->data ) {
        suma_data += i;
        pocet += 1;
    }

    int shift = static_cast<int>( suma_data / pocet );

    for ( auto & i : data->data ) {
        i -= shift;
    }
    return shift;
}

void opravit_pasmo_avrg_to_zero ( Shared_data data, int shift ) {
    if ( shift == 0 ) return;
    for ( auto & i : data->data ) {
        i += shift;
    }
}

