#ifndef WT_INTER_HEADER_INCLUDED
#define WT_INTER_HEADER_INCLUDED

#include "wt.h"

using WTlib::WTdata;
using WTlib::WTencoder;
using WTlib::WTdecoder;
using WTlib::WTcommon;
using WTlib::Environment;
using WTlib::OZNACENI_PASMA;
using WTlib::KODOVACI_MOZNOST;
using WTlib::KODER;

enum class SMER {
	KODER, 
	DEKODER
};


//#define N_THREADS_AVAIABLE 16
#define N_THREADS_AVAIABLE 1

#endif // WT_INTER_HEADER_INCLUDED
