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
#include "zpracovani_pasma.h"
#include "entropicky_koder.h"
#include "predictor.h"
#include "avrg_to_zero.h"

using WTlib::OZNACENI_PASMA;

class Zpracovat_hotove_pasmo;





// pomocna trida pro volani entropickeho koderu
class Zpracovat_hotove_pasmo : public Command {
public:
    Zpracovat_hotove_pasmo (OZNACENI_PASMA i): pasmo(i) {}
    ~Zpracovat_hotove_pasmo () {}

    virtual void call (Environment* env);

protected:
    OZNACENI_PASMA pasmo;

private:
    // pro komprese
    Shared_data     get_data_to_compress (Environment*);
    // pro dekomprese
    Shared_data     get_data_to_fill (Environment*);
    void            push_filled_data (Environment*, Shared_data);
};


/////////////// out ////////////

// vlozi na zasobnik stavu pozadavek na zakodovani/dekodovani pasma
void push_zpracovat_hotove_pasmo ( Environment* env, OZNACENI_PASMA pasmo ) {
    env->push_command( std::make_shared<Zpracovat_hotove_pasmo>(pasmo) );
}

///////////////////////////////
//
//

void Zpracovat_hotove_pasmo::call (Environment* env) {

    auto entropicky_koder = get_entropicky_koder(env, pasmo);
    auto prediktor_na_pasmo = get_predictor(env, pasmo);
    Shared_data   data;

    int avrg = 0;

    //bool avrg_zero_pripraven = env->nastaveni->avrg_zero;
    bool avrg_zero_pripraven = env->nastaveni->avrg_zero && 
        ( pasmo == OZNACENI_PASMA::LL || pasmo == OZNACENI_PASMA::BLUE );

    switch (env->smer) {
        case SMER::KODER:
            data = get_data_to_compress(env);

            if ( avrg_zero_pripraven ) {
                avrg = provest_pasmo_avrg_to_zero(data);
                env->put_int(avrg);
            }

            prediktor_na_pasmo->forward_predict(data);
            entropicky_koder->encode(env, data);
            break;

        case SMER::DEKODER:
            if ( avrg_zero_pripraven ) {
                avrg = env->get_int();
            }

            data = get_data_to_fill(env);
            data = entropicky_koder->decode(env, data);
            prediktor_na_pasmo->reverse_predict(data);

            if ( avrg_zero_pripraven ) {
                opravit_pasmo_avrg_to_zero( data, avrg );
            }

            push_filled_data(env, data);
            break;
    }
}






////////////////////////////////
//// manipulace s stackem //////

// komprese
Shared_data Zpracovat_hotove_pasmo::get_data_to_compress (Environment* env) {
    auto tmp = env->data_stack.top();
    env->data_stack.pop();
    assert(tmp->data.size());
    return tmp;
}
// dekomprese
Shared_data Zpracovat_hotove_pasmo::get_data_to_fill (Environment* env) {
    auto tmp = env->data_stack.top();
    env->data_stack.pop();
    assert(tmp->rozmer_x && tmp->rozmer_y);
    tmp->reserve(); // vytvorit misto
    return tmp;
}
void Zpracovat_hotove_pasmo::push_filled_data (Environment* env, Shared_data data) {
    env->data_stack_recon.push(data);
}
