#include "entropicky_koder.h"
#include "mqc.h"
#include "wt_inter.h"



// obecna konstrukce entropickeho koderu
class Ent_ebcot : public Entropicky_koder {
public:
		Ent_ebcot (OZNACENI_PASMA pasmo, KODOVACI_MOZNOST kodovat_jako_pasmo);
		virtual ~Ent_ebcot () {}
		
		// koduje data podle predanych dat
		virtual void		encode (Environment*, Shared_data);
		// dekoduje podle predaneho neinicializovane sablony (resp. vyska, sirka
		virtual Shared_data decode (Environment*, Shared_data);

protected:
		//MqcState mqc;
		KODOVACI_MOZNOST kodovat_jako_pasmo;
};
