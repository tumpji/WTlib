
#ifndef PREDICTOR_INTER_HEADER_INCLUDED
#define PREDICTOR_INTER_HEADER_INCLUDED

#include "predictor.h"

namespace Predictor {
	class Nic : public Predictor {
	public:
		Nic () {};
		virtual ~Nic () {};

		virtual void forward_predict (Shared_data data) {(void)data;};
		virtual void reverse_predict (Shared_data data) {(void)data;};
	};



	class Left: public Predictor {
	public:
		Left () {};
		virtual ~Left () {};

		virtual void forward_predict (Shared_data data);
		virtual void reverse_predict (Shared_data data);
	};
	class Up: public Predictor {
	public:
		Up () {};
		virtual ~Up () {};

		virtual void forward_predict (Shared_data data);
		virtual void reverse_predict (Shared_data data);
	};
	class Diag: public Predictor {
	public:
		Diag () {};
		virtual ~Diag () {};

		virtual void forward_predict (Shared_data data);
		virtual void reverse_predict (Shared_data data);
	};



	class Med : public Predictor {
	public:
		Med () {};
		virtual ~Med () {};

		virtual void forward_predict (Shared_data data);
		virtual void reverse_predict (Shared_data data);
	};

	class Paeth: public Predictor {
	public:
		Paeth () {};
		virtual ~Paeth() {};

		virtual void forward_predict (Shared_data data);
		virtual void reverse_predict (Shared_data data);
	};






	class Gradient_adjusted : public Predictor {
	public:
		Gradient_adjusted () {};
		virtual ~Gradient_adjusted () {};

		virtual void forward_predict (Shared_data data);
		virtual void reverse_predict (Shared_data data);
	};

}

#endif // PREDICTOR_INTER_HEADER_INCLUDED
