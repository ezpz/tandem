#include <graph/util.h>

ColorType mkcol (int r, int g, int b, int alpha) {
    FloatType fr = static_cast< FloatType >(r) / 255.0;
    FloatType fg = static_cast< FloatType >(g) / 255.0;
    FloatType fb = static_cast< FloatType >(b) / 255.0;
    FloatType fa = static_cast< FloatType >(alpha) / 255.0;
    return al_map_rgba_f (fr * fa, fg * fa, fb * fa, fa);
}
