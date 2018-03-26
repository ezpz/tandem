#ifndef TYPES_H__
#define TYPES_H__

#include <cfloat>
#include <allegro5/allegro.h>

#define DEFAULT_FONT_SIZE 8.0

typedef ALLEGRO_COLOR   ColorType;

enum Orientation {
    SIDE_BOTTOM = 0,
    SIDE_LEFT,
    SIDE_TOP,
    SIDE_RIGHT,
    VERTICAL,
    HORIZONTAL
};

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
