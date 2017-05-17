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
#ifndef ROZLOZENI_HEADER_DEFINED
#define ROZLOZENI_HEADER_DEFINED

#include "main.h"

// odcovska trida vsech rozlozeni
// rozlozeni u me neni dekompozice
// napr. rozlozeni horizontalni, veritkalni, RedBlack, YellowBlue
// jsou v tomto pripade ruzne (narozdil od pyramidove a RedBlack dekom)
//
//
// stara se o pristup ke spravnemu zdroji
// => get_data()   vraci bud sablonu (vyplnene pouze velikosti)
//                 nebo vlastni data
//    obdobne funguje push_data
//
// prepinac sablona rozhoduje o pristupu k datum
// a taky jestli se maji data nejak zpracovavat
//
// dale makro IS_KODER, IS_DEKODER

#define IS_KODER   (env->smer == SMER::KODER)
#define IS_DEKODER (env->smer == SMER::DEKODER)

//#define COMMAND_PUSH(X,Y) env->push_command( std::make_shared<X>(Y) )


class Rozlozeni : public Command {
public:
	Rozlozeni ( bool sablona) : sablona(sablona) { }

	virtual void call (Environment*) = 0;
protected:
	bool sablona;

private:
	// zajsituje ziskavani dat a jejich mazani
	Shared_data get_from_stack (std::stack<Shared_data>& s) {
		auto data = s.top();
		s.pop();
		return data;
	}


protected:
	// koder bere vzdy z hlavniho stacku
	// dekoder-sablona bere s hlavniho stacku
	// dekoder-!sablona bere s vedlejsiho recon stacku
	Shared_data 
	get_data (Environment* env) 
	{
		// rozklada se rovnou data 
		if ( IS_KODER ) {
			return get_from_stack(env->data_stack);
		} else if ( sablona ) { // dekoder
			return get_from_stack(env->data_stack);
		} else {
			return get_from_stack(env->data_stack_recon);
		}
	}

	// koder vklada do hlavniho stejne jako sablona u dekoderu
	// nesablona u dekoderu vklada do vedlejsiho rekon
	void push_data (Environment* env, Shared_data data) {
		if ( IS_KODER )
			env->data_stack.push(data);
		else if ( sablona ) 
			env->data_stack.push(data);
		else
			env->data_stack_recon.push(data);
	}
};

template< typename Trida, typename ... Args >
inline static void COMMAND_PUSH( Environment *env, Args ... args ) {
	env->push_command( std::make_shared<Trida>( args... ) );
}


#endif // ROZLOZENI_HEADER_DEFINED

