
#include <algorithm>
#include <cstdio>
#include <cmath>

#include <graph/plot.h>
#include <graph/util.h>
#include <dataset/summary.h>

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

void BasicPlot::XGrid () const { XGrid (Par ()); }
void BasicPlot::XGrid (const Parameters& par) const { 
    FloatType xstride = view_.XRange ().Distance () / par.xticks;
    FloatType xmax = view_.XRange ().High (),
              ymax = view_.YRange ().High ();
    FloatType xmin = view_.XRange ().Low (),
              ymin = view_.YRange ().Low ();
    /* TODO: make configurable */
    ColorType col = mkcol (192, 192, 192, 30);

    GrabFocus ();

    for (FloatType x = xmin + xstride; x < xmax; x += xstride) {
        al_draw_line (x, ymin, x, ymax, col, 1);
    }

}

void BasicPlot::YGrid () const { YGrid (Par ()); }
void BasicPlot::YGrid (const Parameters& par) const { 
    FloatType ystride = view_.YRange ().Distance () / par.yticks;
    FloatType xmax = view_.XRange ().High (),
              ymax = view_.YRange ().High ();
    FloatType xmin = view_.XRange ().Low (),
              ymin = view_.YRange ().Low ();
    /* TODO: make configurable */
    ColorType col = mkcol (192, 192, 192, 30);

    GrabFocus ();

    for (FloatType y = ymin + ystride; y < ymax; y += ystride) {
        al_draw_line (xmin, y, xmax, y, col, 1);
    }
}

void BasicPlot::Grid () const { Grid (Par ()); }
void BasicPlot::Grid (const Parameters& par) const {
    XGrid (par);
    YGrid (par);
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
    FloatType a = xdomain.X ();
    FloatType b = xdomain.Y ();
    FloatType ymin = ydomain.Low ();
    FloatType off = par.font_px;
    ColorType col = mkcol (255, 255, 255, 255);

    if (a > b) { std::swap (a, b); }

    GrabFocus ();

    for (FloatType x = a + xstride; x < b; x += xstride) {
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
    /*
     * TODO: The only difference between TOP/BOTTOM or LEFT/RIGHT
     * is the Range settings. Roll up the common parts here (i.e.
     * iterating over the dataset and finding the bin values and 
     * ratio) then pass the results onto a common method to handle
     * the plotting
     */
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
        default:
            throw InvalidOrientation ("HistogramPlot::Plot", par.side);
    }
}

void HistogramPlot::HistBottom (const Dataset& data, const Parameters& par) {

    Dataset::const_iterator DIT = data.Begin (),
        DEND = data.End ();

    int nbins = par.nbins;
    const Range& xdomain = par.xdomain;
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

    /* TODO: 
     * FIXME:
     * Ignore any passed in ydomain and fix it to the bounds of
     * calculated values. This should check for 'default' vs. 'user-
     * supplied' parameters
     */
    Parameters p(Par ());
    p.SetYDomain (0.0, 
            1.10 * (static_cast< FloatType >(bin_max) /
                static_cast< FloatType >(data.Size ())));
    Par(p);
    const Range& ydomain = p.ydomain;

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

    /* TODO: 
     * FIXME:
     * Ignore any passed in xdomain and fix it to the bounds of
     * calculated values. This should check for 'default' vs. 'user-
     * supplied' parameters
     */
    Parameters p(Par ());
    p.SetXDomain (1.10 * (static_cast< FloatType >(bin_max) /
                static_cast< FloatType >(data.Size ())), 0.0);
    Par(p);
    const Range& xdomain = p.xdomain;

    for (int n = 0; n < nbins; ++n) {
        FloatType a = lowy + n * bin_width, b = lowy + (n + 1) * bin_width;
        /* Only if the results fit in the selected ylimits */
        if (ydomain.Contains (a) && ydomain.Contains (b)) {
            FloatType ratio = static_cast< FloatType >(bins[n]) / 
                static_cast< FloatType >(data.Size ());
            FloatType x1 = transform (ratio, xdomain, XRange ());
            FloatType x2 = transform (0.0, xdomain, XRange ());
            FloatType y1 = transform (a, ydomain, YRange ());
            FloatType y2 = transform (b, ydomain, YRange ());
            al_draw_filled_rectangle (x1, y1, x2, y2, Par ().sfill);
            /* TODO: only draw border if option is enabled */
            al_draw_rectangle (x1, y1, x2, y2, col, 1.0);
        }
    }
}

