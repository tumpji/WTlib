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

#include <cstring>
#include <mutex>

#include "main.h"
#include "entropicky_koder.h"

#include "mqc.h"

#include "ebcot_dec.h"
#include "ebcot_enc.h"
#include "ent_ebcot.h"

#include "sber_statistik.h"
#include "work_manager.h"

#define MIN(x,y) ( ((x)<(y))?(x):(y) )

namespace {
	/*
	void print_context ( 
			Jpeg2000T1Context* t1, int width, int height, int posun ) {
		std::cout << "Data: " << " stride: " << t1->stride << std::endl;

		for ( int i = 0; i < width*height ; ++i )
			std::cout << std::dec << (t1->data[i] >> posun) << " ";
		std::cout << std::endl;
	}
	*/

	std::shared_ptr<Jpeg2000Cblk>
	encode_tile ( 
	  const int *data, // MqcState *mqc,
	  const int dtstart,
	  const int stride, const int width, const int height, 
	  const int bandpos )
	{
		MqcState mqc;
		Jpeg2000T1Context t1;
		std::shared_ptr<Jpeg2000Cblk> cblk (new Jpeg2000Cblk);

		assert(bandpos >= 0 and bandpos < 4);

		memset(&mqc, 0 , sizeof(mqc) );
		memset(&t1, 0 , sizeof(t1) );
		memset(cblk.get(), 0, sizeof(Jpeg2000Cblk));

		t1.stride = width;

		// naplneni dat
		int * ptr_out = t1.data;
		for ( int radek = 0; radek < height; ++radek )
		{
			int i_radek = dtstart + radek*stride;
			for ( int sloupec = 0; sloupec < width; ++sloupec )
			{
				*ptr_out++ = data[i_radek++] << NMSEDEC_FRACBITS;
			}
		}

		encode_cblk(&t1, &mqc, cblk.get(), width, height, bandpos );
		ff_mqc_flush(&mqc);
		cblk->length = ff_mqc_length(&mqc);
		return cblk;
	}

	void decode_tile (
		int *data,
		std::shared_ptr<Jpeg2000Cblk> cblk, //MqcState *mqc,
		const int dtstart,
		const int stride, const int width, const int height, const int bandpos )
	{
		Jpeg2000T1Context t1;
		MqcState mqc;

		memset(&t1, 0, sizeof(Jpeg2000T1Context));
		t1.stride = width;
		memset(&mqc, 0, sizeof(MqcState));

		decode_cblk(&t1, &mqc, cblk.get(), width, height, bandpos);

		// ulozeni z5
		int *ptr_src = t1.data;
		for ( int radek = 0; radek < height; ++radek )
		{
			int i_radek = dtstart + radek*stride;
			for ( int sloupec = 0; sloupec < width; ++sloupec )
			{
				data[i_radek++] = *(ptr_src++) / 2;
			}
		}

		//print_context(&t1, width, height, 0);
	}

	/*
	void print_cblk (Jpeg2000Cblk * cblk) {
		std::cout << "CBLK: " << "npasses: " << (int)cblk->npasses << " "
		   	<< "nonzerobits: " << (int)cblk->nonzerobits << " " 
			<< "length: " << (int)cblk->length << std::endl;
		for (int i = 0; i < cblk->length+1; ++i )
		{
			std::cout << std::hex << (int)cblk->data[i] << " ";
		}
		std::cout << std::endl << "CBLK end" << std::endl;
	}*/

	// ulozi/nacte cblk z pameti
	void get_set_cblk ( Environment* env, std::shared_ptr<Jpeg2000Cblk> cblk, int key)
	{
		env->open(key); // zacatek zapisu => ceka na povoleni

		assert( cblk->length < sizeof(cblk->data) );
		unsigned char tmp;
		int tmp_int;

		// 1. ulozit npasses
		assert(cblk->npasses >= 0 && cblk->npasses < 256);
		tmp = static_cast<unsigned char>(cblk->npasses);
		env->get_set_u8(tmp);
		cblk->npasses = tmp;

		// 2. ulozit nonzerobits
		assert(cblk->nonzerobits >= 0 && cblk->nonzerobits < 256);
		tmp = static_cast<unsigned char>(cblk->nonzerobits);
		env->get_set_u8(tmp);
		cblk->nonzerobits = tmp;

		// 3. ulozit velikost dat
		tmp_int = cblk->length;
		env->get_set_int( tmp_int );
		cblk->length = tmp_int;

		// 4. ulozit data
		static_assert( sizeof(cblk->data[0]) == 1, "nutne zmenit kvuli endianite" );
		env->get_set_data( cblk->data, cblk->length );

		env->close(key); // konec zapisu => uz se nepotrebuje klic
	}

