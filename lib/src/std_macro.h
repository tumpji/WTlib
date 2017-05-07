#ifndef MY_STD_MACRO_HEADER_INCLUDED
#define MY_STD_MACRO_HEADER_INCLUDED

#include <cassert>

#define MIN(X,Y) ( ((X)<(Y))?(X):(Y) )
#define MAX(X,Y) ( ((X)<(Y))?(Y):(X) )
#define ABS(X)   ( ((X)<0)?(-X):(X)  )


#endif // MY_STD_MACRO_HEADER_INCLUDED
