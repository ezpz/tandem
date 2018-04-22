#ifndef UTIL_H__
#define UTIL_H__

#include <vector>
#include <string>

#include <graph/types.h>
#include <graph/range.h>
#include <graph/primitives.h>

ColorType mkcol (int r, int g, int b, int alpha);

/* 
 * Select a color along the spectrum between two other colors
 * [amnt] is a value between 0.0 (c1) and 1.0 (c2)
 */
ColorType gradient (ColorType c1, ColorType c2, FloatType amnt);

/*
 * Break a string with '\n' characters into multiple strings
 */
std::vector< std::string > breakLines (const std::string& str);

Line lineclip (const Range& xlim, const Range& ylim, const Line& line);
std::vector< FloatType > prettyTicks (const Range& range, int ndiv);
const char * orientation2str (Orientation o);


#endif /* UTIL_H__ */
