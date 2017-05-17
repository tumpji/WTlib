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
