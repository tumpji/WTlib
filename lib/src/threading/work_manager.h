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




