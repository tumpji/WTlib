#include <iostream>
#include <utility>
#include <cassert>
#include <memory>

#include "dekompozice_internal.h"  // vystupni metody
#include "vlnka.h"
#include "rozlozeni.h"

#include "zpracovani_pasma.h"
#include "std_macro.h"

/* implementace obecne pyramidové dekompozice
 *
 * Vytvořeno pomocí gramatiky popsána dole.
 * Nezavislé na použité vlnce.
 */


/* analytická gramatika 
 * ***************************
	Dek_pyr(n) ::= Dek_hor(n)[cele pasmo]

	Dek_hor(n) ::= H   Dek_ver(n)[L]			Dek_ver(0)[H]
	Dek_hor(0) ::= Ulozit[LL]

	Dek_ver(n) ::= V   Dek_hor(n-1)[LL]		    Ulozit[LH]
	Dek_ver(0) ::= V   Ulozit[HL]               Ulozit[HH]
*/



///////////////////////////// ROZKLADANI /////////////////////////
//////////////////////////////////////////////////////////////////

namespace {

// provadi horizontalni rozkladani pasma
std::pair<Shared_data, Shared_data>
rozlozeni (Environment* env, Shared_data vstup)
{
	assert(vstup->rozmer_x > 1);
	std::shared_ptr<Vlnka>		vlnka = get_vlnka(env);

	// vystup - predalokace
	Shared_data 
		L = std::make_shared<WTdata>( (vstup->rozmer_x+1)/2, vstup->rozmer_y ),
		H = std::make_shared<WTdata>( (vstup->rozmer_x  )/2, vstup->rozmer_y );

	// po radcich
	for ( int radek = 0; radek < vstup->rozmer_y; ++radek)
	{
		int h0 = radek * vstup->rozmer_x;   // hranice <  vcetne
		int h1 = h0 + vstup->rozmer_x - 1;  // hranice >

		// pro vsechny kroky WT
		for ( int index_step = 0; 
			  index_step < vlnka->get_number_lifting_steps(); 
			  ++index_step ) {
			vlnka->lifting_predict(vstup->data, index_step,
								   false, true, 
								   h0, h0 + 1, h1, 1);
			vlnka->lifting_update(vstup->data, index_step,
								  false, true,
								  h0, h0, h1, 1);
		}

		// radek je hotov nasleduje rozlozeni dat
		for ( int i = 0; i < vstup->rozmer_x/2; ++i ) {
			L->data[ (radek * L->rozmer_x) + i ] = vstup->data[h0 + 2*i    ];
			H->data[ (radek * H->rozmer_x) + i ] = vstup->data[h0 + 2*i + 1];
		}
		if ( vstup->rozmer_x % 2 ) 
			L->data[ (radek+1)*(L->rozmer_x) - 1 ] = vstup->data[h1];
	}
	
	return std::make_pair(L, H);
}

// provadi horizontalni skladani dvou pasem
Shared_data slozeni ( Environment* env, Shared_data L, Shared_data H)
{
	std::shared_ptr<Vlnka>   vlnka = get_vlnka(env);

	assert(L->rozmer_y == H->rozmer_y);
	assert(L->rozmer_x - H->rozmer_x <= 1);

	Shared_data vystup = std::make_shared<WTdata>(
			L->rozmer_x + H->rozmer_x, L->rozmer_y );

	for ( int radek = 0; radek < vystup->rozmer_y; ++radek )
	{
		int h0 = radek * vystup->rozmer_x;
		int h1 = h0 + vystup->rozmer_x - 1;

		// naplneni do jedne 
		for (int i = 0; i < vystup->rozmer_x/2; ++i ) {
			vystup->data[ h0 + i*2     ] = L->data[ radek*L->rozmer_x + i ];
			vystup->data[ h0 + i*2 + 1 ] = H->data[ radek*H->rozmer_x + i ];
		}
		if ( vystup->rozmer_x % 2 )
			vystup->data[h1] = L->data[ (radek+1)*L->rozmer_x - 1 ];

		// provest lifting obracene
		for ( int index_step = vlnka->get_number_lifting_steps() - 1; 
			  index_step >= 0; 
			  --index_step ) 
		{
			vlnka->lifting_update(vystup->data, index_step,
								  true, true,
								  h0, h0, h1, 1);
			vlnka->lifting_predict(vystup->data, index_step,
								   true, true, 
								   h0, h0 + 1, h1, 1);
		}
	}
	return vystup;
}

// ukolem je obratit vektor nasledujicim zpusobem
// abc     adgj    abc
// def  => behk => def
// ghi     cfil    ghi
// jkl             jkl
Shared_data obraceni ( Shared_data data ) {
	Shared_data vystup = std::make_shared<WTdata>(data->rozmer_y, data->rozmer_x);

	assert(data->data.size());
	assert(data->data.size() == static_cast<unsigned>(data->rozmer_x * data->rozmer_y) );
	
	// radek, sloupec v originalnich datech -> data
	for ( int sloupec = 0; sloupec < data->rozmer_x; ++sloupec )
	for ( int radek = 0; radek < data->rozmer_y; ++radek )
	{
		vystup->data[radek + sloupec*data->rozmer_y] = 
			data->data[sloupec + radek*data->rozmer_x];
	}
	return vystup;
}

} // end unnamed namespace 

