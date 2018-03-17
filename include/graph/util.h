#ifndef UTIL_H__
#define UTIL_H__

#include <graph/types.h>
#include <graph/range.h>
#include <graph/primitives.h>

ColorType mkcol (int r, int g, int b, int alpha);
Line lineclip (const Range& xlim, const Range& ylim, const Line& line);


#endif /* UTIL_H__ */