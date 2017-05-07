#include <array>

#include "model_sign.h"
namespace {

	// umoznuje kodovat pouze znamenka
	struct Sign_Model1 {
		Sign_Model1 () : numCountPlus(1), soucet(2) { }
		~Sign_Model1 () {}

		void encode ( Aritmeticky_koder & arm, int value ) { encode(arm, value < 0); }
		void encode ( Aritmeticky_koder & arm, bool value );

		bool decode ( Aritmeticky_koder & arm );

	private:
		unsigned int numCountPlus;
		unsigned int soucet;
	};

	void Sign_Model1::encode ( Aritmeticky_koder &arm, bool value ) {
		int low_count, high_count;

		if ( value ) {
			low_count = numCountPlus;
			high_count = soucet;
		} else {
			low_count = 0;
			high_count = numCountPlus;
		}

		arm.Encode( low_count, high_count, soucet );


		if ( not value )
			numCountPlus += 1;
		soucet += 1;
	}

	bool Sign_Model1::decode ( Aritmeticky_koder &arm ) {
		unsigned low_count, high_count;
		unsigned value = arm.DecodeTarget( soucet );

		bool ret = numCountPlus <= value;


		if ( ret ) {
			low_count = numCountPlus;
			high_count = soucet;
		} else {
			low_count = 0;
			high_count = numCountPlus;
		}

		arm.Decode( low_count, high_count );

		// update
		if ( not ret )
			numCountPlus += 1;
		soucet += 1;

		return ret;
	}
}


// 32 kontextu pro binarni ulozeni sign.
// leve znamenko / diag znamenko / horni znamenko / pasmo
struct SignModel_32 : public SignModel::SignModel_base {
	SignModel_32 () {};
	~SignModel_32 () {};

	virtual void 
		encode ( Environment*, Aritmeticky_koder &, Shared_data, OZNACENI_PASMA );

	virtual std::vector<bool> 
		decode ( Environment*, Aritmeticky_koder &, Shared_data, OZNACENI_PASMA );

private:
	Sign_Model1* get_sign_model( int levo, int diag, int horni, OZNACENI_PASMA pasmo ) {
		return get_sign_model(levo < 0, diag < 0, horni < 0, pasmo);
	}
	Sign_Model1* get_sign_model( bool, bool, bool, OZNACENI_PASMA pasmo );

	Sign_Model1	sign_models[4*2*2*2];  
};

Sign_Model1* SignModel_32::get_sign_model ( bool levo, bool diag, bool horni, OZNACENI_PASMA pasmo )
{
	unsigned index = 0;

	switch (pasmo) {
		default:
			assert(false);
			break;
		case OZNACENI_PASMA::BLUE:
		case OZNACENI_PASMA::LL:
			index += 1;
		case OZNACENI_PASMA::YELLOW:
		case OZNACENI_PASMA::LH:
			index += 1;
		case OZNACENI_PASMA::BLACK_BLUE:
		case OZNACENI_PASMA::HL:
			index += 1;
		case OZNACENI_PASMA::BLACK_YELLOW:
		case OZNACENI_PASMA::HH:
			break;
	}

	index <<= 1; if ( levo ) index += 1;
	index <<= 1; if ( diag ) index += 1;
	index <<= 1; if ( horni ) index += 1;
	assert( index < 4*2*2*2 );
	return &sign_models[index];
}

void SignModel_32::encode (
	Environment* env, 
	Aritmeticky_koder& arm, Shared_data data, OZNACENI_PASMA pasmo ) 
{
	const int rozmer_x = data->rozmer_x, rozmer_y = data->rozmer_y;
	int leva = 0, diag = 0, horni = 0, act = data->data[0];

	// prvni vzorek
	Sign_Model1 *a = get_sign_model(leva, diag, horni, pasmo);
	a->encode( arm, act );

	// prvni radek
	for ( int isloupec = 1; isloupec < data->rozmer_x; ++isloupec ) {
		leva = data->data[isloupec - 1];
		diag = horni = leva;
		act = data->data[isloupec];

		a = get_sign_model(leva, diag, horni, pasmo);
		a->encode( arm, act );
	}

	for ( int iradek = 1; iradek < data->rozmer_y; ++iradek ) {
		// prvni na radku
		act = data->data[iradek * rozmer_x];
		horni = data->data[(iradek-1) * rozmer_x];
		diag = leva = horni;

		a = get_sign_model(leva, diag, horni, pasmo);
		a->encode( arm, act );

		// ostatni plne funkcni
		for ( int isloupec = 1; isloupec < data->rozmer_x; ++isloupec ) {
			const int pozice = iradek*rozmer_x + isloupec;
			diag = data->data[pozice - rozmer_x - 1];	
			horni = data->data[pozice - rozmer_x];
			leva = data->data[pozice - 1];
			act = data->data[pozice];

			a = get_sign_model(leva, diag, horni, pasmo);
			a->encode( arm, act );
		}
	}
}

std::vector<bool> SignModel_32::decode( 
		Environment* env, Aritmeticky_koder &arm, Shared_data data, OZNACENI_PASMA pasmo )
{
	const int rozmer_x = data->rozmer_x, rozmer_y = data->rozmer_y;
	std::vector<bool> out_vec; 
	out_vec.reserve(rozmer_x*rozmer_y);
	bool leva = false, diag = false, horni = false, akt;
	int value;

	// prvni
	Sign_Model1 *a = get_sign_model(leva, diag, horni, pasmo);
	out_vec.push_back( a->decode(arm) );

	// prvni radek
	for ( int isloupec = 1; isloupec < rozmer_x; ++isloupec )
	{
		diag = horni = leva = *out_vec.rbegin();

		a = get_sign_model(leva, diag, horni, pasmo);
		out_vec.push_back( a->decode(arm) );
	}

	// ostatni radky
	for ( int iradek = 1; iradek < rozmer_y; ++iradek ) {
		// prvni na radku
		diag = leva = horni = out_vec[ (iradek-1)*rozmer_x ];

		a = get_sign_model(leva, diag, horni, pasmo);
		out_vec.push_back( a->decode(arm) );

		for ( int isloupec = 1; isloupec < rozmer_x; ++isloupec ) {
			const int pozice = iradek*rozmer_x + isloupec;
			diag =  out_vec[pozice - rozmer_x - 1];	
			horni = out_vec[pozice - rozmer_x];
			leva =  out_vec[pozice - 1];

			a = get_sign_model(leva, diag, horni, pasmo);
			out_vec.push_back( a->decode(arm) );
		}
	}

	assert( out_vec.size() == static_cast<unsigned>(rozmer_x * rozmer_y) );
	return out_vec;
}



namespace {
	SignModel_32   sign_32;
}

namespace SignModel{
	void init_sign_models () {
		sign_32 = SignModel_32();
	}

	SignModel_base* get_sign_model_by_type ( Type type ) {
		switch ( type ) {
			case T32:
				return &sign_32;
			default:
				assert(false);
				return nullptr;
		}
	}
}
