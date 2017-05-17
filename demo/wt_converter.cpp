#include <iostream>

#include "wt.h"
#include "main.h"
#include "wt_converter.h"
#include "pics_converter.h"

void spustit_koder( Parse_opts opts, std::string cesta,  Obrazek obrazek  ) {
	// vytvarim instanci encoder 
	// pridavam predem nastavene hodnoty
	WTlib::WTencoder encoder (opts.metoda); 

	// vkladani dat
	for ( const auto& d : obrazek )
		encoder.vlozit_pasmo(std::move(d));

	// spusteni koderu
	encoder.start(cesta); // TODO

	return; // hotovo ...
}

Obrazek spustit_dekoder ( std::string cesta ) 
{
	WTlib::WTdecoder dec; // instance dekoderu (nastaveni se nevklada)

	return  dec.start(cesta);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// TEST


WTlib::WTdata get_test_pasmo (int x, int y, int max) 
{
	WTlib::WTdata pasmo (x,y);

	for ( int i = 0; i < x * y; ++i ) {
		pasmo.data[i] = rand() % max;
	}

	return pasmo;
}

static Obrazek load_test (int x, int y, int pasma) {
	Obrazek vec;

	for ( int pn = 0; pn < pasma; ++pn ) {
		vec.push_back( get_test_pasmo(x,y, 256) );
	}

	return vec;
}

#if defined(WIND32) || defined(_WIN32) || defined(__WIN32)
	#define RED(X) X
	#define GREEN(X) X
#else
	#define RED(X) "\e[1;31m" X "\e[0m"
	#define GREEN(X) "\e[1;32m" X "\e[0m"
#endif


static void spustit_test ( Parse_opts opts, int width, int height, int bp=1 ) {
	using std::cerr; 
	using std::endl;

	cerr << "Starting test " << width << "x" << height << "x" << bp;

	auto test_original_data = load_test(width, height, bp);

	spustit_koder(opts, "wt_test_data.wt", test_original_data);
	auto vystup = spustit_dekoder("wt_test_data.wt");


	if ( check_obrazek(test_original_data, vystup) ) {
		cerr << RED("\tERROR") << endl;
	}
	else {
		cerr << GREEN("\tOK") << endl;
	}
}


void spustit_test ( Parse_opts opts ) {

	spustit_test(opts, 33,33);
	spustit_test(opts, 5,1);

	spustit_test(opts, 1, 1);
	spustit_test(opts, 100, 1);
	spustit_test(opts,   2,2 );
	spustit_test(opts, 100, 5);
	spustit_test(opts, 1, 100);
	spustit_test(opts, 5, 100);
	spustit_test(opts, 32, 32);

	for ( int x = 1; x < 50; x += 5 )
	for ( int y = 1; y < 50; y += 5 )
		spustit_test(opts, x, y, 5 );

	spustit_test(opts, 2048, 2048, 3);
}
