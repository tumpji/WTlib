#include "aritmeticky.h"

/*
namespace { 

template < unsigned Value >
class NthModel {
	NthModel () { 
		soucet = Value;
		for ( unsigned i = 0; i < Value; ++i )
			numCount[i] = 1;
	}

	void encode ( Aritmeticky_koder & arm, unsigned value ) {
		assert( value < Value );
		unsigned low_count = 0, high_count = numCount[value];
		for ( unsigned i = 0; i < value; ++i )
			low_count += numCount[i];
		high_count += low_count;

		arm.Encode( low_count, high_count, soucet );
		numCount[value] += 1;
		soucet += 1;
	}

	unsigned decode ( Aritmeticky_koder & arm ) {
		unsigned low_count = 0, symbol = 0;
		unsigned value = arm.DecodeTarget( soucet );
		for ( ; low_count + numCount[symbol] <= value; ++symbol )
			low_count += numCount[symbol];
		arm.Decode( low_count, low_count + numCount[symbol] );
		numCount[symbol] += 1;
		soucet += 1;
		assert( symbol < Value );
		return symbol;
	}

private:
	unsigned soucet;
	unsigned numCount[Value];
};

 std::array< NthModel<2> >  dva[6];
 std::array< NthModel<3> >  tri[4];
 std::array< NthModel<4> >  ctyri[1];

}

class AritmPredictor {
};
*/
