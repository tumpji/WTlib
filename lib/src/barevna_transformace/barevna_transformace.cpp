
#include <iostream>
#include <cassert>

#include "wt.h"
#include "barevna_transformace.h"
#include "std_macro.h"

using DATA = std::vector<std::shared_ptr<WTdata>>&; 


using WTlib::BAREVNY_PROSTOR;

namespace {

// nejprve kontrola
// rozmery prvnich 3 musi byt stejne (jestli nejsou chyba)
// jestli je pocet pasem < 3 automaticky pouzit NIC a warning
void provest_kontrolu_barevne_transofrmace 
	( BAREVNY_PROSTOR &prostor, const std::vector<Shared_data>& data) 
{
	unsigned number_check = 0;

	switch (prostor) {
		case BAREVNY_PROSTOR::RAW:
			number_check = 0;
			break;

		case BAREVNY_PROSTOR::RCT:
		case BAREVNY_PROSTOR::YCoCgR:
		case BAREVNY_PROSTOR::RDgDb:
		case BAREVNY_PROSTOR::LDgEb:
		case BAREVNY_PROSTOR::mRDgDb:
		case BAREVNY_PROSTOR::mLDgEb:
		case BAREVNY_PROSTOR::LOCOI:
			number_check = 3;
			break;

		default:
			throw std::invalid_argument( "Unknown color transform" );
			break;
	}

	if ( data.size() < number_check ) {
		std::cerr << "Warning: Cannot use color transform due to number of color components" << std::endl;
		prostor = BAREVNY_PROSTOR::RAW;
		return;
	}

	for (unsigned i = 1; i < number_check; ++i ) {
		if ( data[i-1]->rozmer_x   !=  data[i]->rozmer_x ||
			 data[i-1]->rozmer_y   !=  data[i]->rozmer_y   )
			throw std::invalid_argument( "First few color planes must have same size");
	}

	return;
}


void provest_rct (SMER smer, DATA data) {
	if (data.size() < 3) { // nelze
		return;
	}

	int  velikost = data[0]->rozmer_x * data[0]->rozmer_y;

	uint64_t suma = 0;
	uint64_t pocet = 0;

	if ( smer == SMER::KODER ) {
		auto& Rpasmo = data[0]->data;
		auto& Gpasmo = data[1]->data;
		auto& Bpasmo = data[2]->data;

		for (int i = 0; i < velikost; ++i )
		{
			int Y  = (Rpasmo[i] + 2*Gpasmo[i] + Bpasmo[i]) >> 2;
			int Cb = Bpasmo[i] - Gpasmo[i];
			int Cr = Rpasmo[i] - Gpasmo[i];

			Rpasmo[i] = Y;
			Gpasmo[i] = Cb;
			Bpasmo[i] = Cr;

			suma += Y;
			pocet += 1;
		}
	} else { // dekoder
		auto& Ypasmo  = data[0]->data;
		auto& Cbpasmo = data[1]->data;
		auto& Crpasmo = data[2]->data;

		for (int i = 0; i < velikost; ++i)
		{
			int G = Ypasmo[i] - ((Cbpasmo[i] + Crpasmo[i])>>2);
			int B = Cbpasmo[i] + G;
			int R = Crpasmo[i] + G;

			Ypasmo[i] = R;
			Cbpasmo[i] = G;
			Crpasmo[i] = B;
		}
	}


	return;
}

void provest_YCoCgR (SMER smer, DATA data) {
	if (data.size() < 3) { // nelze
		return;
	}

	int  velikost = data[0]->rozmer_x * data[0]->rozmer_y;

	if ( smer == SMER::KODER ) {
		auto& Rpasmo = data[0]->data;
		auto& Gpasmo = data[1]->data;
		auto& Bpasmo = data[2]->data;

		for (int i = 0; i < velikost; ++i )
		{
			int Co  = Rpasmo[i] - Bpasmo[i];
			int t   = Bpasmo[i] + (Co/2);
			int Cg = Gpasmo[i] - t;
			int Y = t + (Cg/2); 

			Rpasmo[i] = Y;
			Gpasmo[i] = Co;
			Bpasmo[i] = Cg;
		}
	} else { // dekoder
		auto& Ypasmo  = data[0]->data;
		auto& Copasmo = data[1]->data;
		auto& Cgpasmo = data[2]->data;

		for (int i = 0; i < velikost; ++i)
		{
			int t = Ypasmo[i] - (Cgpasmo[i]/2);
			int G = Cgpasmo[i] + t;
			int B = t - (Copasmo[i]/2);
			int R = B + Copasmo[i];

			Ypasmo[i]  = R;
			Copasmo[i] = G;
			Cgpasmo[i] = B;
		}
	}
	return;
}

void provest_RDgDb (SMER smer, DATA data) {
	if (data.size() < 3) { // nelze
		return;
	}

	int  velikost = data[0]->rozmer_x * data[0]->rozmer_y;

	if ( smer == SMER::KODER ) {
		auto& Rpasmo = data[0]->data;
		auto& Gpasmo = data[1]->data;
		auto& Bpasmo = data[2]->data;

		for (int i = 0; i < velikost; ++i )
		{
			int R  = Rpasmo[i];
			int Dg = R - Gpasmo[i];
			int Db = Gpasmo[i] - Bpasmo[i];

			Rpasmo[i] = R;
			Gpasmo[i] = Dg;
			Bpasmo[i] = Db;
		}
	} else { // dekoder
		auto& Rpasmo  = data[0]->data;
		auto& Dgpasmo = data[1]->data;
		auto& Dbpasmo = data[2]->data;

		for (int i = 0; i < velikost; ++i)
		{
			int R = Rpasmo[i];
			int G = R - Dgpasmo[i];
			int B = G - Dbpasmo[i];

			Rpasmo[i] = R;
			Dgpasmo[i] = G;
			Dbpasmo[i] = B;
		}
	}
	return;
}

void provest_LDgEb (SMER smer, DATA data) {
	if (data.size() < 3) { // nelze
		return;
	}

	int  velikost = data[0]->rozmer_x * data[0]->rozmer_y;

	if ( smer == SMER::KODER ) {
		auto& Rpasmo = data[0]->data;
		auto& Gpasmo = data[1]->data;
		auto& Bpasmo = data[2]->data;

		for (int i = 0; i < velikost; ++i )
		{
			int Dg = Rpasmo[i] - Gpasmo[i];
			int L  = Rpasmo[i] - (Dg/2);
			int Eb = Bpasmo[i] - L;

			Rpasmo[i] = L;
			Gpasmo[i] = Dg;
			Bpasmo[i] = Eb;
		}
	} else { // dekoder
		auto& Lpasmo  = data[0]->data;
		auto& Dgpasmo = data[1]->data;
		auto& Ebpasmo = data[2]->data;

		for (int i = 0; i < velikost; ++i)
		{
			int R = Lpasmo[i] + Dgpasmo[i]/2;
			int G = R - Dgpasmo[i];
			int B = Ebpasmo[i] + Lpasmo[i];

			Lpasmo[i] = R;
			Dgpasmo[i] = G;
			Ebpasmo[i] = B;
		}
	}
	return;
}

void provest_LOCOI (SMER smer, DATA data) {
	if (data.size() < 3) { // nelze
		return;
	}

	int  velikost = data[0]->rozmer_x * data[0]->rozmer_y;

	if ( smer == SMER::KODER ) {
		auto& Rpasmo = data[0]->data;
		auto& Gpasmo = data[1]->data;
		auto& Bpasmo = data[2]->data;

		for (int i = 0; i < velikost; ++i )
		{
			int G  = Gpasmo[i];
			int Dg = Rpasmo[i] - Gpasmo[i];
			int Cu = Bpasmo[i] - Gpasmo[i];

			Rpasmo[i] = G;
			Gpasmo[i] = Dg;
			Bpasmo[i] = Cu;
		}
	} else { // dekoder
		auto& Gpasmo  = data[0]->data;
		auto& Dgpasmo = data[1]->data;
		auto& Cupasmo = data[2]->data;

		for (int i = 0; i < velikost; ++i)
		{
			int R = Dgpasmo[i] + Gpasmo[i];
			int G = Gpasmo[i];
			int B = Cupasmo[i] + G;

			Gpasmo[i] = R;
			Dgpasmo[i] = G;
			Cupasmo[i] = B;
		}
	}
	return;
}

int vypocet_smod_fw( int a , int b ) {
	return ((a + b/2) % b) - b/2;
}
int vypocet_mod_bw( int a , int b ) {
	return (a + 2*b) % b;
}

void provest_mRDgDb(SMER smer, DATA data) {
	if (data.size() < 3) { // nelze
		return;
	}

	int  velikost = data[0]->rozmer_x * data[0]->rozmer_y;

	if ( smer == SMER::KODER ) {
		auto& Rpasmo = data[0]->data;
		auto& Gpasmo = data[1]->data;
		auto& Bpasmo = data[2]->data;

		for (int i = 0; i < velikost; ++i )
		{
			if ( MAX(MAX(Rpasmo[i], Gpasmo[i]), Bpasmo[i]) >= 256 )
				throw std::invalid_argument("mRDgDb transformation requires 8bit values");

			int R   = Rpasmo[i];
			int mDg = vypocet_smod_fw(Rpasmo[i] - Gpasmo[i], 256);
			int mDb = vypocet_smod_fw(Gpasmo[i] - Bpasmo[i], 256);

			Rpasmo[i] = R;
			Gpasmo[i] = mDg;
			Bpasmo[i] = mDb;
		}
	} else { // dekoder
		auto& Rpasmo  = data[0]->data;
		auto& mDgpasmo = data[1]->data;
		auto& mDbpasmo = data[2]->data;

		for (int i = 0; i < velikost; ++i)
		{
			int R = Rpasmo[i];
			int G = (R - mDgpasmo[i]) % 256;
			int B = (G - mDbpasmo[i]) % 256;

			Rpasmo[i] = R;
			mDgpasmo[i] = G;
			mDbpasmo[i] = B;
		}
	}
	return;
}

void provest_mLDgEb(SMER smer, DATA data) {
	if (data.size() < 3) { // nelze
		return;
	}

	int  velikost = data[0]->rozmer_x * data[0]->rozmer_y;

	if ( smer == SMER::KODER ) {
		auto& Rpasmo = data[0]->data;
		auto& Gpasmo = data[1]->data;
		auto& Bpasmo = data[2]->data;

		for (int i = 0; i < velikost; ++i )
		{
			if ( MAX(MAX(Rpasmo[i], Gpasmo[i]), Bpasmo[i]) >= 256 )
				throw std::invalid_argument("mRDgDb transformation requires 8bit values");

			int mDg = vypocet_smod_fw(Rpasmo[i] - Gpasmo[i], 256); 
			//int mL  = vypocet_smod_fw(Rpasmo[i] - mDg/2, 256);
			int mL  = vypocet_mod_bw(Rpasmo[i] - (mDg>>1), 256);
			int mEb = vypocet_smod_fw(Bpasmo[i] - mL, 256);

			Rpasmo[i] = mL;
			Gpasmo[i] = mDg;
			Bpasmo[i] = mEb;
		}
	} else { // dekoder
		auto& mLpasmo  = data[0]->data;
		auto& mDgpasmo = data[1]->data;
		auto& mEbpasmo = data[2]->data;

		for (int i = 0; i < velikost; ++i)
		{
			int R = vypocet_mod_bw(mLpasmo[i] + (mDgpasmo[i]>>1), 256);
			int G = vypocet_mod_bw(R - mDgpasmo[i], 256);
			int B = vypocet_mod_bw(mEbpasmo[i] + mLpasmo[i], 256);

			mLpasmo[i] = R;
			mDgpasmo[i] = G;
			mEbpasmo[i] = B;
		}
	}
	return;
}




} // end unnamed namespace



