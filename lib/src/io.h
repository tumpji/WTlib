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
#ifndef IO_HEADER_INCLUDED
#define IO_HEADER_INCLUDED

#include <string>
#include <fstream>
#include <cstdint>
#include <string>

#include <mutex>
#include <thread>
#include <condition_variable>

#include "wt_inter.h"

static_assert( sizeof(int) == sizeof(uint32_t), 
        "Je potreba provest upravu kodu/kompolace pro tuto platformu" );

class Bit_Buffer;

namespace WTio {
namespace Internal {

// zakldani trida pro zapis/cteni ze souboru
class IObase {
public:
    bool is_good () { return io.good(); }

protected:
    IObase () { io.exceptions( std::ifstream::failbit | std::ifstream::badbit ); }
    ~IObase () { if (io.is_open()) io.close(); }

    void open (std::string cesta, decltype(std::ios::binary) metoda) {
        try {
            io.exceptions( std::ifstream::failbit | std::ifstream::badbit );
            io.open(cesta.c_str(), metoda);
        } catch (...) {
            throw std::invalid_argument("Could not open a file");
        }
    }

    std::fstream io;
    friend class ::Bit_Buffer;
};

// zapis do souboru
class Writer : virtual public IObase {
protected:
    Writer () {}; ~Writer () {};

    void open (std::string cesta) { super::open(cesta, std::ios::out | std::ios::binary); }
public:
    void            put_u8  (unsigned char);
    void            put_int (int);
    void            write   (unsigned char* data, int size);
private:
    typedef IObase super;
};

// cteni ze souboru
class Reader : virtual public IObase {
protected:
    Reader () {}; ~Reader () {};

    void open (std::string cesta) { super::open(cesta, std::ios::in | std::ios::binary); }
public:
    unsigned char   get_u8   ();
    int             get_int  ();
    void            read (unsigned char* data, int size);
private:
    typedef IObase super;
};

} // Internal namespace


// cteni a zaroven zapis do souboru
class IO : public Internal::Writer, public Internal::Reader {
    public:
        IO (SMER smer, std::string cesta) : IObase() {
            write = (smer == SMER::KODER);
            if ( write ) Writer::open(cesta);
            else Reader::open(cesta);
        }

        void get_set_u8     (unsigned char &);
        void get_set_int    (int  &);
        // pozor na endianitu
        void get_set_data   (unsigned char *, int size);

    private:
        bool write;
};

class IO_thread : public IO {
    public:
        IO_thread(SMER smer, std::string cesta): 
            IO(smer, cesta),
            cur_thread(0), n_threads(0) //, free_threads(N_THREADS_AVAIABLE)
        { }
        ~IO_thread () { wait_for_all();}


        // vraci klic, ktery zajisti korektni cteni/zapis (ve spravnem poradi)
        // poradi je urceno volanim teto metody (proto je vhodne ji nedelat praral.)
        int get_key ();

        // cekani dokud tento klic neni na rade
        void open (int key);

        // signalizuje, ze klic uz nebude zapisovat do souboru a tim se dostava na radu dalsi
        void close (int key);

        // ceka na posledniho producenta/konzumenta (podle komprese/de)
        // vhodne v pripade vystupu z paralelni sekce
        void wait_for_all ();
    private:
        std::mutex                      mux;
        std::condition_variable_any     io_lock;

        volatile int        cur_thread;
        volatile int        n_threads;

        //std::condition_variable_any       thread_lock;
        //volatile int                  free_threads;
};

}






///////////////// INLINE definice /////////////////
//


namespace WTio {
    ////////////////////////////
    ////  IO

    inline void IO::get_set_u8 (unsigned char &a) {
        if ( write ) Writer::put_u8(a);
        else a = Reader::get_u8();
    }

    inline void IO::get_set_int (int &a) {
        if ( write ) Writer::put_int(a);
        else a = Reader::get_int();
    }

    inline void IO::get_set_data (unsigned char *data, int size ) {
        if ( write ) Writer::write(data, size);
        else Reader::read(data, size);
    }
} // end namespace WTio


namespace WTio { 
namespace Internal {

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// Writer 

inline void Writer::put_u8 (unsigned char a) {
    io.put( *reinterpret_cast<char*>(&a) );
}

inline void Writer::put_int (int cislo) {
    uint32_t dt = *reinterpret_cast<uint32_t*>(&cislo);
    unsigned char tmp = dt >> 24;
    io.put( *reinterpret_cast<char*>(&tmp) );
    tmp = dt >> 16;
    io.put( *reinterpret_cast<char*>(&tmp) );
    tmp = dt >> 8;
    io.put( *reinterpret_cast<char*>(&tmp) );
    tmp = dt;
    io.put( *reinterpret_cast<char*>(&tmp) );
}

inline void Writer::write (unsigned char * data, int size ) {
    io.write( reinterpret_cast<char*>(data), size );
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// Reader

inline unsigned char Reader::get_u8 () {
    char tmp = io.get();
    return *reinterpret_cast<unsigned char*>(&tmp);
}

inline int Reader::get_int () {
    char tmp = io.get();
    uint32_t dt = *reinterpret_cast<unsigned char*>(&tmp) << 24;
    tmp = io.get();
    dt += *reinterpret_cast<unsigned char*>(&tmp) << 16;
    tmp = io.get();
    dt += *reinterpret_cast<unsigned char*>(&tmp) << 8;
    tmp = io.get();
    dt += *reinterpret_cast<unsigned char*>(&tmp);

    return *reinterpret_cast<int32_t*>(&dt);
}

inline void Reader::read (unsigned char * data, int size ) {
    io.read(reinterpret_cast<char*>(data), size);
}

}} // end WTio::internal namespace

#endif // IO_HEADER_INCLUDED
