#ifndef TYPES_H__
#define TYPES_H__

#include <allegro5/allegro.h>

#define DEFAULT_FONT_SIZE 8.0

typedef ALLEGRO_COLOR   ColorType;

#ifdef USE_FLOAT
    typedef float           FloatType;
    #define FT_EPSILON      FLT_EPSILON
    #define FT_MAX          FLT_MAX
    #define FT_MIN          FLT_MIN
#else
    typedef double          FloatType;
    #define FT_EPSILON      DBL_EPSILON
    #define FT_MAX          DBL_MAX
    #define FT_MIN          DBL_MIN
#endif

#endif /* TYPES_H__ */
