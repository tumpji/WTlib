/*
 *  This file is part of WTlib.
 *
 *  WTlib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  WTlib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with WTlib.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef VLNKA_HEADER_INCLUDED
#define VLNKA_HEADER_INCLUDED

#include <memory>
#include "main.h"

// vlnka - definuje operaci predict a update
// pouze pro linou vlnkovou transformaci
class Vlnka {
public:
	Vlnka () {}
	virtual ~Vlnka () {}

	// pocet operaci P & U pro 1x data
	// pro 1x predict + 1x update vraci cislo 1 (napr. CDF 5/3 - 1)
	// (CDF 9/7 - 2), nezalezi na smeru
	virtual int get_number_lifting_steps () = 0;

	// pro predict
	virtual void lifting_predict(
			std::vector<int>& data, // data
			int index_step, // index predictu (podle get_number_lifting_steps)
			bool reversed , // jde o rekonstrukci == true
			bool divide_and_round, // viz update
			int start,      // hranice nejmensiho indexu (validni napr 0)
			int act,        // prvni meneny index (u 1DWT je to 1)
			int stop ,      // posledni validni (nemusi byt meneny)
			int step ) = 0; // krok o jeden vzorek (lina WT se posouva o 2xstep)

	// podobna pro update vse plati podobne jako u predict
	virtual void lifting_update(
			std::vector<int>& data,
			int index_step,
			bool reversed,
			bool divide_and_round, // rozsireni pro RB (jinak == true)
			int start,
			int act,
			int stop,
			int step ) = 0;

	typedef void (Vlnka::*Lift_ptr)(std::vector<int>&, int, bool, bool, int, int, int, int);

	// pro RB - vraci o kolik by normalne onen stupen posunul hodnotu
	virtual int predict_left_shift (int index_step) = 0;
	virtual int update_left_shift (int index_step)  = 0;
};


// vytvari a preda konkretni vlnku podle zadani uzivatele
std::shared_ptr<Vlnka> get_vlnka (Environment* env);


#endif // header

