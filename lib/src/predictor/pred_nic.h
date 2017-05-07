
#ifndef NIC_PREDICTOR_HEADER_INCLUDED
#define NIC_PREDICTOR_HEADER_INCLUDED

#include "predictor.h"

namespace Predictor {
	class Nic : public Predictor {
	public:
		Nic () {};
		virtual ~Nic () {};

		virtual void forward_predict (Shared_data data) {(void)data;};
		virtual void reverse_predict (Shared_data data) {(void)data;};
	};
}

#endif // NIC_PREDICTOR_HEADER_INCLUDED
