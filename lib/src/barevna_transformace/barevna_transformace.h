#ifndef WT_BAREVNA_TRANSFORMACE_INCLUDED
#define WT_BAREVNA_TRANSFORMACE_INCLUDED

#include <vector>
#include <memory>

#include "main.h"
#include "wt.h"

// provede barevnou transformaci 
// samo si hleda smer
void provest_barevnou_transformaci (
		SMER,
		WTlib::BAREVNY_PROSTOR,
		std::vector<Shared_data>& );


#endif // WT_BAREVNA_TRANSFORMACE_INCLUDED