	int to_bandpos ( KODOVACI_MOZNOST moznost ) {
		switch(moznost) {
			case KODOVACI_MOZNOST::EBCOT_LL: return 0;
			case KODOVACI_MOZNOST::EBCOT_LH: return 1;
			case KODOVACI_MOZNOST::EBCOT_HL: return 2;
			case KODOVACI_MOZNOST::EBCOT_HH: return 3;
			default:
				throw std::invalid_argument("This ebcot variant is not implemented");
		}
		return 0;
	}


	int encode_thread ( Environment* env, int key, OZNACENI_PASMA pasmo, 
		int *data, int dt_start, int stride, int width, int height, int bpno )
	{
		auto cblk = ::encode_tile( data, 
				dt_start, stride, width, height, bpno );

		::get_set_cblk(env, cblk, key);

		stat::vlozit_velikost_zakodovaneho_pasma(env, pasmo, cblk->length);
	}

	int decode_thread ( Environment* env, int key,
		 int *data, int dt_start, int stride, int s_width, int s_height, int bpno )
	{
		auto cblk = std::make_shared<Jpeg2000Cblk>();
		memset(cblk.get(), 0, sizeof(Jpeg2000Cblk));
		get_set_cblk(env, cblk, key);

		::decode_tile(data, cblk,
			dt_start, stride, s_width, s_height, bpno );
	}

	class Work_manager_ebcot : public Work_manager {
	public:
		Work_manager_ebcot (
				Environment* env, int bpno, int*data, int rozmer_x, int rozmer_y ) :
			env(env), bpno(bpno), rozmer_x(rozmer_x), rozmer_y(rozmer_y), 
			stride(rozmer_x), data(data),
				strop(0), zed(0)
		{ }
		Work_manager_ebcot(Environment* env, OZNACENI_PASMA pasmo, int bpno, int *data, 
				int rozmer_x, int rozmer_y ):
			Work_manager_ebcot(env, bpno, data, rozmer_x, rozmer_y )
		{ this->pasmo = pasmo; }
	private:
		static const int normal_tile_size = 64;

		Environment* env;
		int rozmer_x, rozmer_y, stride, bpno;
		int *data;
		OZNACENI_PASMA pasmo;
	
	private: // implementace cyklu
		volatile int strop; // na jakem radku je aktualni rozdeleni dlazdic
		volatile int zed;  // na jakem sloupci je aktualni rozdeleni dlazdic
	protected:
		virtual void run_thread () override {
			int key, dt_start, s_width, s_height;

			do {
				// ziskavani argumentu prace
				mux.lock();

				key = env->get_key();
				dt_start = rozmer_x * strop + zed;
				s_width  = MIN(normal_tile_size, rozmer_x - zed);
				s_height = MIN(normal_tile_size, rozmer_y - strop);

				// kontrola existence prace
				assert( s_width > 0 );
				if ( s_height <= 0 ) {
					mux.unlock();

					env->open(key); // vycerpa lock na soubor (bez zapisu)
					env->close(key);

					break; // join
				}

				// generovani prace pro ostatni
				zed += normal_tile_size;
				if ( zed >= rozmer_x ) {
					zed = 0;
					strop += normal_tile_size;
				}
				mux.unlock();

				// zde je prace samotna 
				if ( env->smer == SMER::KODER ) { // kodovani
					encode_thread( env, key, pasmo, data, dt_start, 
							stride, s_width, s_height, bpno );

				} else { // dekodovani
					decode_thread( env, key, data, dt_start, 
							stride, s_width, s_height, bpno );
				}

				// prebrani dalsich
			} while (true);
			// or join
		}
	};

} // end namespace