void provest_barevnou_transformaci (
		SMER									smer,
		WTlib::BAREVNY_PROSTOR					prostor,
		std::vector<Shared_data>&	data )
{
	provest_kontrolu_barevne_transofrmace(prostor, data);

	switch ( prostor ) {
		case WTlib::BAREVNY_PROSTOR::RAW:
			break;
		case WTlib::BAREVNY_PROSTOR::RCT:
			::provest_rct(smer, data);
			break;
		case WTlib::BAREVNY_PROSTOR::YCoCgR:
			::provest_YCoCgR(smer, data);
			break;
		case WTlib::BAREVNY_PROSTOR::RDgDb:
			::provest_RDgDb(smer, data);
			break;
		case WTlib::BAREVNY_PROSTOR::LDgEb:
			::provest_LDgEb(smer, data);
			break;
		case WTlib::BAREVNY_PROSTOR::mRDgDb:
			::provest_mRDgDb(smer, data);
			break;
		case WTlib::BAREVNY_PROSTOR::mLDgEb:
			::provest_mLDgEb(smer, data);
			break;
		case WTlib::BAREVNY_PROSTOR::LOCOI:
			::provest_LOCOI(smer, data);
			break;

		default:
			std::invalid_argument("This color transform is not implemented yet.");
			break;
	}

}

