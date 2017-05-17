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
#ifndef MY_STD_MACRO_HEADER_INCLUDED
#define MY_STD_MACRO_HEADER_INCLUDED

#include <cassert>

#define MIN(X,Y) ( ((X)<(Y))?(X):(Y) )
#define MAX(X,Y) ( ((X)<(Y))?(Y):(X) )
#define ABS(X)   ( ((X)<0)?(-X):(X)  )


#endif // MY_STD_MACRO_HEADER_INCLUDED
