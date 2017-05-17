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
#ifndef WT_DEKOMPOZICE_INTERNAL_HEADER_INCLUDED
#define WT_DEKOMPOZICE_INTERNAL_HEADER_INCLUDED

#include "dekompozice.h"
#include "main.h"
#include "wt_inter.h"

namespace Dekompozice {

class Dek_level_cut: public Command {
    public:
        Dek_level_cut(int level): level_rozlozeni(level) {}
        ~Dek_level_cut() {}

        virtual void call (Environment*) = 0;

    protected:
        int level_rozlozeni;
};

/***********************************
 * Klasicka tenzor. pyramidova dekompozice
 * Predava se pocet
 */
class Dek_pyramidova: public Dek_level_cut {
    public:
        Dek_pyramidova(int level): Dek_level_cut(level)
        { oznaceni = "Dekompozice pyr. "; }

        virtual void call (Environment*); 
};

/************************************
 * Cerveno cerna dekompozice
 */
class Dek_red_black: public Dek_level_cut {
    public:
        Dek_red_black(int level, OZNACENI_PASMA pasmo = OZNACENI_PASMA::BLUE): Dek_level_cut(level), pasmo(pasmo)
        { oznaceni = "Dekompozice red black "; }

        virtual void call (Environment*);
    protected:
        OZNACENI_PASMA pasmo;
};

} // end namespace dekompozice




#endif // WT_DEKOMPOZICE_INTERNAL_HEADER_INCLUDED
