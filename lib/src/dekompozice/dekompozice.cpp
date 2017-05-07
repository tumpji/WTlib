
#include "dekompozice_internal.h"


Shared_command 
vyber_dekompozice (Environment* env) 
{
	using namespace Dekompozice;
	const auto &max_level_dekompozice = env->nastaveni->dekompozice_max;

	switch( env->nastaveni->dekompozice ) {

		case WTlib::DEKOMPOZICE::PYRAMIDOVA:
			return std::make_shared<Dek_pyramidova>(max_level_dekompozice);
			break;

		case WTlib::DEKOMPOZICE::RED_BLACK:
			return std::make_shared<Dek_red_black>(max_level_dekompozice);
			break;

		default:
			throw std::invalid_argument(
					"Tato dekompozice neni implementovana");
			break;
	}
}
