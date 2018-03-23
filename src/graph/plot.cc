
#include <algorithm>
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
    if (Display () != al_get_current_display ()) {
        al_set_target_bitmap (al_get_backbuffer (Display ()));
    }
}

bool BasicPlot::Selected () const {
    return Display () == al_get_current_display ();
}

void BasicPlot::Clear () const {
    /* TODO: make these colors configrable */
    if (Selected ()) {
        al_clear_to_color (al_map_rgb (20, 20, 20));
    } else {
        GrabFocus ();
        al_clear_to_color (al_map_rgb (0, 0, 0));
    }
}

void BasicPlot::Update () const {
    GrabFocus ();
    al_flip_display ();
}

void BasicPlot::Par (const Parameters& par) {
    /*
     * Changing the parameters may mean that the range/domain could have
     * changed for the plot. 
     * TODO: handle the resetting of parameters according to those changes
     */
    par_ = par;
}

void BasicPlot::Box () const { Box (Par ()); }
void BasicPlot::Box (const Parameters& par) const {

    GrabFocus ();

    al_draw_rectangle (view_.XRange ().Low (), view_.YRange ().Low (),
            view_.XRange ().High (), view_.YRange ().High (), 
            par.col, par.lwd);
}

void BasicPlot::Grid () const { Grid (Par ()); }
void BasicPlot::Grid (const Parameters& par) const {
    FloatType xstride = view_.XRange ().Distance () / par.xticks,
              ystride = view_.YRange ().Distance () / par.yticks;
    FloatType xmax = view_.XRange ().High (),
              ymax = view_.YRange ().High ();
    FloatType xmin = view_.XRange ().Low (),
              ymin = view_.YRange ().Low ();
    ColorType col = mkcol (192, 192, 192, 30);

    GrabFocus ();

    for (FloatType x = xmin + xstride; x < xmax; x += xstride) {
        al_draw_line (x, ymin, x, ymax, col, 1);
    }

    for (FloatType y = ymin + ystride; y < ymax; y += ystride) {
        al_draw_line (xmin, y, xmax, y, col, 1);
    }
}

void BasicPlot::Lines (const std::vector< Line >& lines) const { 
    Lines (lines, Par ()); 
}
void BasicPlot::Lines (const std::vector< Line >& lines, 
        const Parameters& par) const {

    std::vector< Line >::const_iterator LIT = lines.begin (),
        LEND = lines.end ();

    GrabFocus ();

    for (; LIT != LEND; ++LIT) {
        /* transform from dataset domain to plot range */
        Line clipped = lineclip (par.xdomain, par.ydomain, *LIT);
        FloatType x1 = transform (clipped.Start ().X (), 
                par.xdomain, XRange ());
        FloatType y1 = transform (clipped.Start ().Y (), 
                par.ydomain, YRange ());
        FloatType x2 = transform (clipped.End ().X (), 
                par.xdomain, XRange ());
        FloatType y2 = transform (clipped.End ().Y (), 
                par.ydomain, YRange ());
        al_draw_line (x1, y1, x2, y2, Par ().col, Par ().lwd);
    }
}

void BasicPlot::Xlim (FloatType xmin, FloatType xmax) {
    par_.SetXDomain (xmin, xmax);
}

void BasicPlot::Ylim (FloatType ymin, FloatType ymax) {
    par_.SetYDomain (ymin, ymax);
}

void BasicPlot::XTicks () const { XTicks (Par ()); }
void BasicPlot::XTicks (const Parameters& par) const {

    const Range& xdomain = par.xdomain;
    const Range& ydomain = par.ydomain;

    FloatType xstride = xdomain.Distance () / par.xticks;
    FloatType xmin = xdomain.Low ();
    FloatType xmax = xdomain.High ();
    FloatType ymin = ydomain.Low ();
    FloatType off = par.font_px;
    ColorType col = mkcol (255, 255, 255, 255);

    GrabFocus ();

    for (FloatType x = xmin + xstride; x < xmax; x += xstride) {
        char txt[16] = {0};
        if (xstride < 1.0) {
            snprintf (txt, 16, "%0.2f", x);
        } else {
            snprintf (txt, 16, "%ld", static_cast< long >(floor (x)));
        }

        al_draw_textf (par.font, col, 
                transform (x, xdomain, XRange ()), 
                transform (ymin, ydomain, YRange ()) + off, 
                ALIGN_CENTER, 
                "%s", txt);
    }
}

void BasicPlot::YTicks () const { YTicks (Par ()); }
void BasicPlot::YTicks (const Parameters& par) const {

    const Range &ydomain = par.ydomain;
    const Range &xdomain = par.xdomain;

    FloatType ystride = ydomain.Distance () / par.yticks;
    FloatType ymax = ydomain.High ();
    FloatType xmin = xdomain.Low ();
    FloatType ymin = ydomain.Low ();
    FloatType xoff = par.font_px;
    FloatType yoff = par.font_px * 0.5;
    ColorType col = mkcol (255, 255, 255, 255);

    GrabFocus ();

    for (FloatType y = ymin + ystride; y < ymax; y += ystride) {
        char txt[16] = {0};
        if (ystride < 1.0) {
            snprintf (txt, 16, "%0.2f", y);
        } else {
            snprintf (txt, 16, "%ld", static_cast< long >(floor (y)));
        }

        al_draw_textf (par.font, col, 
                transform (xmin, xdomain, XRange ()) - xoff, 
                transform (y, ydomain, YRange ()) - yoff, 
                ALIGN_RIGHT, 
                "%s", txt);
    }
}

