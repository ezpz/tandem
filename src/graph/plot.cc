
#include <cstdio>

#include <graph/plot.h>
#include <graph/util.h>

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

}

void ScatterPlot::Xlim (FloatType xmin, FloatType xmax) {
    xdomain_.Reset (xmin, xmax);
}

void ScatterPlot::Ylim (FloatType ymin, FloatType ymax) {
    ydomain_.Reset (ymin, ymax);
}

void ScatterPlot::Plot (const std::vector< Point >& points) {
    std::vector< Point >::const_iterator PIT = points.begin (),
        PEND = points.end ();

    for (; PIT != PEND; ++PIT) {
        /* transform from dataset range to plot domain */
        FloatType x = transform (PIT->X (), XRange (), PlotArea ().XRange ());
        FloatType y = transform (PIT->Y (), YRange (), PlotArea ().YRange ());
        al_draw_filled_circle (x, y, 3.0, mkcol (50, 50, 255, 255));
    }
}

