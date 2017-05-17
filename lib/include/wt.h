#ifndef WT_HEADER_INCLUDED
#define WT_HEADER_INCLUDED

#include <iostream>
#include <fstream>

#include <vector>
#include <memory>
#include <climits>

#include <string>
#include <map>

#include <exception>
#include <stdexcept>
#include <cassert>

namespace WTlib {

// data, reprezentuji nejake pasmo
class WTdata {
public:
	enum class Allocation_options { DO_NOT_ALLOCATE, ALLOCATE };

	WTdata (int rozmer_x, 
			int rozmer_y, 
			Allocation_options = Allocation_options::ALLOCATE );
	WTdata (std::string oznaceni, 
			int rozmer_x, 
			int rozmer_y, 
			Allocation_options = Allocation_options::ALLOCATE );
	WTdata (const WTdata& ) = default;
	~WTdata (){};

	// alokace dat v pozdejsi fazy
	void reserve () { data.resize(rozmer_x*rozmer_y, 0); }

	// pristup k prvkum
	int& operator[] (int i) {
		assert(i < rozmer_x*rozmer_y); 
		assert(static_cast<unsigned>(rozmer_x*rozmer_y) == data.size());
		return data[i];
	}


	std::vector<int> data;
	int rozmer_x;
	int	rozmer_y;

	std::string oznaceni;
};

// vnitrni trida
class Environment;

enum class OZNACENI_PASMA : unsigned char {
	NEZNAM,
	LL, LH, HL, HH,       // pyramidal
	BLACK_YELLOW, BLACK_BLUE, YELLOW, BLUE,  // pro Red-Black t
		Last=BLUE
};


// vyber transformace barevneho prostoru z RGB
// v pripade ze pocet pasem < 3 neprovede se nic (i kdyz je zadane)
// 4+ pasmo nebude nijak transformovano 
enum class BAREVNY_PROSTOR : unsigned char {  
	RAW,   // zadne pasmo neni zmeneno
	RCT,   // ala JPEG2000
	YCoCgR,
	RDgDb,
	LDgEb,
	mRDgDb,
	mLDgEb,
	LOCOI,
	Last=LOCOI
};

// vyber liftingu 
// provede pro vsechny pasma
enum class VLNKA : unsigned char {  
	CDF_5_3,    // ala JPEG2000
	CDF_9_7,    // vlnka CDF 9/7-F (4,4)
	HAAR,       // Haarova transformace
	CDF_9_7_M,  // vlnka CDF 9/7-M (4,2)
	v5_11_C,
	v5_11_A,
	v13_7_T,
	v13_7_C,
	v2_6,
	v2_10,
	v6_14,
	CDF_5_3p,  // + v prediktu
	CDF_5_3pm,  // + v prediktu a minus v update
	CDF_9_7_Mm,  // - v prediktu
	v1_3,  // linearni 
	v1_3p, // linearni - v prediktu
	v1_7,  // - v prediktu
	v1_7m, // - v prediktu
		Last=v1_7m
};	

// vyber dekompozice 
// provede se na vsechny pasma po barevne trans.
enum class DEKOMPOZICE : unsigned char { 
	PYRAMIDOVA,		// pyramidova dekompozice
	RED_BLACK,      // red-black wavelet transform
		Last=RED_BLACK
};

enum class PREDIKTOR : unsigned char {
	NIC,
	LEFT,
	UP,
	DIAGONAL,
	MED, 
	PAETH, 
	GRADIENT_ADJUSTED,
		Last=GRADIENT_ADJUSTED
};

// vyber entropickeho koderu
enum class KODER : unsigned char {
	EBCOT,   // uprava MQ koderu co je v JPEG2000
	ARITM_256E, 
	ARITM_512E,
	ARITM_1024E,
	RAW, // raw - bez entropickeho koderu (pouze int do souboru)
	Last=RAW
};

// vyber pro kodovani jednotlivych pasem
enum class KODOVACI_MOZNOST : unsigned char {
	EBCOT,    // kodovat implicitne
	EBCOT_LL, // kodovat LL
	EBCOT_LH,
	EBCOT_HL,
	EBCOT_HH,
	EBCOT_Last = EBCOT_HH,
	ARITM_256E,
	ARITM_512E,
	ARITM_1024E,
	RAW,
	Last=RAW
};




/////////////////////////////
// nastaveni komprese / zjisteni jak bylo komprimovano
class WTcommon {
public:
	WTcommon (const WTcommon&) = default;
	WTcommon ();

	//	barevny prostor ktery se bude komprimovat (z RGB)
	BAREVNY_PROSTOR		barevny_prostor;
	bool                avrg_zero;

	//  vlnka pouzita pri WT
	VLNKA				vlnka;