///////////////////////////////////////////////////////////////
/////////////////////// DEFINICE ROZKLADANI ///////////////////

namespace Dekompozice::Pyramidova {


// horizontalni rozlozeni
class Rozlozeni_hor : public Rozlozeni {
public:
	Rozlozeni_hor(bool sablona): Rozlozeni(sablona) {}

private:
	std::pair<Shared_data, Shared_data>
	rozlozit_sablonu (Environment* env, Shared_data s) {
		(void)env;
		const auto alloc = WTdata::Allocation_options::DO_NOT_ALLOCATE;

		Shared_data 
			L = std::make_shared<WTdata>( (s->rozmer_x+1)/2, s->rozmer_y, alloc ),
			H = std::make_shared<WTdata>( (s->rozmer_x  )/2, s->rozmer_y, alloc );
		return std::make_pair(L,H);
	}

public:
	virtual void call (Environment* env) {
		if ( IS_KODER ) {
			auto data = get_data(env);
			auto p = ::rozlozeni(env, data);
			push_data(env, p.second);
			push_data(env, p.first);
		} else if ( sablona ) {
			auto data = get_data(env);
			auto p = rozlozit_sablonu(env, data);
			push_data(env, p.second);
			push_data(env, p.first);
		} else { // jedna se o rekonstrukci opravdovych dat
			auto H = get_data(env); // tyto data se vkladaji obracene
			auto L = get_data(env);
			auto d = ::slozeni(env, L, H);
			push_data(env, d);
		}
	}
};

// vertikalni rozlozeni
class Rozlozeni_ver : public Rozlozeni {
public:
	Rozlozeni_ver(bool sablona): Rozlozeni(sablona) {}

private:
	std::pair<Shared_data, Shared_data> rozlozit_sablonu (Environment* env, Shared_data s) {
		(void)env;
		const auto alloc = WTdata::Allocation_options::DO_NOT_ALLOCATE;

		Shared_data 
			L = std::make_shared<WTdata>( s->rozmer_x, (s->rozmer_y+1)/2, alloc ),
			H = std::make_shared<WTdata>( s->rozmer_x, (s->rozmer_y  )/2, alloc );
		return std::make_pair(L,H);
	}

public:
	virtual void call (Environment* env) {
		if ( IS_KODER ) {
			auto data = ::obraceni(get_data(env));
			auto p = ::rozlozeni(env, data);
			push_data(env, ::obraceni(p.second));
			push_data(env, ::obraceni(p.first));
		} else if ( sablona ) {
			auto data = get_data(env);
			auto p    = rozlozit_sablonu(env, data);
			push_data(env, p.second);
			push_data(env, p.first);
		} else { // jedna se o rekonstrukci opravdovych dat
			auto H = ::obraceni(get_data(env)); // obracene se vkladaji
			auto L = ::obraceni(get_data(env));
			auto d = ::slozeni(env, L, H);
			push_data(env, ::obraceni(d));
		}
	}
};


////////////////////////////////////////////////////////////////////////////////
//******************************************************************************
//..............................................................................
//			LOGIKA ROZKLADU

enum INTERNI_OZNACENI_PASMA {
	L = 0, LL = 0, LH = 1,
	H = 2, HL = 2, HH = 3
};



/* horizontalni logika dekompozice 
 * konstruktor 0 => ulozit
 * konstruktor n => <-> Dek_ver(n) Dek_ver(1)  
 */
class Horizontalni_logika: public Dek_pyramidova {
public:
	Horizontalni_logika(int level, INTERNI_OZNACENI_PASMA op) : Dek_pyramidova(level), op(op)
	{oznaceni += "horizontalni ";}
	virtual void call (Environment* env);

private:
	// lze delit v hor smeru
	bool splnuje_kriteria (Environment *env) {
		const auto& kodovana_data = *env->data_stack.top();
		int hor_velikost = kodovana_data.rozmer_x;
		return hor_velikost >= env->nastaveni->dekompozice_min_rozmer;
	}

	// uz delit nelze
	bool nesplnuje_vsechna_kriteria (Environment *env);
	INTERNI_OZNACENI_PASMA op;
};


/* vertikalni logika dekompozice  
 * konstruktor 0 => chyba 
 * konstruktor n => ^|_   Dek_hor(n-1)   Dek_hor(0) 
 */
class Vertikalni_logika: public Dek_pyramidova {
public:
	Vertikalni_logika(int i, INTERNI_OZNACENI_PASMA op) : Dek_pyramidova(i), op(op)
	{oznaceni += "vertikalni ";}
	virtual void call (Environment* env);

	// jestli lze rozdelit 
	static bool splnuje_kriteria (Environment *env ) {
		const auto& kodovana_data = *env->data_stack.top();
		int ver_velikost = kodovana_data.rozmer_y;
		return ver_velikost >= env->nastaveni->dekompozice_min_rozmer;
	}

