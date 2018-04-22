#include <algorithm>
#include <cmath>
#include <cfloat>
#include <graph/util.h>

ColorType mkcol (int r, int g, int b, int alpha) {
    FloatType fr = static_cast< FloatType >(r) / 255.0;
    FloatType fg = static_cast< FloatType >(g) / 255.0;
    FloatType fb = static_cast< FloatType >(b) / 255.0;
    FloatType fa = static_cast< FloatType >(alpha) / 255.0;
    return al_map_rgba_f (fr * fa, fg * fa, fb * fa, fa);
}

ColorType gradient (ColorType c1, ColorType c2, FloatType amnt) {
    FloatType rstep = (c1.r - c2.r) / 100.0;
    FloatType gstep = (c1.g - c2.g) / 100.0;
    FloatType bstep = (c1.b - c2.b) / 100.0;
    FloatType astep = (c1.a - c2.a) / 100.0;
    ColorType c;

    if (amnt < 0.0) { amnt = 0.0; }
    if (amnt > 1.0) { amnt = 1.0; }

    c.r = c1.r - (amnt * 100.0 * rstep);
    c.g = c1.g - (amnt * 100.0 * gstep);
    c.b = c1.b - (amnt * 100.0 * bstep);
    c.a = c1.a - (amnt * 100.0 * astep);

    return c;
}

std::vector< std::string > breakLines (const std::string& str) {
    std::vector< std::string > lines;
    std::string::size_type pos = str.find ("\n");
    if (std::string::npos == pos) { 
        lines.push_back (str);
        return lines;
    } else {
        lines.push_back (str.substr (0, pos));
    }
    while (std::string::npos != pos) {
        std::string::size_type end = str.find ("\n", pos + 1);
        lines.push_back (str.substr (pos + 1, end));            
        pos = end;
    }
    return lines;
}

const char * orientation2str (Orientation o) {
    switch (o) {
        case SIDE_BOTTOM: return "SIDE_BOTTOM";
        case SIDE_TOP: return "SIDE_TOP";
        case SIDE_LEFT: return "SIDE_LEFT";
        case SIDE_RIGHT: return "SIDE_RIGHT";
        case HORIZONTAL: return "HORIZONTAL";
        case VERTICAL: return "VERTICAL";
        default: return "MISSING ORIENTATION CASE";
    }
    return "ORIENTATION SWITCH FAIL";
}

/*
 * Borrowed most of the below for the R code to do the same
 */
std::vector< FloatType > prettyTicks (const Range& range, int ndiv) {
    FloatType low = range.Low (), high = range.High ();
    FloatType dx = high - low;
    FloatType shrink_sml = 0.25;
    FloatType rounding_eps = 1e-10;
    FloatType h = 0.8, h5 = 1.7;
    FloatType cell = 0.0, u = 0.0, base = 0.0, unit = 0.0, ns = 0.0, nu = 0.0;
    FloatType dist = 0.0, step = 0.0;
    bool i_small = false;
    int min_n = 1;

    if (0 == dx && 0 == high) {
        cell = 1.0;
        i_small = true;
    } else {
        cell = std::max (std::fabs (low), std::fabs (high));
        u = 1 + ((h5 >= 1.5 * h + 0.5) ? 1.0 / (1 + h) : 1.5 / (1 + h5));
        u *= std::max (1.0, static_cast< FloatType >(ndiv)) * FT_EPSILON;
        i_small = dx < cell * u * 3.0;
    }

    if (i_small) {
        if (cell > 10.0) {
            cell = 9.0 + cell / 10.0;
        }
        cell *= shrink_sml;
        if (min_n > 1) {
            cell /= min_n;
        }
    } else {
        cell = dx;
        if (ndiv > 1) {
            cell /= ndiv;
        }
    }

    if (cell < 20 * FT_MIN) {
        cell = 20 * FT_MIN;
    } else if (cell * 10 > FT_MAX) {
        cell = 0.1 * FT_MAX;
    }

    base = std::pow (10.0, floor (log10 (cell)));
    unit = base;

    u = 2 * base;
    if (u - cell < h * (cell - unit)) {
        unit = u;
        u = 5 * base;
        if (u - cell < h5 * (cell - unit)) {
            unit = u;
            u = 10 * base;
            if (u - cell < h * (cell - unit)) {
                unit = u;
            }
        }
    }

    ns = floor (low / unit + rounding_eps);
    nu = ceil (high / unit - rounding_eps);

    while (ns * unit > low + rounding_eps * unit) {
        ns -= 1;
    }

    while (nu * unit < high - rounding_eps * unit) {
        nu += 1;
    }

    std::vector< FloatType > xs;
    dist = nu - ns;
    step = dist / ndiv;
    for (int i = 0; i < ndiv; ++i) {
        xs.push_back (low + step * i);
    }
    return xs;
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
