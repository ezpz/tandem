#ifndef PLOT_UTIL_H__
#define PLOT_UTIL_H__

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

/*
 * Absolute range of an axis
 */
struct Range {
    float low, high;
    Range () : low(0.0), high(0.0) {}
    Range (float l, float h) : low(l), high(h) {
        assert (low <= high);
    }
    inline float Delta () const { return high - low; }
};

struct Margin {
    float top, bottom, left, right;
};

struct Options {

    ALLEGRO_COLOR       col;        /* color */
    float               lwd;        /* line width */
    float               cex;        /* expansion factor for points/text */
    ALLEGRO_FONT        *font;      /* default font */
    float               font_px;    /* height of font in px */

    Margin              oma;        /* outer margins */
    Range               xlim, ylim; /* plot region bounds */

    /* Reset all default values */
    void Reset ();
    
    Options () { Reset (); }

    static const Options& Defaults () {
        static Options defaults;
        return defaults;
    }

};

/* 
 * Plot border in pixels surrounding the plotting area
 * Used for lables and titles
 */
struct Border {
    float bottom, left, top, right;
    Border () : bottom(0.0), left(0.0), top(0.0), right(0.0) {}
    inline void Adjust (const Options& opt) {
        bottom = opt.oma.bottom * opt.font_px;
        left = opt.oma.left * opt.font_px;
        top = opt.oma.top * opt.font_px;
        right = opt.oma.right * opt.font_px;
    }
};


class Point {

    double x_, y_;

public:

    Point (const Point &other) : x_(other.X ()), y_(other.Y ()) {}
    Point (double x, double y) : x_(x), y_(y) {}
    Point () : x_(0.0), y_(0.0) {}

    double X () const { return x_; }
    double Y () const { return y_; }

    void X (double x) { x_ = x; }
    void Y (double y) { y_ = y; }

};


class Line {

    Point a_, b_;

public:

    Line (const Line &other) : a_(other.a_), b_(other.b_) {}
    Line (const Point &a, const Point &b) : a_(a), b_(b) {}

    const Point& Start () const { return a_; }
    const Point& End () const { return b_; }

};

struct PointData {
    Point p;    /* absolute point */ 
    Point np;   /* normalized point */
    /* TODO: too heavy to store with every single point... */
    Options opt;/* Options used to draw point */

    PointData (const Point& pp, const Point& npp, const Options& o) :
        p(pp), np(npp), opt(o) {}

private:

    PointData ();
};

struct LineData {
    Line l;     /* absolute line */
    Line nl;    /* normalized/clipped line */
    /* TODO: too heavy to store with every single line... */
    Options opt;/* Options used to draw line */

    LineData (const Line& ll, const Line& nll, const Options& o) :
        l(ll), nl(nll), opt(o) {}

private:

    LineData ();
};

ALLEGRO_COLOR mkcol (int r, int g, int b, int a);

#endif /* PLOT_UTIL_H__ */
