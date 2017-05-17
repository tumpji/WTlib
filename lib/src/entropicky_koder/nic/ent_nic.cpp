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

#include "ent_nic.h"
#include <iostream>

#include "sber_statistik.h"


void Ent_NIC::encode (Environment* env, Shared_data data)
{
    int dt_length = data->rozmer_x * data->rozmer_y;
    assert(data->data.size() == static_cast<unsigned>(dt_length));


    std::cerr << "Vkladam " << data->rozmer_x << "x" << data->rozmer_y << "\t"
       << WTlib::to_string(pasmo)   << std::endl;

    // vlozi do souboru vsechny
    for (int i = 0; i < dt_length; ++i ) {
        int nacteny = data->data[i];
        env->get_set_int(nacteny);
    }

    stat::vlozit_velikost_zakodovaneho_pasma(env, pasmo, dt_length * sizeof(int));
}

Shared_data Ent_NIC::decode (Environment* env, Shared_data data)
{
    int dt_length = data->rozmer_x * data->rozmer_y;

    std::cerr << "Nacitam " << data->rozmer_x << "x" << data->rozmer_y << "\t"
        << WTlib::to_string(pasmo) << std::endl;

    for (int i = 0; i < dt_length; ++i) {
        int nacteny; 
        env->get_set_int(nacteny);
        data->data[i] = nacteny;
    }

    return data;
}
