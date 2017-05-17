#ifndef PREDICTOR_HEADER_INCLUDED
#define PREDICTOR_HEADER_INCLUDED

#include <memory>

#include "main.h"


namespace Predictor {

class Predictor {
public:
	Predictor () {}
	virtual ~Predictor () {}

	virtual void forward_predict (Shared_data) = 0;
	virtual void reverse_predict (Shared_data) = 0;
};

using Shared = std::shared_ptr<Predictor>;
}

// vraci ten spravny prediktor pro danou 
Predictor::Shared  get_predictor  ( Environment * env, OZNACENI_PASMA pasmo );

// preidktor pro puvodni pasmo
Predictor::Shared  get_predictor  ( Environment * env );






#endif // PREDICTOR_HEADER_INCLUDED
