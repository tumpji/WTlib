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

#include "main.h"

// koduje obrazek podle predanych parametru
void spustit_koder ( 
        Parse_opts      opts, 
        std::string     cesta,
        Obrazek         obrazek );

// dekoduje obrazek
Obrazek spustit_dekoder ( std::string cesta );




// spusti jednoduchy test
void spustit_test ( Parse_opts opts );