	// dekompozice pouzita pri WT
	DEKOMPOZICE         dekompozice;
	// maximalni pocet urovni dekompozice
	int	dekompozice_max; // 0 == WT neprobehne
	// minimalni rozmer pasma pripusteneho k dalsimu rozkladu (v ruznem smeru)
	int	dekompozice_min_rozmer; // >= 2
	// vedlejsi level dekompozice
	int dekompozice_side_level; 


	// prediktor pouzivany jeste pred WT
	PREDIKTOR           prediktor_pred;
	// prediktor pouzivany na pasma (konkretni)
	std::map<OZNACENI_PASMA, PREDIKTOR> prediktor_pasmo;


	// entropicky koder - definuje implicitni chovani
	KODER               koder;
	// mapovani pasma na ruzne moznosti ruznych koderu
	std::map<OZNACENI_PASMA, KODOVACI_MOZNOST> vyjimka_koder;
	


	bool verbose;


	// implicitni hodnoty : nastavuji/ctou se normalne 
	const static auto imp_barevny_postor			= BAREVNY_PROSTOR::RCT;
	const static bool imp_avrg_zero                 = false;

	const static auto imp_vlnka						= VLNKA::CDF_5_3;

	const static auto imp_dekompozice				= DEKOMPOZICE::PYRAMIDOVA;
	const static  int imp_dekompozice_max			= INT_MAX;
	const static  int imp_dekompozice_min_rozmer	= 2; 
	const static  int imp_dekompozice_side_level    = 1;

	const static auto imp_prediktor_puvodni_data      = PREDIKTOR::NIC;
	const static auto imp_prediktor_rozlozeneho_pasma = PREDIKTOR::NIC;

	const static auto imp_koder						= KODER::EBCOT;


protected:
	// sem se ukladaji se pasma 
	std::vector<std::shared_ptr<WTdata>> tmp_data_vec;

	friend class Environment_stacks;
	friend class Environment;
	friend std::ostream& operator<< (std::ostream& stream, const WTcommon& dt);
	friend void set_up_settings(Environment*);
};





////////////////////////////////////////
// KOMPRIMUJE DATA
class WTencoder : public WTcommon {
	public:
	/* INICIALIZACE kodovani */
	// @throws std::invalid_argument
	WTencoder(const WTcommon&);
	WTencoder();

	/* NASLEDUJI MOZNOSTI VKLADANI DAT PRO TRANSFROMACI */
	// po pixelech
	// vkladat v poradi RGB,... dalsi uz nerozhoduje
	void vlozit_pasmo (WTdata kopie);
	//void vlozit_pasmo (std::vector<WTdata>&& move);
	

	// provede ulozeni vlozene reprezentace
	void start (std::string cesta);
};

//////////////////////////////////////
// DEKOMPRIMUJE DATA
class WTdecoder : public WTcommon {
public:
	WTdecoder();

	// spusti
	std::vector<WTdata> start (std::string cesta);
};






//////////////////////////////////////////////
////////// definice pomocnych funkci

// to_string (X) : prevede na retezec pro uzivatele
// to_byte(X) : prevede na cislo od 0->N
// byte_to_X(Y) : prevede cislo na oznaceni

#define WTlib_MACRO(X) \
	std::string   to_string  (X); \
	unsigned char to_byte    (X); \
	X             byte_to_##X (unsigned char);


WTlib_MACRO(BAREVNY_PROSTOR)
WTlib_MACRO(VLNKA)
WTlib_MACRO(DEKOMPOZICE)
WTlib_MACRO(PREDIKTOR)
WTlib_MACRO(KODER)

WTlib_MACRO(OZNACENI_PASMA)
WTlib_MACRO(KODOVACI_MOZNOST)

#undef WTlib_MACRO

inline std::ostream& operator<< (std::ostream& stream, const WTcommon& dt) {
	using std::endl;
	stream << "Color transform  : " << to_string(dt.barevny_prostor) << endl;
	stream << "  avrg_zero      : " << (dt.avrg_zero ? "True" : "False") << endl;
	stream << "Wavelet          : " << to_string(dt.vlnka) << endl;
	stream << "Decomposition    : " << to_string(dt.dekompozice) << endl;
	stream << "  max. number : "    << dt.dekompozice_max << endl;
	stream << "  min. size   : "    << dt.dekompozice_min_rozmer << endl;
	stream << "  side level  : "    << dt.dekompozice_side_level << endl;

	stream << "Predictor original data : " << to_string(dt.prediktor_pred) << endl;
	for ( const auto p : dt.prediktor_pasmo ) 
		stream << "  predictor on " << to_string(p.first) << " : " << to_string(p.second) << endl;

	stream << "Coder            : " << to_string(dt.koder) << endl;
	for ( const auto p : dt.vyjimka_koder )
		stream << "  but " << to_string(p.first) << "  code as " << to_string(p.second) << endl;

	return stream;
}


} // end namespace WTlib



#endif // WT_HEADER_INCLUDED
