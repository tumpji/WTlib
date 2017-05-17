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
#include "io.h"

namespace WTio {

void IO_thread::wait_for_all() {
	std::unique_lock<std::mutex> lck (mux);
	while ( cur_thread != n_threads )
		io_lock.wait(lck);
	n_threads = 0;
	cur_thread = 0;
}

int IO_thread::get_key () {
	std::unique_lock<std::mutex> lck (mux);
	return n_threads++;
}

void IO_thread::open (const int key) {
	mux.lock();
	while ( key != cur_thread )
		io_lock.wait(mux);
	mux.unlock();
}

void IO_thread::close(const int key) {
	(void)key; assert( key == cur_thread );
	mux.lock();

	cur_thread += 1;

	mux.unlock();
	io_lock.notify_all();
}


} // WTio

/*
void IO_thread::wait_generate_thread () {
	mux.lock();
	while ( not free_threads )
		thread_lock.wait(mux);

	free_threads -= 1;
	assert(free_threads >= 0);

	//std::cout << free_threads << std::endl;

	mux.unlock();
}

void IO_thread::close_thread () {
	mux.lock();
	free_threads += 1;
	mux.unlock();

	thread_lock.notify_one();
}
*/
