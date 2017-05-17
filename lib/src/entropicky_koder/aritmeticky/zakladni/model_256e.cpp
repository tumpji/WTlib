/*
 *  This file is part of WTlib.
 *
 *  WTlib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  WTlib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with WTlib.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "std_macro.h"

#include "models.h"
#include "model_256e.h"

namespace {

template<unsigned int N>
struct ValueModel_internal : public ValueModel::ValueModel_base {
	static const unsigned BITS = N;
	static const unsigned MAX_VALUE = (1 << BITS) - 1; // 255
	static const unsigned E_VALUE = 1 << BITS; // 256

	ValueModel_internal (); 
	~ValueModel_internal () {};

	void encode ( Aritmeticky_koder &, unsigned value );
	unsigned decode ( Aritmeticky_koder& );

	virtual unsigned get_e_value () const {return E_VALUE;}
	virtual unsigned get_bits () const {return BITS;}
private:
	unsigned int numCount [E_VALUE + 1]; // 1 == shift << 8 and add
};

typedef ValueModel_internal< 4> ValueModel_16 ;
typedef ValueModel_internal< 5> ValueModel_32 ;
typedef ValueModel_internal< 6> ValueModel_64 ;
typedef ValueModel_internal< 7> ValueModel_128;
typedef ValueModel_internal< 8> ValueModel_256 ;
typedef ValueModel_internal< 9> ValueModel_512 ;
typedef ValueModel_internal<10> ValueModel_1024;



////////////////////////////////////


template<unsigned int N>
ValueModel_internal<N>::ValueModel_internal () {
	for ( unsigned x = 0; x < E_VALUE + 1; ++x )
		numCount[x] = 1;
	soucet = E_VALUE + 1;
}

template<unsigned int N>
void ValueModel_internal<N>::encode ( Aritmeticky_koder& koder, unsigned value ) {
	assert( value <= E_VALUE );

	unsigned low_count = 0, high_count = 0;

	for ( unsigned i = 0; i < value; i++ )
		low_count += numCount[i];

	high_count = low_count + numCount[value];

	koder.Encode( low_count, high_count, soucet );

	numCount[value] += 1;
	soucet += 1;
}

template<unsigned int N>
unsigned ValueModel_internal<N>::decode ( Aritmeticky_koder& koder ) {
	unsigned low_count = 0, symbol = 0;

	unsigned value = koder.DecodeTarget( soucet );

	// hledani
	for ( ; low_count + numCount[symbol] <= value; symbol++ )
	   low_count += numCount[symbol];	

	unsigned high_count = low_count + numCount[symbol];

	koder.Decode( low_count, high_count );

	numCount[ symbol ]  += 1;
	soucet += 1;

	assert( symbol <= E_VALUE );
	return symbol;
}


} // end namespace 




namespace ValueModel {
	namespace {
		ValueModel_16    value_model_16;
		ValueModel_32    value_model_32;
		ValueModel_64    value_model_64;
		ValueModel_128   value_model_128;
		ValueModel_256   value_model_256;
		ValueModel_512   value_model_512;
		ValueModel_1024  value_model_1024;
	}

	ValueModel_base * get_value_model_by_type ( Type type ) {
		switch ( type ) {
			case T16:
				return &value_model_16;
			case T32:
				return &value_model_32;
			case T64:
				return &value_model_64;
			case T128:
				return &value_model_128;
			case T256:
				return &value_model_256;
			case T512:
				return &value_model_512;
			case T1024:
				return &value_model_1024;
			default:
				assert(false);
				return nullptr;
				break;
		};
	}

	void init_value_models () {
		value_model_256 = ValueModel_256();
		value_model_512 = ValueModel_512();
		value_model_1024= ValueModel_1024();
	}
	

}
