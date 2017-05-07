
#ifndef SIGN_MODEL_HEADER_INCLUDED
#define SIGN_MODEL_HEADER_INCLUDED

#include <vector>
#include "aritmeticky.h"

namespace SignModel {

// bazova trida, ktera slouzi pro ulozeni znamenka z libovolnych dat
struct SignModel_base {
	virtual void 
		encode ( Environment*, Aritmeticky_koder &, Shared_data data, OZNACENI_PASMA pasmo ) = 0;
	virtual std::vector<bool> 
		decode ( Environment*, Aritmeticky_koder &, Shared_data data, OZNACENI_PASMA pasmo ) = 0;
};

enum Type { T32 };

SignModel_base* get_sign_model_by_type ( Type type );

// inicializace - provadi se pred kodovanim a dekodovanim
void init_sign_models ();

}



#endif // SIGN_MODEL_HEADER_INCLUDED

