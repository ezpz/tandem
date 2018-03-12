#include <algorithm>
#include <graph/util.h>

ColorType mkcol (int r, int g, int b, int alpha) {
    FloatType fr = static_cast< FloatType >(r) / 255.0;
    FloatType fg = static_cast< FloatType >(g) / 255.0;
    FloatType fb = static_cast< FloatType >(b) / 255.0;
    FloatType fa = static_cast< FloatType >(alpha) / 255.0;
    return al_map_rgba_f (fr * fa, fg * fa, fb * fa, fa);
}

Line lineclip (const Range& xlim, const Range& ylim, const Line& line) {

    /* Liang-Barsky Clipping */
    FloatType xmin = xlim.X (),
              xmax = xlim.Y (),
              ymin = ylim.X (),
              ymax = ylim.Y ();
    FloatType x1 = line.Start ().X (),
              y1 = line.Start ().Y (),
              x2 = line.End ().X (),
              y2 = line.End ().Y ();
    FloatType p1 = -(x2 - x1),
              p2 = -p1,
              p3 = -(y2 - y1),
              p4 = -p3;
    FloatType q1 = x1 - xmin,
              q2 = xmax - x1,
              q3 = y1 - ymin,
              q4 = ymax - y1;

    FloatType posarr[5] = {0.0}, negarr[5] = {0.0};
    int posind = 1, negind = 1;

    posarr[0] = 1.0;
    negarr[0] = 0.0;

    /* Parallel to window boundary */
    if ((p1 == 0 && q1 < 0) || (p3 == 0 && q3 < 0)) { 
        return line;  /* TODO: fix this */
    }

    if (p1 != 0) {
        FloatType r1 = q1 / p1;
        FloatType r2 = q2 / p2;
        if (p1 < 0) {
            negarr[negind++] = r1; 
            posarr[posind++] = r2;
        } else {
            negarr[negind++] = r2;
            posarr[posind++] = r1;
        }
    }

    if (p3 != 0) {
        FloatType r3 = q3 / p3;
        FloatType r4 = q4 / p4;
        if (p3 < 0) {
            negarr[negind++] = r3;
            posarr[posind++] = r4;
        } else {
            negarr[negind++] = r4;
            posarr[posind++] = r3;
        }
    }

    FloatType rn1 = *std::max_element (negarr, negarr + negind);
    FloatType rn2 = *std::min_element (posarr, posarr + posind);

    FloatType xn1 = x1 + p2 * rn1;
    FloatType yn1 = y1 + p4 * rn1; 
    FloatType xn2 = x1 + p2 * rn2;
    FloatType yn2 = y1 + p4 * rn2;

    Point a(xn1, yn1);
    Point b(xn2, yn2);
    return Line(a,b);
}
