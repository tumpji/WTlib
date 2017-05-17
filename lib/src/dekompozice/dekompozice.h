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
#ifndef WT_DEKOMPOZICE_HEADER_INCLUDED
#define WT_DEKOMPOZICE_HEADER_INCLUDED

#include <memory>
#include <stdexcept>

#include "wt.h"
#include "main.h"

// vraci prikaz k vykonani dekompozice
Shared_command   vyber_dekompozice   (Environment* env);


// pro generovani vlny
//void RB_obrazek ( Environment * env );


#endif // WT_DEKOMPOZICE_HEADER_INCLUDED