	//friend bool Horizontalni_logika::nesplnuje_vsechna_kriteria(Environment *);
	INTERNI_OZNACENI_PASMA op;
};

bool Horizontalni_logika::nesplnuje_vsechna_kriteria (Environment *env) {
	if ( Vertikalni_logika::splnuje_kriteria(env) )
		return false;
	if ( splnuje_kriteria(env) )
		return false;
	return true;
}


inline OZNACENI_PASMA dekodovat_interni_pasmo ( INTERNI_OZNACENI_PASMA ip )
{
	switch (ip) {
		case INTERNI_OZNACENI_PASMA::LL:
			return OZNACENI_PASMA::LL;
		case INTERNI_OZNACENI_PASMA::LH:
			return OZNACENI_PASMA::LH;
		case INTERNI_OZNACENI_PASMA::HL:
			return OZNACENI_PASMA::HL;
		case INTERNI_OZNACENI_PASMA::HH:
			return OZNACENI_PASMA::HH;
		default:
			assert(false);
	}
}


void Horizontalni_logika::call(Environment* env)
{
	assert(env->data_stack.size());
	assert(level_rozlozeni >= 0);

	//std::cerr << "Horizontalni " << std::endl;

	const auto &side_level = env->nastaveni->dekompozice_side_level;

	/*
	if ( env->nastaveni->dekompozice_max == level_rozlozeni )
	{
		std::cout << "HOOOOOOOOOOOOOOU" << std::endl;
		COMMAND_PUSH<Vertikalni_logika>(env, level_rozlozeni, INTERNI_OZNACENI_PASMA::L );
		return;
	}*/

	// uz nelze dale rozkladat
	if ( nesplnuje_vsechna_kriteria(env) || level_rozlozeni == 0 ) 
		push_zpracovat_hotove_pasmo(env, dekodovat_interni_pasmo(op) );
	// preskakuji horizontalni rozdeleni ( nelze rozkladat pouze v tomto smeru )
	else if ( not splnuje_kriteria(env) )
		COMMAND_PUSH<Vertikalni_logika>(env, level_rozlozeni - 1, INTERNI_OZNACENI_PASMA::L );
	// lze rozkladat
	else if ( level_rozlozeni ) { // rozkladani jeste povoleno

		const int vystupni_level_h = MIN( level_rozlozeni - 1, side_level );

		// skladani
		if ( IS_DEKODER )  // [] <- H, L
			COMMAND_PUSH<Rozlozeni_hor>(env, false); 

		// H 
		COMMAND_PUSH<Vertikalni_logika>( env, vystupni_level_h,    INTERNI_OZNACENI_PASMA::H ); 
		// L 
		COMMAND_PUSH<Vertikalni_logika>( env, level_rozlozeni - 1, INTERNI_OZNACENI_PASMA::L ); 

		// rozkladani ( u dekoderu je to sablona co se rozklada )
		COMMAND_PUSH<Rozlozeni_hor>(env, true); // [] -> L , H
	}
}

void Vertikalni_logika::call (Environment* env)
{
	assert(env->data_stack.size());
	assert(level_rozlozeni >= 0);

	//std::cerr << "Vertikalni " << std::endl;

	const auto &side_level = env->nastaveni->dekompozice_side_level;

	if ( not splnuje_kriteria(env) || level_rozlozeni == 0 ) {
		// !! {LL = L} & {HL = H} => takze se bude kodovat jak prislo
		COMMAND_PUSH<Horizontalni_logika>( env, MAX(0,level_rozlozeni-1), op );
	}
	else {
		// skladani u dekoderu
		if (IS_DEKODER) { 
			COMMAND_PUSH<Rozlozeni_ver>( env, false ); // L <- LH LL
		}	

		const int vystupni_level_l = MIN(level_rozlozeni - 1, MAX(0,side_level - 1));
		assert( vystupni_level_l >= 0 );
		assert( level_rozlozeni - 1 >= 0 );

		// H -> HH (3) / L -> LH (1)
		COMMAND_PUSH<Horizontalni_logika>( env, vystupni_level_l, 
				static_cast<INTERNI_OZNACENI_PASMA>(op | 1) );

		// L -> LL (0) / H -> HL (2)
		COMMAND_PUSH<Horizontalni_logika>( env, level_rozlozeni - 1, op );

		// rozkladani ( u dekoderu je to sablona )
		COMMAND_PUSH<Rozlozeni_ver>( env, true ); // L -> LH LL
	}
}

} // end namespace Dekompozice::Pyramidova




///////////////////////////// FINALNI TAHY /////////////////////////
//////////////////////////////////////////////////////////////////

namespace Dekompozice {
	void Dek_pyramidova::call (Environment* env) {
		assert(level_rozlozeni >= 0);

		COMMAND_PUSH<Pyramidova::Horizontalni_logika>( 
				env, level_rozlozeni, 
				Dekompozice::Pyramidova::INTERNI_OZNACENI_PASMA::LL );
	}
}

