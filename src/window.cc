
#include "window.h"

Point2d Window::Translate (const Point2d &p) const {
    double x = origin_.X () + p.X ();
    double y = origin.Y () + (p.Y () - Height ());
    return Point2d (x, y);
}

Point2d Window::Translate (double x, double y) const {
    return Translate (Point2d (x, y));
}