//const static int normal_tile_size = 32;
const static int normal_tile_size = 64;


Ent_ebcot::Ent_ebcot (OZNACENI_PASMA pasmo, KODOVACI_MOZNOST kodovat_jako_pasmo) :
	Entropicky_koder(pasmo),
	kodovat_jako_pasmo(kodovat_jako_pasmo) 
{
	static std::mutex  ebcot_luts_mutex;
	static bool luts_init_complete = false;

	//memset(&mqc, 0, sizeof(MqcState));

	std::lock_guard<std::mutex> guard(ebcot_luts_mutex);

	if ( not luts_init_complete ) {
		luts_init_complete = true;
		init_luts();				   // lookuptables
		ff_jpeg2000_init_tier1_luts(); // 
		ff_mqc_init_context_tables();  // kontextove tab
	}
}

void 
Ent_ebcot::encode (Environment* env, Shared_data data)
{
	Work_manager_ebcot wm (env, pasmo, to_bandpos(kodovat_jako_pasmo), data->data.data(),
			data->rozmer_x, data->rozmer_y);

	wm.generate_threads();
	wm.wait_for_threads();

	/*
	int &rozmer_x = data->rozmer_x, &rozmer_y = data->rozmer_y;

	const int glob_width = MIN(data->rozmer_x, normal_tile_size);
	const int glob_height = MIN(data->rozmer_y, normal_tile_size);
	const int stride = rozmer_x;

	for ( int strop = 0; strop < rozmer_y; strop += glob_height )
	{
		int i_dt_start = strop * rozmer_x;
		for ( int zed = 0; zed < rozmer_x; zed += glob_width ) 
		{
			// skutecne velikosti 
			int s_width  = MIN(glob_width, rozmer_x - zed);
			int s_height = MIN(glob_height, rozmer_y - strop); 
			int dt_start = i_dt_start + zed;

			int key = env->get_key();

			std::thread(encode_thread, env, key, pasmo, data->data.data(), dt_start,
					stride, s_width, s_height, to_bandpos(kodovat_jako_pasmo) ).detach();

			// pro single thread
			//encode_thread(env, key, pasmo, data->data.data(), dt_start,
			//		stride, s_width, s_height, to_bandpos(kodovat_jako_pasmo) );
		}
	}

	env->wait_for_all();
	*/
}

Shared_data
Ent_ebcot::decode ( Environment* env, Shared_data data)
{

	Work_manager_ebcot  wm ( env, to_bandpos(kodovat_jako_pasmo), data->data.data(),
			data->rozmer_x, data->rozmer_y );

	wm.generate_threads();
	wm.wait_for_threads();

	/*
	int &rozmer_x = data->rozmer_x, &rozmer_y = data->rozmer_y;

	const int glob_width = MIN(data->rozmer_x, normal_tile_size);
	const int glob_height = MIN(data->rozmer_y, normal_tile_size);
	const int stride = rozmer_x;

	for ( int strop = 0; strop < rozmer_y; strop += glob_height )
	{
		int i_dt_start = strop * rozmer_x;
		for ( int zed = 0; zed < rozmer_x; zed += glob_width ) 
		{
			// skutecne velikosti 
			int s_width  = MIN(glob_width, rozmer_x - zed);
			int s_height = MIN(glob_height, rozmer_y - strop); 
			int dt_start = i_dt_start + zed;

			int key = env->get_key();

			env->wait_generate_thread();

			std::thread(decode_thread, env, key, data->data.data(), dt_start,
					stride, s_width, s_height, to_bandpos(kodovat_jako_pasmo) ).detach();

			*//* signle thread
			auto cblk = std::make_shared<Jpeg2000Cblk>();
			memset(cblk.get(), 0, sizeof(Jpeg2000Cblk));
			get_set_cblk(env, cblk, key);

			::decode_tile(data->data.data(), cblk,
					dt_start, stride, s_width, s_height, to_bandpos(kodovat_jako_pasmo) );
			*//*
		}
	}

	env->wait_for_all();
	*/
	return data;
}
