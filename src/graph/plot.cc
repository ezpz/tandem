
#include <cstdio>
#include <cmath>

#include <graph/plot.h>
#include <graph/util.h>
#include <graph/exceptions.h>

void BasicPlot::Initialize () {
    FloatType off_left = par_.oma.left * par_.font_px,
              off_right = par_.oma.right * par_.font_px,
              off_top = par_.oma.top * par_.font_px,
              off_bottom = par_.oma.bottom * par_.font_px;

    view_.SetXRange (0.0 + off_left, DisplayWidth () - off_right);
    view_.SetYRange (DisplayHeight () - off_top, 0.0 + off_bottom);
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
    al_draw_rectangle (view_.XRange ().Low (), view_.YRange ().Low (),
            view_.XRange ().High (), view_.YRange ().High (), 
            par_.col, par_.lwd);
}

void ScatterPlot::XTicks () const {
    Parameters par = Par();
    FloatType xstride = xdomain_.Distance () / par.xticks;
    FloatType xmin = xdomain_.Low ();
    FloatType xmax = xdomain_.High ();
    FloatType ymin = ydomain_.Low ();
    FloatType off = par.font_px;
    ALLEGRO_COLOR col = mkcol (255, 255, 255, 255);

    for (FloatType x = xmin + xstride; x < xmax; x += xstride) {
        al_draw_textf (par.font, col, 
                transform (x, XDomain (), XRange ()), 
                transform (ymin, YDomain (), YRange ()) + off, 
                ALIGN_CENTER, 
                "%ld", static_cast< long >(floor (x)));
    }

}

void ScatterPlot::YTicks () const {
    Parameters par = Par();
    FloatType ystride = ydomain_.Distance () / par.yticks;
    FloatType ymax = ydomain_.High ();
    FloatType xmin = xdomain_.Low ();
    FloatType ymin = ydomain_.Low ();
    FloatType xoff = par.font_px;
    FloatType yoff = par.font_px * 0.5;
    ALLEGRO_COLOR col = mkcol (255, 255, 255, 255);

    for (FloatType y = ymin + ystride; y < ymax; y += ystride) {
        al_draw_textf (par.font, col, 
                transform (xmin, XDomain (), XRange ()) - xoff, 
                transform (y, YDomain (), YRange ()) - yoff, 
                ALIGN_RIGHT, 
                "%ld", static_cast< long >(floor (y)));
    }

}

void BasicPlot::Grid () const {
    FloatType xstride = view_.XRange ().Distance () / par_.xticks,
              ystride = view_.YRange ().Distance () / par_.yticks;
    FloatType xmax = view_.XRange ().High (),
              ymax = view_.YRange ().High ();
    FloatType xmin = view_.XRange ().Low (),
              ymin = view_.YRange ().Low ();
    ALLEGRO_COLOR col = mkcol (192, 192, 192, 30);

    for (FloatType x = xmin + xstride; x < xmax; x += xstride) {
        al_draw_line (x, ymin, x, ymax, col, 1);
    }

    for (FloatType y = ymin + ystride; y < ymax; y += ystride) {
        al_draw_line (xmin, y, xmax, y, col, 1);
    }

}

void BasicPlot::XTicks () const {
    throw NotImplemented ("BasicPlot::XTicks");
}

void BasicPlot::YTicks () const {
    throw NotImplemented ("BasicPlot::YTicks");
}

void BasicPlot::Points (const std::vector< Point >&) {
    throw NotImplemented ("BasicPlot::Points");
}

void BasicPlot::Lines (const std::vector< Line >&) {
    throw NotImplemented ("BasicPlot::Lines");
}

void BasicPlot::Text (const Point&, const std::string&) {
    throw NotImplemented ("BasicPlot::Text");
}

void ScatterPlot::Text (const Point& at, const std::string& text) {
    /* TODO: Load font according to Par () cex ? */
    ALLEGRO_COLOR col = mkcol (255, 255, 255, 255);
    al_draw_textf (Par ().font, col, 
            transform (at.X (), XDomain (), XRange ()), 
            transform (at.Y (), YDomain (), YRange ()), 
            ALIGN_LEFT, 
            "%s", text.c_str ());
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
        /* transform from dataset domain to plot range */
        FloatType x = transform (PIT->X (), XDomain (), XRange ());
        FloatType y = transform (PIT->Y (), YDomain (), YRange ());
        al_draw_filled_circle (x, y, 1.0, Par ().col);
    }
}

void ScatterPlot::Lines (const std::vector< Line >& lines) {
    std::vector< Line >::const_iterator LIT = lines.begin (),
        LEND = lines.end ();

    for (; LIT != LEND; ++LIT) {
        /* transform from dataset domain to plot range */
        Line clipped = lineclip (XDomain (), YDomain (), *LIT);
        FloatType x1 = transform (clipped.Start ().X (), 
                XDomain (), XRange ());
        FloatType y1 = transform (clipped.Start ().Y (), 
                YDomain (), YRange ());
        FloatType x2 = transform (clipped.End ().X (), 
                XDomain (), XRange ());
        FloatType y2 = transform (clipped.End ().Y (), 
                YDomain (), YRange ());
        al_draw_line (x1, y1, x2, y2, Par ().col, Par ().lwd);
    }
}

