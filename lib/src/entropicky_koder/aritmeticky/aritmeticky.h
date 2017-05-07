#ifndef ARITMETICKY_KODER_HEADER_INCLUDED
#define ARITMETICKY_KODER_HEADER_INCLUDED
#include "entropicky_koder.h"

class Bit_Buffer {
public:
	// pouze pro cteni
	Bit_Buffer ( Environment * env );
	~Bit_Buffer ();


	// prida bit do bufferu
	void SetBit			( const unsigned char bit );

	// preda bit
	unsigned char GetBit();

	// flush  ( vystupni )
	void SetBitFlush	();

private:
	// buffer pro vystupni bity
	unsigned char cache;
	// pocet bitu v bufferu
	unsigned char cache_count;

	Environment *env;
	// informuje o velikosti zapsaneho / cteneho 
	int input_len;
	int init_position;
};

class Aritmeticky_koder {
public:
	Aritmeticky_koder ( Environment* );
	~Aritmeticky_koder ();

	void DecodeStart();
	// zjisi cislo
	unsigned int DecodeTarget( const unsigned int total );
	// pro update
	void Decode( 
		const unsigned int low_count, 
		const unsigned int high_count );

	// koduje podle intervalu a souctu
	void Encode( 
		const unsigned int low_count, 
		const unsigned int high_count,
		const unsigned int total );
	// provede flush
	void EncodeFinish();


private:
	Bit_Buffer bit_buffer;
	// encoder & decoder
	unsigned int mLow;
	unsigned int mHigh;
	unsigned int mStep;
	unsigned int mScale;
	// decoder
	unsigned int mBuffer;
};

#endif // ARITMETICKY_KODER_HEADER_INCLUDED



