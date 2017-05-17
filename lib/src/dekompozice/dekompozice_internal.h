#ifndef WT_DEKOMPOZICE_INTERNAL_HEADER_INCLUDED
#define WT_DEKOMPOZICE_INTERNAL_HEADER_INCLUDED

#include "dekompozice.h"
#include "main.h"
#include "wt_inter.h"

namespace Dekompozice {

class Dek_level_cut: public Command {
	public:
		Dek_level_cut(int level): level_rozlozeni(level) {}
		~Dek_level_cut() {}

		virtual void call (Environment*) = 0;

	protected:
		int level_rozlozeni;
};

/***********************************
 * Klasicka tenzor. pyramidova dekompozice
 * Predava se pocet
 */
class Dek_pyramidova: public Dek_level_cut {
	public:
		Dek_pyramidova(int level): Dek_level_cut(level)
		{ oznaceni = "Dekompozice pyr. "; }

		virtual void call (Environment*); 
};

/************************************
 * Cerveno cerna dekompozice
 */
class Dek_red_black: public Dek_level_cut {
	public:
		Dek_red_black(int level, OZNACENI_PASMA pasmo = OZNACENI_PASMA::BLUE): Dek_level_cut(level), pasmo(pasmo)
		{ oznaceni = "Dekompozice red black "; }

		virtual void call (Environment*);
	protected:
		OZNACENI_PASMA pasmo;
};

} // end namespace dekompozice




#endif // WT_DEKOMPOZICE_INTERNAL_HEADER_INCLUDED
