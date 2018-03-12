
#include <cstdio>

#include <graph/plot.h>
#include <graph/util.h>
#include <graph/exceptions.h>

void BasicPlot::Initialize () {
    view_.SetXRange (0.0, DisplayWidth ());
    view_.SetYRange (DisplayHeight (), 0.0);
}

void BasicPlot::GrabFocus () const {
    if (win_ != al_get_current_display ()) {
        al_set_target_bitmap (al_get_backbuffer (Display ()));
    }
}

void BasicPlot::Clear () const {
    GrabFocus ();
    /* TODO: make this color configrable */
    al_clear_to_color (al_map_rgb (0, 0, 0));
}

void BasicPlot::Update () const {
    GrabFocus ();
    al_flip_display ();
}

void BasicPlot::Box () const {
    /* TODO: implement */
}

void BasicPlot::Points (const std::vector< Point >&) {
    throw NotImplemented ("BasicPlot::Points");
}

void BasicPlot::Lines (const std::vector< Line >&) {
    throw NotImplemented ("BasicPlot::Lines");
}

void ScatterPlot::Xlim (FloatType xmin, FloatType xmax) {
    xdomain_.Reset (xmin, xmax);
}

void ScatterPlot::Ylim (FloatType ymin, FloatType ymax) {
    ydomain_.Reset (ymin, ymax);
}

void ScatterPlot::Points (const std::vector< Point >& points) {
    std::vector< Point >::const_iterator PIT = points.begin (),
        PEND = points.end ();

    for (; PIT != PEND; ++PIT) {
        /* transform from dataset range to plot domain */
        FloatType x = transform (PIT->X (), XRange (), PlotArea ().XRange ());
        FloatType y = transform (PIT->Y (), YRange (), PlotArea ().YRange ());
        al_draw_filled_circle (x, y, 1.5, mkcol (50, 50, 255, 255));
    }
}

void ScatterPlot::Lines (const std::vector< Line >& lines) {
    std::vector< Line >::const_iterator LIT = lines.begin (),
        LEND = lines.end ();

    for (; LIT != LEND; ++LIT) {
        /* transform from dataset range to plot domain */
        Line clipped = lineclip (XRange (), YRange (), *LIT);
        FloatType x1 = transform (clipped.Start ().X (), 
                XRange (), PlotArea ().XRange ());
        FloatType y1 = transform (clipped.Start ().Y (), 
                YRange (), PlotArea ().YRange ());
        FloatType x2 = transform (clipped.End ().X (), 
                XRange (), PlotArea ().XRange ());
        FloatType y2 = transform (clipped.End ().Y (), 
                YRange (), PlotArea ().YRange ());
        al_draw_line (x1, y1, x2, y2, mkcol (50, 50, 255, 255), 1.5);
    }
}

