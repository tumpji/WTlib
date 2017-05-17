
#include "work_manager.h"


void Work_manager::wait_for_threads() {
	for ( std::thread &t : vlakna )
		if ( t.joinable() )
			t.join();
}


void Work_manager::generate_threads() {

	wait_for_threads ();
	// all threads are join

	for ( std::thread &t : vlakna ) {
		// generovani nove prace 
		t = std::thread(&Work_manager::init_thread, this);
	}
}

void Work_manager::init_thread (Work_manager* wm) {
	wm->run_thread();
}
