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

#include <memory>
#include <cassert>
#include <map>
#include <vector>

#include "aritmeticky.h"
#include "std_macro.h"
#include "entropicky_koder.h"

#include "aritm_sign32_256e.h"
#include "model_256e.h"
#include "model_sign.h"

template class Valxe_signx<SignModel::Type::T32, ValueModel::Type::T256>;
template class Valxe_signx<SignModel::Type::T32, ValueModel::Type::T512>;
template class Valxe_signx<SignModel::Type::T32, ValueModel::Type::T1024>;

template<SignModel::Type st, ValueModel::Type vt>
void Valxe_signx<st, vt>::encode (Environment* env, Shared_data data) {
    assert( data->rozmer_x * data->rozmer_y == static_cast<int>(data->data.size()) );

    const auto & d = data->data;
    Aritmeticky_koder arm ( env );

    // sign pass
    sign_model->encode( env, arm, data, pasmo );  

    const unsigned E_VALUE = value_model->get_e_value();


    for ( unsigned i = 0; i < d.size(); ++i ) {
        // odstraneni znamenka
        unsigned vzorek = static_cast<unsigned>(ABS(d[i]));

        do {
            unsigned int kodovany = vzorek & ( (1 << value_model->get_bits()) - 1 );
            vzorek >>= value_model->get_bits();

            value_model->encode( arm, kodovany );

            if ( vzorek ) { // pridavam znak pro pokracovani
                value_model->encode( arm, E_VALUE);
            }

        } while ( vzorek );
    }

    value_model->encode( arm, 0 ); // posledni extra hodnota 
    arm.EncodeFinish();
}

template<SignModel::Type st, ValueModel::Type vt>
Shared_data Valxe_signx<st, vt>::decode (Environment* env, Shared_data data) {
    assert( data->rozmer_x * data->rozmer_y == static_cast<int>(data->data.size()) );
    const int rozmer_x = data->rozmer_x, rozmer_y = data->rozmer_y;

    const auto & d = data->data;
    Aritmeticky_koder arm ( env );
    arm.DecodeStart();
    
    std::vector<bool> sign_pass = sign_model->decode(env, arm, data, pasmo);

    bool last_e = false;
    int posun = 0;
    int i = 0;

    const unsigned E_VALUE = value_model->get_e_value();

    while (true) {
        unsigned symbol = value_model->decode( arm );

        // nulovani posunu jen kdyz predtim nebylo E nebo tet neni E
        if ( symbol != E_VALUE && not last_e ) {
            posun = 0;
        }

        // Dalsi hodnota bude upgrade
        if ( symbol == E_VALUE ) {
            i -= 1;
            posun += value_model->get_bits();
            last_e = true;
        }
        else {
            if ( i >= rozmer_x * rozmer_y ) 
                break;

            // provede ulozeni
            if ( posun == 0 ) 
                data->data[i] = symbol;
            else {
                data->data[i] += symbol << posun;
            }
            
            i += 1;
            last_e = false;
        }
    }

    // sign pass
    assert( static_cast<unsigned>(rozmer_x * rozmer_y) == data->data.size() );
    assert( static_cast<unsigned>(rozmer_x * rozmer_y) == sign_pass.size() );

    for ( unsigned i = 0; i < static_cast<unsigned>(rozmer_x * rozmer_y); ++i )
        if ( sign_pass[i] )
            data->data[i] = - data->data[i];

    return data;
}






