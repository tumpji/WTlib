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
#ifndef WT_MAIN_HEADER_INC
#define WT_MAIN_HEADER_INC

#include <memory>
#include <string>
#include <atomic>

#include <stack> // zasobnik
#include <queue> // fronta

//#include "wt.h"  // definice komprimacnich voleb
#include "wt_inter.h"
#include "io.h"  // vstup vystup


class Command;
using Shared_data    = std::shared_ptr<WTdata>;
using Shared_command = std::shared_ptr<Command>;

class Entropicky_koder;

/* jeden prikaz
 * ten rika, co se s daty ma delat a jak */
class Command {
	public:
		Command () {};
		virtual ~Command () {};

		virtual void call (Environment*) = 0;
		virtual std::string whoami () { return oznaceni; } 
	protected:
		std::string oznaceni;
};


// kolekce clenu spravujici zasobniky v Env 
struct Environment_stacks {

	/* zásobník příkazů, 
	 * každý představuje 
	 * operaci nad obrázkem */
	std::stack<Shared_command>  command_stack;

	// zde budou ulozeny prazdne alokovane data k vyplneni
	// nebo vyplnena data ktera cekaji na entropicky koder (kodovani)
	std::stack<Shared_data>     data_stack; 
	// sem uklada entropicky koder data ktere prave dekodoval 
	// a na tomto stacku se provadi rekonstrukce
	std::stack<Shared_data>     data_stack_recon; 

	// manipulace s command_stack
	void            push_command (Shared_command);
	Shared_command  get_command (); // top + pop
};


/* diky teto tride lze tuto knihovnu pouzivat 
 * i vicevlaknove */
class WTlib::Environment : public WTio::IO_thread, public Environment_stacks {
public:
	// konstruktory 
	Environment (WTlib::WTencoder*, std::string cesta);
	Environment (WTlib::WTdecoder*, std::string cesta);

	// zacatek provadeni transformace (vcetne z5)
	void start (); 


	// zde lze najit nastaveni  z WTencoder / WTdecoder
	WTcommon* nastaveni;
	// znaci jakym smerem je prave kodovano/dekodovano
	SMER smer;
};






//////////////////////////////////////////////////////////////
///////// INLINE 

/////////////////
// comm stack inline 
inline Shared_command  Environment_stacks::get_command () {
	auto top = command_stack.top();
	command_stack.pop();
	return top;
}

inline void  Environment_stacks::push_command (Shared_command comm) {
	command_stack.push(comm);
}








#endif // WT_MAIN_HEADER_INC
