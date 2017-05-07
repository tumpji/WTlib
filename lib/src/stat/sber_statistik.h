#ifndef SBER_STATISTIK_HEADER_INCLUDED
#define SBER_STATISTIK_HEADER_INCLUDED
#include "main.h"
#include "entropicky_koder.h"


namespace stat {

void vlozit_velikost_zakodovaneho_pasma(
	   Environment* env,
	   OZNACENI_PASMA pasmo,
	   int velikost );	   

// human-machine readeble stat. o pasmech v cerr
// #0 <jemno>:<cislo>            
//		oznacuje celkovou velikost pasma (<cislo>B) pojmenovaneho <jemno>
// #1 <cislo>:<cislo2>
//		oznacuje celkovou velikost urovne (<cislo2>B) urovne <cislo> (0 = LL/B/...)
void tisk_statistiky (Environment* env);

}


#endif // SBER_STATISTIK_HEADER_INCLUDED
