#ifndef WINDOW_H__
#define WINDOW_H__

#include "plot_util.h"

class Window {

    Point2d origin_;
    double width_, height_;

    ALLEGRO_COLOR bg_col_, border_col_;

    /* Must provide origin */
    Window ();

    const Point2d BottomLeft () const { return origin_; }
    const Point2d TopRight () const;
        
public:

    Window (double x, double y, double w, double h) : 
        origin_(x,y), width_(w), height_(h),
        bg_col_(mkcol (0, 0, 0, 255)), border_col_(mkcol (20, 200, 20, 200)) {}

    Window (const Point2d &p, double w, double h) : 
        origin_(p), width_(w), height_(h),
        bg_col_(mkcol (0, 0, 0, 255)), border_col_(mkcol (20, 200, 20, 200)) {}

    Window (const Point2d &p1, const Point2d &p2) :
        origin_(p1), width_(fabs(p2.X () - p1.X ())), 
        height_(fabs(p2.Y () - p1.Y ())),
        bg_col_(mkcol (0, 0, 0, 255)), border_col_(mkcol (20, 200, 20, 200)) {}

    /* 
     * Take an offset relative to /this/ window origin and 
     * return an absolute offset from the root window origin
     */
    Point2d Translate (const Point2d &p) const;
    Point2d Translate (double x, double y) const;

    double Width () const { return width_; }
    double Height () const { return height_; }
};

#endif /* WINDOW_H__ */
