#ifndef ARITM_MODEL_SIGN32_256E_HEADER_DEFINED
#define ARITM_MODEL_SIGN32_256E_HEADER_DEFINED

#include "entropicky_koder.h"
#include "model_sign.h"
#include "model_256e.h"

template<SignModel::Type st, ValueModel::Type vt>
struct Valxe_signx: public Entropicky_koder {
	Valxe_signx( OZNACENI_PASMA pasmo ) : 
		Entropicky_koder( pasmo ), 
		sign_model( SignModel::get_sign_model_by_type(st) ),
		value_model( ValueModel::get_value_model_by_type(vt) )
	{};

	virtual ~Valxe_signx () {};

	// koduje data podle predanych dat
	virtual void		encode (Environment* env, Shared_data data);
	// dekoduje podle predaneho neinicializovane sablony (resp. vyska, sirka
	virtual Shared_data decode (Environment* env, Shared_data data);
private:

	SignModel::SignModel_base   * sign_model;
	ValueModel::ValueModel_base * value_model;
};


#endif // ARITM_MODEL_SIGN32_256E_HEADER_DEFINED
