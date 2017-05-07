#ifndef PICS_CONVERTER_HEADER_INCLUDED
#define PICS_CONVERTER_HEADER_INCLUDED

#include <vector>
#include <memory>

#include "wt.h"





// otevre obrazek v .png
std::vector<WTlib::WTdata> 
load_obrazek (std::string cesta);

// ulozi obrazek do .png
void 
save_obrazek (std::string cesta, std::vector<WTlib::WTdata> src);


// vraci false pro shodne data
bool 
check_obrazek( const Obrazek& , const Obrazek& );


#endif // PICS_CONVERTER_HEADER_INCLUDED
