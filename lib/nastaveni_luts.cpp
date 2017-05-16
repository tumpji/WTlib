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

#include <map>
#include <string>

#include "wt.h"


using namespace WTlib;
using std::map;
using std::string;

namespace {

	const map<OZNACENI_PASMA, std::string> map_string_OZNACENI_PASMA = {
		{OZNACENI_PASMA::LL, "LL"},
		{OZNACENI_PASMA::LH, "LH"},
		{OZNACENI_PASMA::HL, "HL"},
		{OZNACENI_PASMA::HH, "HH"},

		{OZNACENI_PASMA::BLACK_YELLOW, "BLACK_YELLOW"},
		{OZNACENI_PASMA::BLACK_BLUE, "BLACK_BLUE"},
		{OZNACENI_PASMA::YELLOW, "YELLOW"},
		{OZNACENI_PASMA::BLUE, "BLUE"}
	};

	const map<BAREVNY_PROSTOR, std::string> map_string_BAREVNY_PROSTOR = {
		{BAREVNY_PROSTOR::RAW, "Not in use"},
		{BAREVNY_PROSTOR::RCT, "Reversible color transform JPEG2000"},
		{BAREVNY_PROSTOR::YCoCgR, "YCoCg-R"},
		{BAREVNY_PROSTOR::RDgDb, "RDgDb"},
		{BAREVNY_PROSTOR::LDgEb, "LDgEb"},
		{BAREVNY_PROSTOR::mRDgDb, "mRDgDb (only 8b)"},
		{BAREVNY_PROSTOR::mLDgEb, "mLDgEb (only 8b)"},
		{BAREVNY_PROSTOR::LOCOI, "LOCO-I"},
	};

	const map<VLNKA, std::string> map_string_VLNKA = {
		{VLNKA::CDF_5_3  ,	"CDF 5/3"},
		{VLNKA::CDF_5_3p  ,	"CDF 5/3 (+)"},
		{VLNKA::CDF_5_3pm , "CDF 5/3 (+-)"},
		{VLNKA::CDF_9_7,	"CDF 9/7-F"},
		{VLNKA::HAAR,		"Haar"     },
		{VLNKA::CDF_9_7_M,  "CDF 9/7-M"},
		{VLNKA::CDF_9_7_Mm,  "CDF 9/7-M (-)"},
		{VLNKA::v2_6,		"2/6"},
		{VLNKA::v2_10,		"2/10"},
		{VLNKA::v5_11_C,    "5/11-C"},
		{VLNKA::v5_11_A,    "5/11-A"},
		{VLNKA::v6_14,		"6/14"},
		{VLNKA::v13_7_T,    "13/7-T"},
		{VLNKA::v13_7_C,    "13/7-C"},
		{VLNKA::v1_3,		"Linear"   },
		{VLNKA::v1_3p,		"Linear (+)" },
		{VLNKA::v1_7,		"1/7"   },
		{VLNKA::v1_7m,		"1/7 (-)"},
	};


	const map<DEKOMPOZICE, std::string> map_string_DEKOMPOZICE = {
		{DEKOMPOZICE::RED_BLACK,  "Red-Black"},
		{DEKOMPOZICE::PYRAMIDOVA, "Pyramidal"}
	};

	const map<PREDIKTOR, std::string> map_string_PREDIKTOR = {
		{PREDIKTOR::NIC,	"Not in use"},
		{PREDIKTOR::LEFT,	"Left"},
		{PREDIKTOR::UP,		"Up"},
		{PREDIKTOR::DIAGONAL, "Diagonal" },
		{PREDIKTOR::MED,	"Med"},
		{PREDIKTOR::PAETH,	"Paeth"},
		{PREDIKTOR::GRADIENT_ADJUSTED, "Gradient_adjusted"}
	};

	const map<KODER, std::string> map_string_KODER = {
		{KODER::EBCOT,  "EBCOT koder"},
		{KODER::ARITM_256E,  "Aritm. with 256 values"},
		{KODER::ARITM_512E,  "Aritm. with 512 values"},
		{KODER::ARITM_1024E, "Aritm. with 1024 values"},
		{KODER::RAW,    "Not in use"}
	};

	const map<KODOVACI_MOZNOST, std::string> map_string_KODOVACI_MOZNOST = {
		{KODOVACI_MOZNOST::EBCOT    , "Ebcot default behaviour"},
		{KODOVACI_MOZNOST::EBCOT_LL , "Ebcot LL"},
		{KODOVACI_MOZNOST::EBCOT_LH , "Ebcot LH"},
		{KODOVACI_MOZNOST::EBCOT_HL , "Ebcot HL"},
		{KODOVACI_MOZNOST::EBCOT_HH , "Ebcot HH"},
		{KODOVACI_MOZNOST::ARITM_256E,  "Aritm. with 256 values"},
		{KODOVACI_MOZNOST::ARITM_512E,  "Aritm. with 512 values"},
		{KODOVACI_MOZNOST::ARITM_1024E, "Aritm. with 1024 values"},
		{KODOVACI_MOZNOST::RAW      , "Raw"}
	};

}


#define DEFINE_TO_STRING_FCE(X) \
	string WTlib::to_string (X a) { \
		auto it = map_string_##X.find(a); \
		if ( it == map_string_##X.end() ) \
			return "Undefined"; \
		return it->second; \
	} \
	unsigned char WTlib::to_byte (X a) { \
		return static_cast<unsigned char>(a); \
	} \
	X WTlib::byte_to_##X (unsigned char a) { \
		if ( static_cast<unsigned char>(X::Last) >= a) \
			return static_cast<X>(a); \
		else \
			throw std::invalid_argument( \
					"Cannot convert id " + std::to_string((unsigned)a) + " to "#X ); \
	}

DEFINE_TO_STRING_FCE(OZNACENI_PASMA)

DEFINE_TO_STRING_FCE(BAREVNY_PROSTOR)
DEFINE_TO_STRING_FCE(VLNKA)
DEFINE_TO_STRING_FCE(DEKOMPOZICE)
DEFINE_TO_STRING_FCE(PREDIKTOR)
DEFINE_TO_STRING_FCE(KODER)
DEFINE_TO_STRING_FCE(KODOVACI_MOZNOST)