void BasicPlot::Text (const Point& at, const std::string& text) const { 
    Text (at, text, Par ()); 
}
void BasicPlot::Text (const Point& at, const std::string& text, 
        const Parameters& par) const { 

    ColorType col = mkcol (255, 255, 255, 255);

    GrabFocus ();

    al_draw_textf (par.font, col, 
            transform (at.X (), par.xdomain, XRange ()), 
            transform (at.Y (), par.ydomain, YRange ()), 
            ALIGN_LEFT, 
            "%s", text.c_str ());
}

void ScatterPlot::Plot (const Dataset& data) { Plot (data, Par ()); }
void ScatterPlot::Plot (const Dataset& data, const Parameters& par) {
    Dataset::const_iterator DIT = data.Begin (),
        DEND = data.End ();

    GrabFocus ();

    for (; DIT != DEND; ++DIT) {
        /* transform from dataset domain to plot range */
        FloatType x = transform (DIT->X (), par.xdomain, XRange ());
        FloatType y = transform (DIT->Y (), par.ydomain, YRange ());
        if (XRange ().Contains (x) && YRange ().Contains (y)) {
            if (par.cex < 1.0) {
                al_draw_pixel (x, y, par.col);
            } else {
                al_draw_circle (x, y, par.cex * par.rad, par.col, par.lwd);
            }
        }
    }
}

void HistogramPlot::Plot (const Dataset& data) { Plot (data, Par ()); }
void HistogramPlot::Plot (const Dataset& data, const Parameters& par) {
    switch (par.side) {
        case SIDE_BOTTOM:
            HistBottom (data, par);
            break;
        case SIDE_TOP:
            HistTop (data, par);
            break;
        case SIDE_RIGHT:
            HistRight (data, par);
            break;
        case SIDE_LEFT:
            HistLeft (data, par);
            break;
    }
}

#include <limits>
void HistogramPlot::HistBottom (const Dataset& data, const Parameters& par) {

    Dataset::const_iterator DIT = data.Begin (),
        DEND = data.End ();

    int nbins = par.nbins;
    const Range& xdomain = par.xdomain;
    const Range& ydomain = par.ydomain;
    FloatType lowx = data.XDomain ().Low ();
    FloatType bin_width = data.XDomain ().Distance () / nbins;
    std::vector< long > bins(nbins);
    long bin_max = 0;
    ColorType col = mkcol (0, 0, 0, 255);

    GrabFocus ();

    for (; DIT != DEND; ++DIT) {
        int bin = static_cast< int >(floor ((DIT->X () - lowx) / bin_width));
        /* anything on the border gets placed in the final bin */
        if (bin == nbins) { --bin; }
        bins[bin]++;
        bin_max = std::max (bin_max, bins[bin]);
    }

    for (int n = 0; n < nbins; ++n) {
        FloatType a = lowx + n * bin_width, b = lowx + (n + 1) * bin_width;
        /* Only if the results fit in the selected xlimits */
        if (xdomain.Contains (a) && xdomain.Contains (b)) {
            FloatType x1 = transform (a, xdomain, XRange ());
            FloatType x2 = transform (b, xdomain, XRange ());
            FloatType ratio = static_cast< FloatType >(bins[n]) / 
                static_cast< FloatType >(data.Size ());
            FloatType y1 = transform (0.0, ydomain, YRange ());
            FloatType y2 = transform (ratio, ydomain, YRange ());
            al_draw_filled_rectangle (x1, y1, x2, y2, Par ().sfill);
            /* TODO: only draw border if option is enabled */
            al_draw_rectangle (x1, y1, x2, y2, col, 1.0);
        }
    }
}

void HistogramPlot::HistRight (const Dataset& data, const Parameters& par) {

    Dataset::const_iterator DIT = data.Begin (),
        DEND = data.End ();

    int nbins = par.nbins;
    const Range& xdomain = par.xdomain;
    const Range& ydomain = par.ydomain;
    FloatType lowy = data.YDomain ().Low ();
    FloatType bin_width = data.YDomain ().Distance () / nbins;
    std::vector< long > bins(nbins);
    long bin_max = 0;
    ColorType col = mkcol (0, 0, 0, 255);

    GrabFocus ();

    for (; DIT != DEND; ++DIT) {
        int bin = static_cast< int >(floor ((DIT->Y () - lowy) / bin_width));
        /* anything on the border gets placed in the final bin */
        if (bin == nbins) { --bin; }
        bins[bin]++;
        bin_max = std::max (bin_max, bins[bin]);
    }

    for (int n = 0; n < nbins; ++n) {
        FloatType a = lowy + n * bin_width, b = lowy + (n + 1) * bin_width;
        /* Only if the results fit in the selected ylimits */
        if (ydomain.Contains (a) && ydomain.Contains (b)) {
            FloatType ratio = static_cast< FloatType >(bins[n]) / 
                static_cast< FloatType >(data.Size ());
            FloatType x1 = transform (xdomain.High () - ratio, 
                    xdomain, XRange ());
            FloatType x2 = transform (xdomain.High (), xdomain, XRange ());
            FloatType y1 = transform (a, ydomain, YRange ());
            FloatType y2 = transform (b, ydomain, YRange ());
            al_draw_filled_rectangle (x1, y1, x2, y2, Par ().sfill);
            /* TODO: only draw border if option is enabled */
            al_draw_rectangle (x1, y1, x2, y2, col, 1.0);
        }
    }
}

void HistogramPlot::HistTop (const Dataset&, const Parameters&) {
}

void HistogramPlot::HistLeft (const Dataset&, const Parameters&) {
}

