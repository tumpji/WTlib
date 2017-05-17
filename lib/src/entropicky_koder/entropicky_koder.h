#ifndef ENT_HEADER_INC
#define ENT_HEADER_INC

#include <memory>

#include "wt_inter.h"
#include "main.h"


// obecna konstrukce entropickeho koderu
class Entropicky_koder {
public:
		
		// koduje data podle predanych dat
		virtual void		encode (Environment* env, Shared_data data) = 0;
		// dekoduje podle predaneho neinicializovane sablony (resp. vyska, sirka
		virtual Shared_data decode (Environment* env, Shared_data data) = 0;

protected:
		Entropicky_koder (OZNACENI_PASMA pasmo): pasmo(pasmo) {}
		virtual ~Entropicky_koder () {}

		// o jake pasmo se jedna (napr. pro statistiku)
		OZNACENI_PASMA pasmo;
};

// inicializace entropickeho koderu
std::shared_ptr<Entropicky_koder> 
	get_entropicky_koder (Environment*, OZNACENI_PASMA pasmo);


void init_all_entropy_coders ();











#endif // ENT_HEADER_INC
