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
