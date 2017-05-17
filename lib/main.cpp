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
#include <cassert>

#include "wt_inter.h"
#include "main.h"

#include "dekompozice.h"

#include "sber_statistik.h"

static_assert( sizeof(int) == 4, "Upravit kontrolni integer" );
#define CONTROL_INTEGER 0xa7c4f58d


#define SET_UP_U8(X) \
    static_assert(sizeof(X) == 1, "nutnost upravit ukladani pro " #X );\
    env->get_set_u8(*reinterpret_cast<unsigned char*>(&X));
#define SET_UP_INT(X) \
    static_assert(sizeof(X) == sizeof(int), "nutnost upravit ukladani pro " #X );\
    env->get_set_int(*reinterpret_cast<int*>(&X));

#define SET_UPE_U8(X) SET_UP_U8(nastaveni->X)
#define SET_UPE_INT(X) SET_UP_INT(nastaveni->X)

namespace WTlib {

// uklada/nacita  nastaveni (ve WTlib::Common)
// informace o velikosti pasem atd.. do souboru a ze soboru
void set_up_settings (Environment* env) {
    auto &nastaveni = env->nastaveni;

    // KONTROLA
    int control_integer = CONTROL_INTEGER;  
    SET_UP_INT( control_integer );
    if ( control_integer != static_cast<int>(CONTROL_INTEGER) )
        throw std::invalid_argument("Toto neni soubor rozkodavatelny touto knihovnou");

    SET_UPE_U8( barevny_prostor );

    unsigned char tmp = env->nastaveni->avrg_zero ? 1 : 0;
    SET_UP_U8( tmp );
    env->nastaveni->avrg_zero = tmp ? true : false;

    SET_UPE_U8( vlnka );

    SET_UPE_U8( dekompozice );

    SET_UPE_INT( dekompozice_max );
    SET_UPE_INT( dekompozice_min_rozmer );
    SET_UPE_INT( dekompozice_side_level );

    SET_UPE_U8( prediktor_pred );
    SET_UPE_U8( koder );

    // zacatek kodovani prediktoru na pasma
    {
        unsigned char pocet_prediktor_pasem = 
            static_cast<unsigned char>(nastaveni->prediktor_pasmo.size());
        SET_UP_U8(pocet_prediktor_pasem);

        if ( nastaveni->prediktor_pasmo.size() )  // koder koduje 
            for ( const auto p : nastaveni->prediktor_pasmo ) {
                unsigned char pasmo = static_cast<unsigned char>(p.first), 
                              prediktor = static_cast<unsigned char>(p.second);
                SET_UP_U8(pasmo);
                SET_UP_U8(prediktor);
            }
        else if ( pocet_prediktor_pasem ) { // dekoder dekoduje
            for ( unsigned i = 0; i < pocet_prediktor_pasem; ++i )
            {
                unsigned char pasmo, prediktor;
                SET_UP_U8(pasmo); SET_UP_U8(prediktor);
                nastaveni->prediktor_pasmo[ WTlib::byte_to_OZNACENI_PASMA(pasmo) ] = 
                    WTlib::byte_to_PREDIKTOR(prediktor);
            }
        }
    }

    // zacatek kodovani vyjimek z kodovani pasem
    {
        unsigned char pocet_vyjimek = 
            static_cast<unsigned char>(nastaveni->vyjimka_koder.size());
        SET_UP_U8(pocet_vyjimek);

        if ( nastaveni->vyjimka_koder.size() ) // koder koduje
            for ( const auto p : nastaveni->vyjimka_koder ) {
                unsigned char pasmo = static_cast<unsigned char>(p.first),
                              vyjimka = static_cast<unsigned char>(p.second);
                SET_UP_U8(pasmo);
                SET_UP_U8(vyjimka);
            }
        else if ( pocet_vyjimek ) { // dekoder
            for ( unsigned i = 0; i < pocet_vyjimek; ++i ) {
                unsigned char pasmo, vyjimka;
                SET_UP_U8(pasmo); SET_UP_U8(vyjimka);
                nastaveni->vyjimka_koder[ WTlib::byte_to_OZNACENI_PASMA(pasmo) ] =
                    WTlib::byte_to_KODOVACI_MOZNOST(vyjimka);

            }
        }
    }

    // zacatek kodovani velikosti jednotlivych pasem
    {
        int pocet_pasem = static_cast<int>(nastaveni->tmp_data_vec.size());
        SET_UP_INT(pocet_pasem);

        if ( nastaveni->tmp_data_vec.size() ) { // koduje se
            for ( unsigned ip = 0; ip < nastaveni->tmp_data_vec.size(); ++ip ) {
                auto& data = *nastaveni->tmp_data_vec[ip];
                assert( static_cast<unsigned>(data.rozmer_x * data.rozmer_y) == data.data.size() );

                SET_UP_INT(data.rozmer_x);
                SET_UP_INT(data.rozmer_y);
            }
        }
        else if ( pocet_pasem ) { // dekoduje se 
            for ( int ip = 0; ip < pocet_pasem; ++ip ) {
                int rozmer_x, rozmer_y;
                SET_UP_INT(rozmer_x); SET_UP_INT(rozmer_y);

                Shared_data dt = std::make_shared<WTdata>( rozmer_x, rozmer_y, 
                        WTdata::Allocation_options::DO_NOT_ALLOCATE );
                nastaveni->tmp_data_vec.push_back(dt);
            }
        }
        else 
            throw std::invalid_argument("Empty compression");
    }

    return;
} 



} // end WTlib


// ************************************************
// WTlib

WTcommon::WTcommon () : 
    barevny_prostor(imp_barevny_postor), 
    avrg_zero(imp_avrg_zero),
    vlnka(imp_vlnka), 
    dekompozice(imp_dekompozice), 
    dekompozice_max(imp_dekompozice_max), 
    dekompozice_min_rozmer(imp_dekompozice_min_rozmer),
    dekompozice_side_level(imp_dekompozice_side_level),
    prediktor_pred(imp_prediktor_puvodni_data),
    koder(imp_koder),
    verbose(false)
{
}


///////////////////////////////////////////////////
//// encoder

WTencoder::WTencoder () : WTcommon() 
{}
WTencoder::WTencoder (const WTcommon& copy) : WTcommon(copy) 
{}

void WTencoder::vlozit_pasmo (WTdata kopie) {
    if ( kopie.data.size() != static_cast<unsigned>(kopie.rozmer_x * kopie.rozmer_y) )
        throw std::invalid_argument("Data missmatch");
    tmp_data_vec.push_back( std::make_shared<WTdata>(kopie) );
}

void WTencoder::start (std::string cesta) {

    // vsechny kontroly idealne zde
    if ( not  cesta.size() )
        throw std::invalid_argument("Path is not set");

    if ( not tmp_data_vec.size() )
        throw std::invalid_argument("Empty compression");

    if ( dekompozice_max < 0 )
        throw std::invalid_argument("Negative number of decomposition is not allowed");

    if ( dekompozice_min_rozmer < 2 )
        throw std::invalid_argument("Minimal decomposing size must be at least 2");

    if ( dekompozice_side_level < 0 )
        throw std::invalid_argument("Side level must be positive or zero.");

    init_all_entropy_coders();
    Environment env(this, cesta);

    set_up_settings(&env);

    env.start();

    if ( verbose ) {
        stat::tisk_statistiky(&env);
    }
}



////////////////////////////////////////////////
///////////// decoder
WTdecoder::WTdecoder () : WTcommon () 
{}

std::vector<WTlib::WTdata> WTdecoder::start (std::string cesta) {
    // kontroly
    if ( not  cesta.size() )
        throw std::invalid_argument("Path is not set");


    // otevirani souboru
    init_all_entropy_coders();
    Environment env (this, cesta);

    set_up_settings(&env);
    env.start();

    // vyndat hodnoty do vystupu z dekomprese
    std::vector<WTdata> vystup;

    for ( auto& d : tmp_data_vec ) {
        WTdata x (d->rozmer_x, d->rozmer_y, WTdata::Allocation_options::DO_NOT_ALLOCATE );
        x.data = std::move(d->data);
        vystup.push_back( std::move(x) );
    }

    return vystup;
}


