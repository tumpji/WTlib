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
#ifndef WT_VLNKA_INTERNAL_HEADER_INCLUDED
#define WT_VLNKA_INTERNAL_HEADER_INCLUDED
#include <cassert>

#include "vlnka.h"

////////////////////////////////////////////////////////
////////////// POMOCNE FUNKCE 

// implementuje zrcadlove rozsireni pro libovolne vlnky
// je dulezite aby hranice_min < hranice_max
// vraci odpovidajici index posle predanych hranic
static inline int zrcadlove_rozsireni (
        int hranice_min,
        int hranice_max,
        int pozadovana_hodnota )
{
    assert(hranice_max > hranice_min);

    while ( hranice_min > pozadovana_hodnota ||
            hranice_max < pozadovana_hodnota )
    {
        if ( hranice_min > pozadovana_hodnota ) {
            pozadovana_hodnota = hranice_min + ( hranice_min - pozadovana_hodnota );
        }
        if (hranice_max  < pozadovana_hodnota ) {
            pozadovana_hodnota = hranice_max - ( pozadovana_hodnota - hranice_max );
        }
    }
    return pozadovana_hodnota;
}

#define Di(x) (ROZSIRENI(i+step*((x)*2+1)))
#define Ui(x) (ROZSIRENI(i+step*((x)*2-1)))


// stejne jako poskytuji
#define ROZHRANI_VLNKOVE_FUNKCE_PLNE \
            std::vector<int>& data, \
            const int index_step, \
            const bool reversed, \
            const bool divide_and_round, \
            const int start, \
                  int act,   \
            const int stop,  \
            const int step  

// zjednodusene o index_step, reversed (lze mit ve static funkcich)
#define ROZHRANI_VLNKOVE_FUNKCE_OMEZENE \
    std::vector<int>& data,     \
    const bool        divide_and_round, \
    const int         start,    \
          int         act,      \
    const int         stop,     \
    const int         step

#define ROZHRANI_VLNKOVE_FUNKCE_OMEZENE_PLUS_INDEX \
    std::vector<int>& data,     \
    const bool        divide_and_round, \
    const int         index, \
    const int         start,    \
          int         act,      \
    const int         stop,     \
    const int         step

#define ROZSIRENI(I) (data[(zrcadlove_rozsireni(start, stop, I))])


namespace LINEAR { 
    class VlnkaLinearni : public ::Vlnka {
        virtual int get_number_lifting_steps () {return 1;}

        
        virtual void lifting_predict ( ROZHRANI_VLNKOVE_FUNKCE_PLNE ) = 0;
        virtual void lifting_update  ( ROZHRANI_VLNKOVE_FUNKCE_PLNE ) {};

        virtual int predict_left_shift (int i) = 0;
        virtual int update_left_shift (int i) {(void)i;assert(i<1); return 0;}
    };

    namespace v3 {
        struct Vlnka : public VlnkaLinearni {
            virtual void lifting_predict ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );
            virtual int predict_left_shift (int i) {(void)i; assert(i<1); return 1;}
        };
    }
    namespace v3p {
        struct Vlnka : public VlnkaLinearni {
            virtual void lifting_predict ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );
            virtual int predict_left_shift (int i) {(void)i; assert(i<1); return 1;}
        };
    }
    namespace v7 {
        struct Vlnka : public VlnkaLinearni {
            virtual void lifting_predict ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );
            virtual int predict_left_shift (int i) {(void)i; assert(i<1); return 4;}
        };
    }
    namespace v7m {
        struct Vlnka : public VlnkaLinearni {
            virtual void lifting_predict ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );
            virtual int predict_left_shift (int i) {(void)i; assert(i<1); return 4;}
        };
    }
}




// klasicka CDF 5/3 (2,2)
namespace CDF_5_3 {
    class Vlnka : public ::Vlnka {
    public:
        virtual int get_number_lifting_steps () {return 1;}

        virtual void lifting_predict ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );
        virtual void lifting_update  ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );

        virtual int predict_left_shift (int i) {(void)i;assert(i<1); return 1;}
        virtual int update_left_shift (int i) {(void)i;assert(i<1); return 2;}
    };
}
namespace CDF_5_3p {
    class Vlnka : public ::Vlnka {
    public:
        virtual int get_number_lifting_steps () {return 1;}

        virtual void lifting_predict ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );
        virtual void lifting_update  ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );

        virtual int predict_left_shift (int i) {(void)i;assert(i<1); return 1;}
        virtual int update_left_shift (int i) {(void)i;assert(i<1); return 2;}
    };
}
namespace CDF_5_3pm {
    class Vlnka : public ::Vlnka {
    public:
        virtual int get_number_lifting_steps () {return 1;}

