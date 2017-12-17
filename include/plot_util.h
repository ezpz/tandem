#ifndef PLOT_UTIL_H__
#define PLOT_UTIL_H__

#include <allegro5/allegro.h>

class Point2d {

    double x_, y_;

public:

    Point2d (const Point2d &other) : x_(other.X ()), y_(other.Y ()) {}
    Point2d (double x, double y) : x_(x), y_(y) {}

    const double X () const { return x_; }
    const double Y () const { return y_; }

    void X (double x) { x_ = x; }
    void Y (double y) { y_ = y; }
};


ALLEGRO_COLOR mkcol (int r, int g, int b, int a);

#endif /* PLOT_UTIL_H__ */
