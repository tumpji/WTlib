/*
 *  This file is part of WTlib.
 *
 *  WTlib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  WTlib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with WTlib.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "main.h"
#include "aritmeticky.h"

#include <iostream>

Bit_Buffer::Bit_Buffer ( Environment * env ) : env(env) {
	cache = 0;
	cache_count = 0;
	input_len = 0;

	// ziska se pozice (na konci se zapise velikost)
	init_position = env->io.tellg();

	// je potreba nacist velikost
	if ( env->smer == SMER::DEKODER ) {
		input_len = env->get_int();
	} else {
		env->put_int(0);
	}
}

Bit_Buffer::~Bit_Buffer () {
	if ( env->smer == SMER::KODER ) {
		env->io.seekg(init_position);
		env->put_int(input_len);
		env->io.seekg(0, env->io.end);
	}
}

void Bit_Buffer::SetBitFlush() {
	while( cache_count != 0 )
		SetBit( 0 );
}

void Bit_Buffer::SetBit( const unsigned char bit )
{
	cache = (cache << 1) | bit;
	cache_count++;

	if(cache_count == 8) // buffer full
	{
		// write
		env->put_u8(cache);
		cache_count = 0;
		input_len += 1;
	}
}
unsigned char Bit_Buffer::GetBit()
{
	if ( cache_count == 0) // buffer empty
	{
		if ( input_len > 0 ) {
			cache = env->get_u8();
			input_len -= 1;
		}
		else
			cache = 0;
		cache_count = 8;
	}

	// extract bit from buffer
	unsigned char b = cache >> 7;
	cache <<= 1;
	cache_count--;

	return b;
}












// constants to split the number space of 32 bit integers
// most significant bit kept free to prevent overflows

static const unsigned int g_FirstQuarter = 0x20000000;
static const unsigned int g_ThirdQuarter = 0x60000000;
static const unsigned int g_Half		  = 0x40000000;

Aritmeticky_koder::Aritmeticky_koder (Environment* env):
	bit_buffer(env)
{
	mLow = 0;
	mHigh = 0x7FFFFFFF; // just work with least significant 31 bits

	mScale = 0;
	mBuffer = 0;
	mStep = 0;
}

Aritmeticky_koder::~Aritmeticky_koder () {
}




void Aritmeticky_koder::Encode( 
		const unsigned int low_count,
		const unsigned int high_count,
		const unsigned int total )
	// total < 2ˆ29
{
	// partition number space into single steps
	mStep = ( mHigh - mLow + 1 ) / total; // interval open at the top => +1
	// update upper bound
	mHigh = mLow + mStep * high_count - 1; // interval open at the top => -1
	// update lower bound
	mLow = mLow + mStep * low_count;
	// apply e1/e2 scaling
	while( ( mHigh < g_Half ) || ( mLow >= g_Half ) )
	{
		if( mHigh < g_Half )
		{
			bit_buffer.SetBit( 0 );
			mLow = mLow * 2;
			mHigh = mHigh * 2 + 1;
			// perform e3 scalings
			for(; mScale > 0; mScale-- )
				bit_buffer.SetBit( 1 );
		}
		else if( mLow >= g_Half )
		{
			bit_buffer.SetBit( 1 );
			mLow = 2 * ( mLow - g_Half );
			mHigh = 2 * ( mHigh - g_Half ) + 1;
			// perform e3 scalings
			for(; mScale > 0; mScale-- )
				bit_buffer.SetBit( 0 );
		}
	}
	// e3
	while( ( g_FirstQuarter <= mLow ) && ( mHigh < g_ThirdQuarter ) )
	{
		// keep necessary e3 scalings in mind
		mScale++;
		mLow = 2 * ( mLow - g_FirstQuarter );
		mHigh = 2 * ( mHigh - g_FirstQuarter ) + 1;
	}
}


void Aritmeticky_koder::EncodeFinish()
{
	// There are two possibilities of how mLow and mHigh can be distributed,
	// which means that two bits are enough to distinguish them.
	if( mLow < g_FirstQuarter ) // mLow < FirstQuarter < Half <= mHigh
	{
		bit_buffer.SetBit( 0 );
		for( unsigned i=0; i<mScale+1; i++ ) // perform e3-scaling
			bit_buffer.SetBit(1);
	}
	else // mLow < Half < ThirdQuarter <= mHigh
	{
		bit_buffer.SetBit( 1 ); // zeros added automatically by the decoder; no need to send them
	}

	// empty the output buffer
	bit_buffer.SetBitFlush();
}


void Aritmeticky_koder::DecodeStart()
{
	// Fill buffer with bits from the input stream
	for( int i=0; i<31; i++ ) // just use the 31 least significant bits
		mBuffer = ( mBuffer << 1 ) | bit_buffer.GetBit();
}

unsigned int Aritmeticky_koder::DecodeTarget( const unsigned int total )
{
	// total < 2ˆ29
	// split number space into single steps
	mStep = ( mHigh - mLow + 1 ) / total; // interval open at the top => +1
	// return current value
	return ( mBuffer - mLow ) / mStep;
}


void Aritmeticky_koder::Decode( const unsigned int low_count,
		const unsigned int high_count )
{
	// update upper bound
	mHigh = mLow + mStep * high_count - 1; // interval open at the top => -1
	// update lower bound
	mLow = mLow + mStep * low_count;
	// e1/e2 scaling
	while( ( mHigh < g_Half ) || ( mLow >= g_Half ) )
	{
		if( mHigh < g_Half )
		{
			mLow = mLow * 2;
			mHigh = mHigh * 2 + 1;
			mBuffer = 2 * mBuffer + bit_buffer.GetBit();
		}
		else if( mLow >= g_Half )
		{
			mLow = 2 * ( mLow - g_Half );
			mHigh = 2 * ( mHigh - g_Half ) + 1;
			mBuffer = 2 * ( mBuffer - g_Half ) + bit_buffer.GetBit();
		}
		mScale = 0;
	}
	// e3 scaling
	while( ( g_FirstQuarter <= mLow ) && ( mHigh < g_ThirdQuarter ) )
	{
		mScale++;
		mLow = 2 * ( mLow - g_FirstQuarter );
		mHigh = 2 * ( mHigh - g_FirstQuarter ) + 1;
		mBuffer = 2 * ( mBuffer - g_FirstQuarter ) + bit_buffer.GetBit();
	}
}


