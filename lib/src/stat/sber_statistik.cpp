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

#include <iostream>
#include <map>
#include <vector>
#include <mutex>

#include "wt_inter.h"
#include "entropicky_koder.h"
#include "sber_statistik.h"

namespace {
    std::map<WTlib::OZNACENI_PASMA, int> mapa_velikosti_pasmo;
    std::vector<int> vec_velikosti_uroven;

    std::mutex mutex;
}

void stat::vlozit_velikost_zakodovaneho_pasma (
        Environment* env,
        WTlib::OZNACENI_PASMA pasmo,
        int velikost )
{
    using PASMO = WTlib::OZNACENI_PASMA;
    (void)env;

    ::mutex.lock();

    // samotne pridavani
    auto s = mapa_velikosti_pasmo.find(pasmo);
    if ( s == mapa_velikosti_pasmo.end() )
        mapa_velikosti_pasmo[pasmo] = velikost;
    else 
        s->second += velikost;


    // ----------- vec_velikosti_uroven --------------

    /*
    // lze nahradit hystorii
    //static PASMO last_pasmo = PASMO::Last;
    static int aktualni_uroven = 0;

    if ( pasmo == PASMO::YELLOW || pasmo == PASMO::LH )
        aktualni_uroven += 1;

    // samotne pridavani
    if ( vec_velikosti_uroven.size() < static_cast<unsigned>(aktualni_uroven) + 1 )
        vec_velikosti_uroven.push_back(velikost);
    else
        *vec_velikosti_uroven.rbegin() += velikost;
        */

    ::mutex.unlock();
    return;
}


void stat::tisk_statistiky (Environment* env)
{
    (void)env;
    using std::cerr; using std::endl;

    for ( const auto pasmo : mapa_velikosti_pasmo ) {
        cerr << "#0  " << to_string(pasmo.first) << ":" << pasmo.second << endl;
    }

    /*
    for ( unsigned i = 0; i < vec_velikosti_uroven.size(); ++i ) {
        cerr << "#1  " << i << ":" << vec_velikosti_uroven[i] << endl; 
    }
    */
}
