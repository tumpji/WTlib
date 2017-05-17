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
#include <iostream> // tisk 
#include <string>
#include <vector>

// boost lib for parsing args
#include <boost/program_options.hpp>
namespace po = boost::program_options;


#include "main.h"


// IO WTlib
#include "wt.h"
#include "wt_converter.h"

// Load and save pics
#include "pics_converter.h"


using std::cerr; 
using std::cout; 
using std::endl;
using WTlib::to_string;


int parse_options (int argc, char** argv, Parse_opts& opts, 
		std::string& vstupni_soubor, std::string& vystupni_soubor );








/////////////////////////////////////
int main (int argc, char** argv) {
	Parse_opts		opts;
	std::string		vstupni_soubor, 
					vystupni_soubor;

	// args. parse
	if ( parse_options(argc, argv, opts, vstupni_soubor, vystupni_soubor) ) {
		std::cerr << "Error occured during parsing options..." << std::endl;
		return 1;
	}
	
	if ( opts.test_switch ) { // test
		cerr << opts.metoda;
		spustit_test(opts);
	}
	else if ( opts.opensave_switch ) { // open save
		auto tmp = load_obrazek(vstupni_soubor);
		save_obrazek(vystupni_soubor, tmp);
	}
	else if ( not opts.decoder_switch ) { // coder
		cerr << "Coding picture \""  << vstupni_soubor 
			 << "\"  to file \"" << vystupni_soubor << "\""	<< endl;
		cerr << opts.metoda;

		auto tmp = load_obrazek(vstupni_soubor);
		spustit_koder( opts, vystupni_soubor, tmp );

		if ( opts.check_switch ) {
			auto tmp_d = spustit_dekoder(vystupni_soubor);

			if ( check_obrazek(tmp, tmp_d) ) {
				cerr << "Check ERROR !!!" << endl;
				throw;
			}
			else 
				cerr << "Checking succeeded ..." << endl;
		}
	} 
	else { // decoder
		cerr << "Dekodovani "  << vstupni_soubor 
			 << "  do " << vystupni_soubor << endl;

		auto tmp = spustit_dekoder(vstupni_soubor);
		save_obrazek(vystupni_soubor, tmp);
	}

	std::cerr << "All done ..." << std::endl;
	return 0;
}




////////////////////////////////////////////////////////////////////////////////
//******************************************************************************
//..............................................................................
// parsing program args

static unsigned char char_to_cislo ( char p ) {
#define MEZI(A,X,Y) ( ((A)>=(X)) and ((A)<=(Y)) )
	if ( MEZI(p, '0', '9') ) {
		return p - '0';
	} else if ( MEZI(p, 'A', 'Z') ) {
		return p - 'A' + 10;
	} else if ( MEZI(p, 'a', 'z') ) {
		return p - 'a' + 10;
	} else {
		cerr << "Cannot parse char '" << p << "' (prediktor)" << endl;
		exit(1);
	}
#undef MEZI
}

static char cislo_to_char ( unsigned char c ) {
	if ( c < 10 ) 
		return c + '0';
	else 
		return c - 10 + 'A';
}

// generates options
template<typename T>
std::string  provide_options () {
	std::string tmp = "\n";
	for (unsigned char i = 0;
		 i <= static_cast<unsigned char>(T::Last);
		 i++)
	{
		auto m = std::to_string(static_cast<unsigned>(i));
		m.resize(3,' ');
		tmp += "  " + m + "-> " + WTlib::to_string(static_cast<T>(i)) + "\n";
	}
	return tmp;
}

// generate options for predictor 1,2,3 .. , 9, A, B...
template<>
std::string  provide_options<WTlib::PREDIKTOR> () {
	std::string tmp = "\n";
	for (unsigned char i = 0;
		 i <= static_cast<unsigned char>(WTlib::PREDIKTOR::Last);
		 i++)
	{
		std::string m;
		m += cislo_to_char(static_cast<unsigned char>(i));
		m.resize(3,' ');
		tmp += "  " + m + "-> ";
	  	tmp += WTlib::to_string(static_cast<WTlib::PREDIKTOR>(i));
		tmp += "\n";
	}
	return tmp;
}


