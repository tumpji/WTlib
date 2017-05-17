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
#ifndef ARITMETICKY_KODER_HEADER_INCLUDED
#define ARITMETICKY_KODER_HEADER_INCLUDED
#include "entropicky_koder.h"

class Bit_Buffer {
public:
    // pouze pro cteni
    Bit_Buffer ( Environment * env );
    ~Bit_Buffer ();


    // prida bit do bufferu
    void SetBit         ( const unsigned char bit );

    // preda bit
    unsigned char GetBit();

    // flush  ( vystupni )
    void SetBitFlush    ();

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



