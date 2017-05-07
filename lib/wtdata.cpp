
#include "wt.h"

namespace WTlib {

WTdata::WTdata 
(int rozmer_x, int rozmer_y, Allocation_options alloc): 
	rozmer_x(rozmer_x), rozmer_y(rozmer_y) 
{
	assert(rozmer_x > 0);
	assert(rozmer_y > 0);
	if ( alloc == Allocation_options::ALLOCATE )
		this->data.resize(rozmer_x*rozmer_y, 0);
}

WTdata::WTdata 
( std::string oznaceni, int rozmer_x, int rozmer_y, Allocation_options alloc )
	: WTdata(rozmer_x, rozmer_y, alloc )
{
	this->oznaceni = oznaceni;
}

} // end namespace WTlib
