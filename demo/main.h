#ifndef MAIN_HADER_INCLUDED
#define MAIN_HADER_INCLUDED

#include <string>
#include <vector>

#include "wt.h"     // moje knihovna

typedef struct Parse_opts {
	// dekoder => true | koder => false
	bool				decoder_switch;
	// it is test
	bool				test_switch;
	// only opens the picture and save it as a png to different file 
	bool				opensave_switch;
	// affter compression it tries to open encrypted file and check if content match
	bool				check_switch;

	// method used in compression
	WTlib::WTcommon		metoda;

} Parse_opts;

typedef std::vector<WTlib::WTdata> Obrazek;





#endif // MAIN_HADER_INCLUDED
