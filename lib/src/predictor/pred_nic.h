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

#ifndef NIC_PREDICTOR_HEADER_INCLUDED
#define NIC_PREDICTOR_HEADER_INCLUDED

#include "predictor.h"

namespace Predictor {
	class Nic : public Predictor {
	public:
		Nic () {};
		virtual ~Nic () {};

		virtual void forward_predict (Shared_data data) {(void)data;};
		virtual void reverse_predict (Shared_data data) {(void)data;};
	};
}

#endif // NIC_PREDICTOR_HEADER_INCLUDED
