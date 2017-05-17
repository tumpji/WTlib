#include <cassert>

#include "vlnka.h"
#include "vlnka_internal.h"
#include "main.h"


std::shared_ptr<Vlnka> get_vlnka (Environment* env) {
	using WTlib::VLNKA;

	switch(env->nastaveni->vlnka) {

		case VLNKA::CDF_5_3:
			return std::make_shared<CDF_5_3::Vlnka>();

		case VLNKA::CDF_5_3p:
			return std::make_shared<CDF_5_3p::Vlnka>();

		case VLNKA::CDF_5_3pm:
			return std::make_shared<CDF_5_3pm::Vlnka>();




		case VLNKA::CDF_9_7:
			return std::make_shared<CDF_9_7_F::Vlnka>();

		case VLNKA::HAAR:
			return std::make_shared<HAAR::Vlnka>();


		case VLNKA::v1_3:
			return std::make_shared<LINEAR::v3::Vlnka>();
		case VLNKA::v1_3p:
			return std::make_shared<LINEAR::v3p::Vlnka>();
		case VLNKA::v1_7:
			return std::make_shared<LINEAR::v7::Vlnka>();
		case VLNKA::v1_7m:
			return std::make_shared<LINEAR::v7m::Vlnka>();



		case VLNKA::CDF_9_7_M:
			return std::make_shared<CDF_9_7_M::Vlnka>();
		case VLNKA::CDF_9_7_Mm:
			return std::make_shared<CDF_9_7_Mm::Vlnka>();

		case VLNKA::v2_6:
			return std::make_shared<V_2_6::Vlnka>();

		case VLNKA::v2_10:
			return std::make_shared<V_2_10::Vlnka>();

		case VLNKA::v5_11_C:
			return std::make_shared<V_5_11_C::Vlnka>();

		case VLNKA::v5_11_A:
			return std::make_shared<V_5_11_A::Vlnka>();

		case VLNKA::v6_14:
			return std::make_shared<V_6_14::Vlnka>();

		case VLNKA::v13_7_T:
			return std::make_shared<V_13_7_T::Vlnka>();

		case VLNKA::v13_7_C:
			return std::make_shared<V_13_7_C::Vlnka>();




		default:
			std::invalid_argument("Wavelet is not implemented yet");
			break;
	}

	return std::shared_ptr<Vlnka>(nullptr); // aby byl kompilator spokojeny
}
