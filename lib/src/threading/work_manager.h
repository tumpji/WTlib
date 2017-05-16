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
#include <mutex>
#include <array>
#include <thread>

#include "wt_inter.h"



class Work_manager {
	public:
		void generate_threads ();

		// ceka na vsechny vlakna
		void wait_for_threads ();

	protected:
		Work_manager() {}
		~Work_manager () { wait_for_threads(); }

		virtual void run_thread () = 0;

		std::mutex   mux;
	private:
		static void init_thread (Work_manager*);
		std::array<std::thread,N_THREADS_AVAIABLE>  vlakna;
};




