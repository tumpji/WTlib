#include <cstdint>

#include "aritmeticky.h"
#include "model_sign.h"
#include "model_256e.h"


class BitPlane {
	BitPlane (int rozmer_x, int rozmer_y) : rozmer_x(rozmer_x), rozmer_y(rozmer_y) {
		data.resize( rozmer_x*rozmer_y, false);
		suma_sloupcu.resize( rozmer_x, 0 );
		suma_radku.resize( rozmer_y, 0 );
	}	

	void spocitat_sumy ( ) {
		unsigned index = 0;
		for ( unsigned y = 0; y < rozmer_y; ++y )
		for ( unsigned x = 0; x < rozmer_x; ++x )
		{

			index += 1;
		}
	}

private:
	std::vector<bool> data;
	std::vector<unsigned> suma_sloupcu, suma_radku; 
};

void zjistit_


class Pruchod {
	Pruchod ( 

};
