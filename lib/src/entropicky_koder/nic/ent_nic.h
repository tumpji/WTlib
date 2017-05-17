#include "entropicky_koder.h"

// pouze uklada integery do souboru
// nedela zadne entropicke kodovani
class Ent_NIC : public Entropicky_koder {
public:
		Ent_NIC (OZNACENI_PASMA pasmo): Entropicky_koder(pasmo) {}
		virtual ~Ent_NIC () {}
		
		// koduje data podle predanych dat
		virtual void		encode (Environment* env, Shared_data data);
		// dekoduje podle predaneho neinicializovane sablony (resp. vyska, sirka
		virtual Shared_data decode (Environment* env, Shared_data data);

protected:
		// seznam vnitrnich stavu
};