void HistogramPlot::HistTop (const Dataset& data, const Parameters& par) {

    Dataset::const_iterator DIT = data.Begin (),
        DEND = data.End ();

    int nbins = par.nbins;
    const Range& xdomain = par.xdomain;
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

    /* TODO: 
     * FIXME:
     * Ignore any passed in ydomain and fix it to the bounds of
     * calculated values. This should check for 'default' vs. 'user-
     * supplied' parameters
     */
    Parameters p(Par ());
    p.SetYDomain (1.10 * (static_cast< FloatType >(bin_max) /
                static_cast< FloatType >(data.Size ())), 0.0);
    Par(p);
    const Range& ydomain = p.ydomain;

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

void HistogramPlot::HistLeft (const Dataset& data, const Parameters& par) {

    Dataset::const_iterator DIT = data.Begin (),
        DEND = data.End ();

    int nbins = par.nbins;
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

    /* TODO: 
     * FIXME:
     * Ignore any passed in xdomain and fix it to the bounds of
     * calculated values. This should check for 'default' vs. 'user-
     * supplied' parameters
     */
    Parameters p(Par ());
    p.SetXDomain (0.0, 
            1.10 * (static_cast< FloatType >(bin_max) /
                static_cast< FloatType >(data.Size ())));
    Par(p);
    const Range& xdomain = p.xdomain;

    for (int n = 0; n < nbins; ++n) {
        FloatType a = lowy + n * bin_width, b = lowy + (n + 1) * bin_width;
        /* Only if the results fit in the selected ylimits */
        if (ydomain.Contains (a) && ydomain.Contains (b)) {
            FloatType ratio = static_cast< FloatType >(bins[n]) / 
                static_cast< FloatType >(data.Size ());
            FloatType x1 = transform (ratio, xdomain, XRange ());
            FloatType x2 = transform (0.0, xdomain, XRange ());
            FloatType y1 = transform (a, ydomain, YRange ());
            FloatType y2 = transform (b, ydomain, YRange ());
            al_draw_filled_rectangle (x1, y1, x2, y2, Par ().sfill);
            /* TODO: only draw border if option is enabled */
            al_draw_rectangle (x1, y1, x2, y2, col, 1.0);
        }
    }
}

void BoxPlot::Plot (const Dataset& data) { Plot (data, Par ()); }
void BoxPlot::Plot (const Dataset& data, const Parameters& par) {

    switch (par.side) {
        case VERTICAL:
            Vertical (data, par);
            break;
        case HORIZONTAL:
            Horizontal (data, par);
            break;
        default:
            throw InvalidOrientation ("HistogramPlot::Plot", par.side);
    }
}

void BoxPlot::Vertical (const Dataset& data, const Parameters& par) {

    std::vector< FloatType > ys;

    data.YData (ys);
    BoxPlotSummary bp(ys);

    /*
     * Center on the viewport x-axis and have data dictate where
     * the boxplot boundary is on the yaxis
     */
    FloatType clx = XRange ().Low () + XRange ().Distance () / 2.0;
    FloatType boxwidth = XRange ().Distance () / 20.0;

    FloatType m = transform (bp.Median (), data.YDomain (), YRange ()),
              uq = transform (bp.UpperQ (), data.YDomain (), YRange ()),
              lq = transform (bp.LowerQ (), data.YDomain (), YRange ());

    FloatType x1 = clx - boxwidth, x2 = clx + boxwidth;

    GrabFocus ();

    al_draw_rectangle (x1, lq, x2, uq, par.col, 2.0);

    /* 
     * cant use BasicPlot::Lines as they are specified in the input domain
     * and our x values here are in viewport coordinates
     */
    al_draw_line (x1, m, x2,  m, par.col, 1.0);

    FloatType y = transform (bp.LowerBound (), par.ydomain, YRange ());
    al_draw_line (clx, y, clx, lq, par.col, 1.0);
    y = transform (bp.UpperBound (), par.ydomain, YRange ());
    al_draw_line (clx, uq, clx, y, par.col, 1.0);

    /* Plot outliers */
    std::vector< FloatType >::const_iterator FIT = ys.begin (), 
        FEND = ys.end ();
    for (; FIT != FEND; ++FIT) {
        if (bp.Outlier (*FIT)) {
            y = transform (*FIT, par.ydomain, YRange ());
            al_draw_circle (clx, y, 1.5 * par.rad, par.col, par.lwd);
        }
    }
}

void BoxPlot::Horizontal (const Dataset& data, const Parameters& par) {

    std::vector< FloatType > xs;

    data.XData (xs);
    BoxPlotSummary bp(xs);

    /*
     * Center on the viewport y-axis and have data dictate where
     * the boxplot boundary is on the xaxis
     */
    FloatType cly = YRange ().Low () + YRange ().Distance () / 2.0;
    FloatType boxheight = YRange ().Distance () / 20.0;

    FloatType m = transform (bp.Median (), par.xdomain, XRange ()),
              uq = transform (bp.UpperQ (), par.xdomain, XRange ()),
              lq = transform (bp.LowerQ (), par.xdomain, XRange ());

    FloatType y1 = cly - boxheight, y2 = cly + boxheight;

    GrabFocus ();

    al_draw_rectangle (lq, y1, uq, y2, par.col, 2.0);

    /* 
     * cant use BasicPlot::Lines as they are specified in the input domain
     * and our y values here are in viewport coordinates
     */
    al_draw_line (m, y1, m, y2, par.col, 1.0);

    FloatType x = transform (bp.LowerBound (), par.xdomain, XRange ());
    al_draw_line (x, cly, lq, cly, par.col, 1.0);
    x = transform (bp.UpperBound (), par.xdomain, XRange ());
    al_draw_line (uq, cly, x, cly, par.col, 1.0);

    /* Plot outliers */
    std::vector< FloatType >::const_iterator FIT = xs.begin (), 
        FEND = xs.end ();
    for (; FIT != FEND; ++FIT) {
        if (bp.Outlier (*FIT)) {
            x = transform (*FIT, par.xdomain, XRange ());
            al_draw_circle (x, cly, 1.5 * par.rad, par.col, par.lwd);
        }
    }
}

bool 
HexBinPlot::AllValid (const std::vector< FloatType > &vs, RangeType which) {
    std::vector< FloatType >::const_iterator VIT = vs.begin (), 
        VEND = vs.end ();
    Range rng = which == RANGE_X ? XRange () : YRange ();
    for (; VIT != VEND; ++VIT) {
        if (! rng.Contains (*VIT)) {
            return false;
        }
    }
    return true;
}

void HexBinPlot::Plot (const Dataset& data) { Plot (data, Par ()); }
void HexBinPlot::Plot (const Dataset& data, const Parameters& par) {

    Range xrng = XRange (), yrng = YRange ();
    FloatType dist = yrng.Distance ();
    FloatType minx = xrng.Low (), maxx = xrng.High ();
    FloatType miny = yrng.Low (), maxy = yrng.High ();
    
    /* TODO: for now, just use fixed number of bins */
    FloatType nbins = 30.0;

    FloatType hex = dist / nbins;
    
    FloatType xstep = 2 * sin (60.0 * M_PI / 180.0) * hex;
    FloatType ystep = 1.5 * hex;
    FloatType a = 0.5 * hex, b = sin (60.0 * M_PI / 180.0) * hex;
    int z = 0;

    FloatType width = 2 * b;
    FloatType height = hex + a;

    int nxbins = static_cast< int >(ceil (xrng.Distance () / width));
    int nybins = static_cast< int >(ceil (yrng.Distance () / height));

    std::vector< std::vector< int > > grid (nybins);
    for (int i = 0; i < nybins; ++i) {
        grid[i].resize (nxbins);
    }

    Dataset::const_iterator DIT = data.Begin (), DEND = data.End ();
    int yidx = 0, xidx = 0, maxbin = 0;
    for (; DIT != DEND; ++DIT) {
        FloatType tx = transform (DIT->X (), par.xdomain, xrng);
        FloatType ty = transform (DIT->Y (), par.ydomain, yrng);

        /* Adjust for border offset surrounding viewport */
        tx -= minx;
        ty -= miny;

        yidx = static_cast< int >(ty / height);
        if (0 == yidx % 2) {
            if (tx > b) {
                xidx = static_cast< int >((tx - b) / width);
            } else {
                xidx = 0;
            }
        } else {
            xidx = static_cast< int >(tx / width);
        }

        grid[yidx][xidx] += 1;
        maxbin = std::max (maxbin, grid[yidx][xidx]);
    }

    ColorType cool = mkcol (0, 0, 255, 8);
    ColorType hot = mkcol (0, 0, 255, 255);

    xidx = yidx = 0;
    for (FloatType y = miny + a; y < maxy; y += ystep, ++z, ++yidx) {

        FloatType xoff = ((1 == z % 2) ? b : 0.0);
        std::vector< FloatType > ys = { y, y + hex, y + hex + a, y - a };

        if (! AllValid (ys, RANGE_Y)) { continue; }

        xidx = 0;
        for (FloatType x = minx + xoff; x < maxx - xstep; x += xstep, ++xidx) {

            std::vector< FloatType > xs = { x, x + b, x + xstep };
            int cnt = grid[yidx][xidx];

            /* Only display bins that had any items */
            if (cnt == 0) { continue; }

            if (AllValid (xs, RANGE_X)) {

                ALLEGRO_VERTEX v[7];
                FloatType alpha = static_cast< FloatType >(cnt) / 
                        static_cast< FloatType >(maxbin);
                ColorType col = gradient (cool, hot, alpha);

                memset (v, 0, sizeof (ALLEGRO_VERTEX) * 7);

                v[0].x = static_cast< float >(x);
                v[0].y = static_cast< float >(y);
                v[0].color = col;
                v[1].x = static_cast< float >(x);
                v[1].y = static_cast< float >(y + hex);
                v[1].color = col;
                v[2].x = static_cast< float >(x + b);
                v[2].y = static_cast< float >(y + hex + a);
                v[2].color = col;
                v[3].x = static_cast< float >(x + xstep);
                v[3].y = static_cast< float >(y + hex);
                v[3].color = col;
                v[4].x = static_cast< float >(x + xstep);
                v[4].y = static_cast< float >(y);
                v[4].color = col;
                v[5].x = static_cast< float >(x + b);
                v[5].y = static_cast< float >(y - a);
                v[5].color = col;
                v[6].x = static_cast< float >(x);
                v[6].y = static_cast< float >(y);
                v[6].color = col;
                al_draw_prim (v, NULL, NULL, 0, 7, ALLEGRO_PRIM_TRIANGLE_FAN);

                for (int i = 0; i < 7; ++i) {
                    v[i].color = par.col;
                }
                al_draw_prim (v, NULL, NULL, 0, 7, ALLEGRO_PRIM_LINE_LOOP);
            }
        }
    }
}