std::string create_code_except_guide () {
	using namespace WTlib;
	std::string vystup = 
		" Alters default behaviour of ent. coder \n"
		" To modify coder for each band: \n"
		"  set characters that represent coder behaviour\n"
	    "  in position dependant manner in string like: \n"
	    "  \"0123\" use beh. num. 0 on band number 0 etc. \n"
		" Positions: \n"
	    "  position 1) represents LL/Blue band \n"	
	    "  position 2) represents LL/Yellow band \n"	
	    "  position 3) represents LL/Black-blue band \n"	
	    "  position 4) represents HH/Black-yellow band\n"	
		"  longer strings are not allowed ... \n"
		" Encoder behaviours: \n"
		;
	for ( unsigned char i = 0; i <= static_cast<unsigned char>(KODOVACI_MOZNOST::Last); ++i )
	{
		char x[2] = {};
		x[0] = cislo_to_char(i);
		vystup += "  " + std::string(&x[0]) + "  -> ";
		vystup += to_string(static_cast<WTlib::KODOVACI_MOZNOST>(i));
		vystup += "\n";
	}
	return vystup;
}
		 

#define AVAILABLE(R,X) (R + provide_options<X>() ).c_str()

int parse_options (int argc, char** argv, Parse_opts& opts, 
		std::string& vstupni_soubor, std::string& vystupni_soubor) {
	po::options_description desc ("Allowed options");

#define RETEZEC po::value<std::string>()
#define CISLO   po::value<unsigned>()

	desc.add_options() 
        ("help,h"                   , "produce help message\n")
        ("input,i"         ,RETEZEC	, "provide source/output file")
        ("output,o"        ,RETEZEC	, "provide source/output file")
        ("verbose"        ,           "get extra info")

        ("decompress"               , "decompression mode\n")
		("opensave"                 , "only open and save picture (no compression)")
		("check"                    , "check by opening compressed file")
		("test"                     , "test compression")

		("color,c",  CISLO, AVAILABLE("color transform used (compression)", WTlib::BAREVNY_PROSTOR) )
		("avrg_zero",  "set average to zero" )
		("predictor,p",   RETEZEC, AVAILABLE("predictor that is used before wt", WTlib::PREDIKTOR) )

        ("wavelet,w" ,CISLO, AVAILABLE("lifting method (only compression)",  WTlib::VLNKA) )

        ("decomposition,d", CISLO, AVAILABLE("decompress input file", WTlib::DEKOMPOZICE) )
        ("dec_max_level" ,CISLO, "maximum decomposition level")
        ("dec_min_size" ,CISLO, "minimum size to enable next level of decomposition\n")
        ("dec_side_level" ,CISLO, "further docomposition of highpass bands (viz doc)")

        ("postpredict"        ,RETEZEC	, "enables predict on every band\n"
										  "  1) with Pyramidal dek. => LL,LH,HL,HH\n"
										  "  2) Red-Black dek. => Blue,Y,Black-Blue,B-Y\n"
										  " examples :\n"
										  "   \"0000\" : disable this option (default)\n"
										  "   \"1\"    : use on LL/Blue 1. predictor\n"
										  "   \"022\"   : similar HL/LH/Y/BB and 2. predictor\n"
										  )

		("coder,e"     ,CISLO, AVAILABLE("default entropy coder behaviour used", WTlib::KODER) )
		("code_alt" , RETEZEC, create_code_except_guide().c_str() )
		;

	po::variables_map vm;
	try {
	po::store(po::parse_command_line(argc, argv, desc), vm);
	} 
	catch (po::error const& e) {
		cerr << e.what() << endl;
		return 1;
	}
	po::notify(vm);

	// HELP
	if ( vm.count("help") ) {
		cout << desc << endl;
		exit(0);
	}

	if ( vm.count("verbose") ) {
		opts.metoda.verbose = true;
	}

	// INPUT
	if (vm.count("input")) {
		if ( vm.count("test") )
			cerr << "Input is ignored" << endl;
		vstupni_soubor= vm["input"].as<std::string>();
	} else if ( not vm.count("test") ) {
		cerr << "Input is not set" << endl;
		return 1;
	}

	// OUTPUT
	if ( vm.count("output") ) {
		vystupni_soubor = vm["output"].as<std::string>();
	} else {
		if ( vm.count("test") ) vystupni_soubor = "test_output.wt";
		else					vystupni_soubor = vstupni_soubor + ".wt";

		cerr << "Output is automaticly set to \"" << vstupni_soubor << ".wt\"" << endl;
	}

	// DECOMPRESS
	opts.decoder_switch = false;
	if ( vm.count("decompress") && (vm.count("opensave") || vm.count("test") || vm.count("check")) ){
		cerr << "Decompression and opensave, check or test is not supported" << endl;
		return 1;
	}
	else if ( vm.count("decompress") ) {
		opts.decoder_switch = true;
	}
	// MAX_LEVEL
	if (vm.count("dec_max_level")) {
		opts.metoda.dekompozice_max = vm["dec_max_level"].as<unsigned>();
	}
	// MIN SIZE
	if (vm.count("dec_min_size")) {
		opts.metoda.dekompozice_min_rozmer = vm["dec_min_size"].as<unsigned>();
	}
	// SIDE LEVEL
	if (vm.count("dec_side_level")) {
		opts.metoda.dekompozice_side_level = vm["dec_side_level"].as<unsigned>();
		}

	if (vm.count("avrg_zero")) {
		opts.metoda.avrg_zero = true;
	}

	// TEST
	if ( vm.count("test") && (vm.count("opensave") || vm.count("check")) ) {
		cerr << "Test is not supported with opensave and check" << endl;
		return 1;
	}
	else if ( vm.count("test") ) opts.test_switch = true;
	else opts.test_switch = false;

	// OPENSAVE
	if ( vm.count("opensave") && vm.count("check") ) {
		cerr << "Opensave with check is not supported" << endl;
		return 1;
	}
	else if ( vm.count("opensave") ) opts.opensave_switch = true; 
	else opts.opensave_switch = false;

	// CHECK
	opts.check_switch = false;
	if ( vm.count("check") ) { opts.check_switch = true; }


#define PARSE_HODNOTA(DESCSTR, WTLIBSTR, OPTSVAR) \
	if (vm.count(DESCSTR)) { \
		unsigned val = vm[DESCSTR].as<unsigned>(); \
		if ( opts.decoder_switch ) \
			cerr << #DESCSTR " setting is ignored" << endl; \
		else if ( vm.count("opensave") ) {\
			cerr << "Opensave with " #DESCSTR " is not supported" << endl;\
			return 1; \
		} else if ( val > 255) \
			throw std::invalid_argument( \
				"Prilis velka hodnota " + std::to_string(val) + " u " #WTLIBSTR );\
		else { \
			opts.metoda.OPTSVAR = WTlib::byte_to_##WTLIBSTR(static_cast<unsigned char>(val)); \
		}\
	}

	try {
		PARSE_HODNOTA("color", BAREVNY_PROSTOR, barevny_prostor);
		PARSE_HODNOTA("wavelet", VLNKA , vlnka);
		PARSE_HODNOTA("decomposition", DEKOMPOZICE, dekompozice);
		PARSE_HODNOTA("coder", KODER , koder);


		// specialni zachezeni s prediktorem (prevod znaku)
		if (vm.count("predictor")) { 
			std::string val = vm["predictor"].as<std::string>(); \

			if ( opts.decoder_switch )
				cerr << "predictor setting is ignored" << endl; 
			else if ( vm.count("opensave") ) {
				cerr << "Opensave with predictor is not supported" << endl;
				return 1; 
			} else if ( val.size() != 1 ) 
				throw std::invalid_argument( "Cannot handle predict value" );
			else { 
				opts.metoda.prediktor_pred = WTlib::byte_to_PREDIKTOR(char_to_cislo(*val.begin()));
			}
		}
	} catch (const std::invalid_argument& e) {
		cerr << e.what() << endl;
		return 1;
	}
#undef PARSE_HODNOTA


	// POSTPREDICT
	if ( vm.count("postpredict") ) {
		std::string vstup = vm["postpredict"].as<std::string>();
		auto t = vstup.rbegin();

		switch ( opts.metoda.dekompozice ) {
			case WTlib::DEKOMPOZICE::PYRAMIDOVA:
				switch ( vstup.size() ) {
					case 4:
						opts.metoda.prediktor_pasmo[ WTlib::OZNACENI_PASMA::HH ] =
							WTlib::byte_to_PREDIKTOR(char_to_cislo(*t++));
					case 3:
						opts.metoda.prediktor_pasmo[ WTlib::OZNACENI_PASMA::HL ] =
							WTlib::byte_to_PREDIKTOR(char_to_cislo(*t++));
					case 2:
						opts.metoda.prediktor_pasmo[ WTlib::OZNACENI_PASMA::LH ] =
							WTlib::byte_to_PREDIKTOR(char_to_cislo(*t++));
					case 1:
						opts.metoda.prediktor_pasmo[ WTlib::OZNACENI_PASMA::LL ] =
							WTlib::byte_to_PREDIKTOR(char_to_cislo(*t++));
					case 0:
						break;
					default:
						cerr << "Cannot handle more than 4 predictor (postpredict)" << endl;
						return 1;
				}
				break;
			case WTlib::DEKOMPOZICE::RED_BLACK:
				switch ( vstup.size() ) {
					case 4:
						opts.metoda.prediktor_pasmo[ WTlib::OZNACENI_PASMA::BLACK_YELLOW ] =
							WTlib::byte_to_PREDIKTOR(char_to_cislo(*t++));
					case 3:
						opts.metoda.prediktor_pasmo[ WTlib::OZNACENI_PASMA::BLACK_BLUE ] =
							WTlib::byte_to_PREDIKTOR(char_to_cislo(*t++));
					case 2:
						opts.metoda.prediktor_pasmo[ WTlib::OZNACENI_PASMA::YELLOW ] =
							WTlib::byte_to_PREDIKTOR(char_to_cislo(*t++));
					case 1:
						opts.metoda.prediktor_pasmo[ WTlib::OZNACENI_PASMA::BLUE ] =
							WTlib::byte_to_PREDIKTOR(char_to_cislo(*t++));
					case 0:
						break;
					default:
						cerr << "Cannot handle more than 4 predictor (postpredict)" << endl;
						return 1;
				}
				break;
			default:
				cerr << "Postpredict is not implemented for this decomposition" << endl;
				return 1;
		}
	}

	// CODEALT
	if ( vm.count("code_alt") ) {
		std::string vstup = vm["code_alt"].as<std::string>();
		auto t = vstup.rbegin();

		switch ( opts.metoda.dekompozice ) {
			case WTlib::DEKOMPOZICE::PYRAMIDOVA:
				switch ( vstup.size() ) {
					case 4:
						std::cout << *t << endl;
						opts.metoda.vyjimka_koder[ WTlib::OZNACENI_PASMA::HH ] =
							WTlib::byte_to_KODOVACI_MOZNOST(char_to_cislo(*t++));
					case 3:
						std::cout << *t << endl;
						opts.metoda.vyjimka_koder[ WTlib::OZNACENI_PASMA::HL ] =
							WTlib::byte_to_KODOVACI_MOZNOST(char_to_cislo(*t++));
					case 2:
						std::cout << *t << endl;
						opts.metoda.vyjimka_koder[ WTlib::OZNACENI_PASMA::LH ] =
							WTlib::byte_to_KODOVACI_MOZNOST(char_to_cislo(*t++));
					case 1:
						std::cout << *t << endl;
						opts.metoda.vyjimka_koder[ WTlib::OZNACENI_PASMA::LL ] =
							WTlib::byte_to_KODOVACI_MOZNOST(char_to_cislo(*t++));
					case 0:
						break;
					default:
						cerr << "Cannot handle more than 4 predictor (code_alt)" << endl;
						return 1;
				}
				break;
			case WTlib::DEKOMPOZICE::RED_BLACK:
				switch ( vstup.size() ) {
					case 4:
						opts.metoda.vyjimka_koder[ WTlib::OZNACENI_PASMA::BLACK_YELLOW ] =
							WTlib::byte_to_KODOVACI_MOZNOST(char_to_cislo(*t++));
					case 3:
						opts.metoda.vyjimka_koder[ WTlib::OZNACENI_PASMA::BLACK_BLUE ] =
							WTlib::byte_to_KODOVACI_MOZNOST(char_to_cislo(*t++));
					case 2:
						opts.metoda.vyjimka_koder[ WTlib::OZNACENI_PASMA::YELLOW ] =
							WTlib::byte_to_KODOVACI_MOZNOST(char_to_cislo(*t++));
					case 1:
						opts.metoda.vyjimka_koder[ WTlib::OZNACENI_PASMA::BLUE ] =
							WTlib::byte_to_KODOVACI_MOZNOST(char_to_cislo(*t++));
					case 0:
						break;
					default:
						cerr << "Cannot handle more than 4 predictor (code_alt)" << endl;
						return 1;
				}
				break;
			default:
				cerr << "Codealt is not enabled for this decomposition" << endl;
				return 1;
		}
	}

	return 0;
}
