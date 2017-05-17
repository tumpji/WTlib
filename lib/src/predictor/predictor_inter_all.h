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

#ifndef PREDICTOR_INTER_HEADER_INCLUDED
#define PREDICTOR_INTER_HEADER_INCLUDED

#include "predictor.h"

namespace Predictor {
    class Nic : public Predictor {
    public:
        Nic () {};
        virtual ~Nic () {};

        virtual void forward_predict (Shared_data data) {(void)data;};
        virtual void reverse_predict (Shared_data data) {(void)data;};
    };



    class Left: public Predictor {
    public:
        Left () {};
        virtual ~Left () {};

        virtual void forward_predict (Shared_data data);
        virtual void reverse_predict (Shared_data data);
    };
    class Up: public Predictor {
    public:
        Up () {};
        virtual ~Up () {};

        virtual void forward_predict (Shared_data data);
        virtual void reverse_predict (Shared_data data);
    };
    class Diag: public Predictor {
    public:
        Diag () {};
        virtual ~Diag () {};

        virtual void forward_predict (Shared_data data);
        virtual void reverse_predict (Shared_data data);
    };



    class Med : public Predictor {
    public:
        Med () {};
        virtual ~Med () {};

        virtual void forward_predict (Shared_data data);
        virtual void reverse_predict (Shared_data data);
    };

    class Paeth: public Predictor {
    public:
        Paeth () {};
        virtual ~Paeth() {};

        virtual void forward_predict (Shared_data data);
        virtual void reverse_predict (Shared_data data);
    };






    class Gradient_adjusted : public Predictor {
    public:
        Gradient_adjusted () {};
        virtual ~Gradient_adjusted () {};

        virtual void forward_predict (Shared_data data);
        virtual void reverse_predict (Shared_data data);
    };

}

#endif // PREDICTOR_INTER_HEADER_INCLUDED