        virtual void lifting_predict ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );
        virtual void lifting_update  ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );

        virtual int predict_left_shift (int i) {(void)i;assert(i<1); return 1;}
        virtual int update_left_shift (int i) {(void)i;assert(i<1); return 2;}
    };
}

// CDF 9/7-F
namespace CDF_9_7_F {
    class Vlnka : public ::Vlnka {
        public:
        virtual int get_number_lifting_steps () {return 2;}

        virtual void lifting_predict ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );
        virtual void lifting_update  ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );

        virtual int predict_left_shift (int i) {(void)i; assert(i<2); return 7;}
        virtual int update_left_shift (int i) {(void)i; assert(i<2); return 12;}
    private:
    };
}

namespace HAAR { 
    class Vlnka : public ::Vlnka {
        public:
        virtual int get_number_lifting_steps () {return 1;}

        virtual void lifting_predict ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );
        virtual void lifting_update  ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );

        virtual int predict_left_shift (int i) {(void)i; assert(i<1); return 0;}
        virtual int update_left_shift (int i) {(void)i; assert(i<1); return 1;}
    private:
    };
}

namespace CDF_9_7_M {
    class Vlnka : public ::Vlnka {
        public:
        virtual int get_number_lifting_steps () {return 1;}

        virtual void lifting_predict ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );
        virtual void lifting_update  ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );

        virtual int predict_left_shift (int i) {(void)i; assert(i<1); return 4;}
        virtual int update_left_shift (int i) {(void)i; assert(i<1); return 2;}
    private:
    };
}
namespace CDF_9_7_Mm {
    class Vlnka : public ::Vlnka {
        public:
        virtual int get_number_lifting_steps () {return 1;}

        virtual void lifting_predict ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );
        virtual void lifting_update  ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );

        virtual int predict_left_shift (int i) {(void)i; assert(i<1); return 4;}
        virtual int update_left_shift (int i) {(void)i; assert(i<1); return 2;}
    private:
    };
}

namespace V_2_6 {
    class Vlnka : public ::Vlnka {
        public:
        virtual int get_number_lifting_steps () {return 2;}

        virtual void lifting_predict ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );
        virtual void lifting_update  ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );

        virtual int predict_left_shift (int i) {if (i) return 2; else return 0;}
        virtual int update_left_shift (int i) {(void)i; return 1;}
    private:
    };
}

namespace V_2_10 {
    class Vlnka : public ::Vlnka {
        public:
        virtual int get_number_lifting_steps () {return 2;}

        virtual void lifting_predict ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );
        virtual void lifting_update  ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );

        virtual int predict_left_shift (int i) {if (i) return 6; else return 0;}
        virtual int update_left_shift (int i) {(void)i; return 1;}
    private:
    };
}

namespace V_5_11_C {
    class Vlnka : public ::Vlnka {
        public:
        virtual int get_number_lifting_steps () {return 2;}

        virtual void lifting_predict ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );
        virtual void lifting_update  ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );

        virtual int predict_left_shift (int i) {if (i) return 4; else return 1;}
        virtual int update_left_shift (int i) {(void)i; return 2;}
    private:
    };
}

namespace V_5_11_A {
    class Vlnka : public ::Vlnka {
        public:
        virtual int get_number_lifting_steps () {return 2;}

        virtual void lifting_predict ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );
        virtual void lifting_update  ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );

        virtual int predict_left_shift (int i) {if (i) return 5; else return 1;}
        virtual int update_left_shift (int i) {(void)i; return 2;}
    private:
    };
}

namespace V_6_14 {
    class Vlnka : public ::Vlnka {
        public:
        virtual int get_number_lifting_steps () {return 2;}

        virtual void lifting_predict ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );
        virtual void lifting_update  ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );

        virtual int predict_left_shift (int i) {if (i) return 4; else return 0;}
        virtual int update_left_shift (int i) {(void)i; return 4;}
    private:
    };
}

namespace V_13_7_T {
    class Vlnka : public ::Vlnka {
        public:
        virtual int get_number_lifting_steps () {return 1;}

        virtual void lifting_predict ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );
        virtual void lifting_update  ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );

        virtual int predict_left_shift (int i) {(void)i; assert(i<1); return 4;}
        virtual int update_left_shift (int i) {(void)i; assert(i<1); return 5;}
    private:
    };
}

namespace V_13_7_C {
    class Vlnka : public ::Vlnka {
        public:
        virtual int get_number_lifting_steps () {return 1;}

        virtual void lifting_predict ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );
        virtual void lifting_update  ( ROZHRANI_VLNKOVE_FUNKCE_PLNE );

        virtual int predict_left_shift (int i) {(void)i; assert(i<1); return 4;}
        virtual int update_left_shift (int i) {(void)i; assert(i<1); return 4;}
    private:
    };
}

#endif // WT_VLNKA_INTERNAL_HEADER_INCLUDED
