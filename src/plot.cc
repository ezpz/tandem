
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include "plot.h"

void PlotArea::Setup (const Options& o) { 
    border_.Adjust (o); 
}

Line PlotArea::ClipLine (const Line &line) {
    /* Liang-Barsky Clipping */
    float xmin = opt_.xlim.low,
          xmax = opt_.xlim.high,
          ymin = opt_.ylim.low,
          ymax = opt_.ylim.high;
    float x1 = line.Start ().X (),
          y1 = line.Start ().Y (),
          x2 = line.End ().X (),
          y2 = line.End ().Y ();
    float p1 = -(x2 - x1),
          p2 = -p1,
          p3 = -(y2 - y1),
          p4 = -p3;
    float q1 = x1 - xmin,
          q2 = xmax - x1,
          q3 = y1 - ymin,
          q4 = ymax - y1;

    float posarr[5] = {0.0}, negarr[5] = {0.0};
    int posind = 1, negind = 1;

    posarr[0] = 1.0;
    negarr[0] = 0.0;

    /* Parallel to window boundary */
    if ((p1 == 0 && q1 < 0) || (p3 == 0 && q3 < 0)) { 
        return line;  /* TODO: fix this */
    }

    if (p1 != 0) {
        float r1 = q1 / p1;
        float r2 = q2 / p2;
        if (p1 < 0) {
            negarr[negind++] = r1; 
            posarr[posind++] = r2;
        } else {
            negarr[negind++] = r2;
            posarr[posind++] = r1;
        }
    }

    if (p3 != 0) {
        float r3 = q3 / p3;
        float r4 = q4 / p4;
        if (p3 < 0) {
            negarr[negind++] = r3;
            posarr[posind++] = r4;
        } else {
            negarr[negind++] = r4;
            posarr[posind++] = r3;
        }
    }

    float rn1 = *std::max_element (negarr, negarr + negind);
    float rn2 = *std::min_element (posarr, posarr + posind);

    float xn1 = x1 + p2 * rn1;
    float yn1 = y1 + p4 * rn1; 
    float xn2 = x1 + p2 * rn2;
    float yn2 = y1 + p4 * rn2;

    Point a(xn1, yn1);
    Point b(xn2, yn2);
    return Line(a,b);
}
/*
 * Assumes that p has been validated using PlotArea::Contains
 */
Point PlotArea::Normalize (const Point &p) const {
    float wpx = Width () - (border_.left + border_.right);
    float hpx = Height () - (border_.top + border_.bottom);
    float xrange = opt_.xlim.Delta ();
    float yrange = opt_.ylim.Delta ();
    float xpxpp  = wpx / xrange, ypxpp = hpx / yrange;
    return Point (border_.left + (p.X () - opt_.xlim.low) * xpxpp, 
            border_.bottom + (p.Y () - opt_.ylim.low) * ypxpp);
}

Line PlotArea::Normalize (const Line &l) const {
    Point a = Normalize (l.Start ()), b = Normalize (l.End ());
    return Line (a, b);
}

void PlotArea::SetYlim (const Range& rng) {
    memcpy (&opt_.ylim, &rng, sizeof (Range));
}

void PlotArea::SetXlim (const Range& rng) {
    memcpy (&opt_.xlim, &rng, sizeof (Range));
}

bool PlotArea::Contains (const Point &p) const {
    return (p.X () >= opt_.xlim.low &&
            p.X () <= opt_.xlim.high &&
            p.Y () >= opt_.ylim.low &&
            p.Y () <= opt_.ylim.high);
}

void PlotArea::DrawAllPoints () const {
    std::vector< PointData >::const_iterator PIT = points_.begin (),
        PEND = points_.end ();
    for (; PIT != PEND; ++PIT) {
        al_draw_filled_circle (PIT->np.X (), Height () - PIT->np.Y (),
                1.0 * PIT->opt.cex, PIT->opt.col);
    }
}

void PlotArea::DrawAllLines () const {
    std::vector< LineData >::const_iterator LIT = lines_.begin (),
        LEND = lines_.end ();
    for (; LIT != LEND; ++LIT) {
        al_draw_line (LIT->nl.Start ().X (), Height () - LIT->nl.Start ().Y (),
                LIT->nl.End ().X (), Height () - LIT->nl.End ().Y (),
                LIT->opt.col, LIT->opt.lwd);
    }
}

void PlotArea::DrawAllText () const {
    std::vector< TextData >::const_iterator SIT = strings_.begin (),
        SEND = strings_.end ();
    for (; SIT != SEND; ++SIT) {
        al_draw_text (SIT->opt.font, SIT->opt.col,
                SIT->np.X (), Height () - SIT->np.Y (),
                SIT->opt.align, SIT->txt.c_str ());
    }
}

void PlotArea::DrawAllRectangles () const {
    std::vector< RectData >::const_iterator RIT = rects_.begin (),
        REND = rects_.end ();
    for (; RIT != REND; ++RIT) {
        al_draw_filled_rectangle (RIT->p1.X (), RIT->p1.Y (),
                RIT->p2.X (), RIT->p2.Y (), RIT->opt.sfill);
    }

    if (selection_) { 
        al_draw_filled_rectangle (sp1_.X (), sp1_.Y (), 
                sp2_.X (), sp2_.Y (), opt_.sfill);
        al_draw_rectangle (sp1_.X (), sp1_.Y (),
                sp2_.X (), sp2_.Y (), opt_.sfill, 3);
        DrawPulse ();
    }
}

void PlotArea::CollectPoints (const Point &a, const Point &b,
        std::vector< PointData > &pts) const {

    float minx = std::min (a.X (), b.X ()),
          maxx = std::max (a.X (), b.X ()),
          miny = std::min (Height () - a.Y (), Height () - b.Y ()),
          maxy = std::max (Height () - a.Y (), Height () - b.Y ());

    std::vector< PointData >::const_iterator PIT = points_.begin (),
        PEND = points_.end ();

    for (; PIT != PEND; ++PIT) {
        float x = PIT->np.X (), y = PIT->np.Y ();
        if (x >= minx && x <= maxx && y >= miny && y <= maxy) {
            pts.push_back (*PIT);
        }
    }
}

void
PlotArea::DrawPulse () const {
    std::vector< PointData > pts;
    CollectPoints (sp1_, sp2_, pts);
    std::vector< PointData >::iterator PIT = pts.begin (), PEND = pts.end ();
    for (; PIT != PEND; ++PIT) {
        al_draw_filled_circle (PIT->np.X (), Height () - PIT->np.Y (),
                2.0 * PIT->opt.cex, PIT->opt.col);
    }
}

void PlotArea::DrawPoint (const Point &p) { DrawPoint (p, opt_); }

void PlotArea::DrawPoint (const Point &p, const Options &o) {

    if (! Contains (p)) { return; }

    GrabFocus ();
    Point np = Normalize (p);
    PointData pd(p, np, o);
    points_.push_back (pd);
    al_draw_filled_circle (np.X (), Height () - np.Y (), 
            1.0 * o.cex, o.col);
}

void PlotArea::DrawLine (const Point &a, const Point &b) {
    DrawLine (Line (a, b), opt_);
}

void PlotArea::DrawLine (const Point &a, const Point &b, const Options &o) {
    DrawLine (Line (a, b), o);
}

void PlotArea::DrawLine (const Line &l) { DrawLine (l, opt_); }

void PlotArea::DrawLine (const Line &l, const Options &o) {

    Line clip = ClipLine (l);
    Line nl = Normalize (clip);

    LineData ld(l, nl, o);
    lines_.push_back (ld);

    GrabFocus ();
    al_draw_line (nl.Start ().X (), Height () - nl.Start ().Y (),
            nl.End ().X (), Height () - nl.End ().Y (),
            o.col, o.lwd);
}

void PlotArea::Histogram () { Histogram (opt_); }

void PlotArea::Histogram (const Options &o) {

    Point p1 = sp1_, p2 = sp2_;
    if (! selection_) {
        p1 = Normalize (Point (opt_.xlim.low, opt_.ylim.low));
        p2 = Normalize (Point (opt_.xlim.high, opt_.ylim.high));
    }

    float lowx = std::min (p1.X (), p2.X ()),
          lowy = std::min (Height () - p1.Y (), Height () - p2.Y ()),
          highx = std::max (p1.X (), p2.X ()),
          highy = std::max (Height () - p1.Y (), Height () - p2.Y ());

    std::vector< PointData > pts;
    CollectPoints (p1, p2, pts);

    if (pts.empty ()) { return; }

    /* Calculate X axis distribution */
    float stridex = (highx - lowx) / static_cast< float >(o.nbins),
          stridey = (highy - lowy) / static_cast< float >(o.nbins),
          xpx = (highx - lowx) / o.nbins,
          ypx = (highy - lowy) / o.nbins;

    if (0.0 == stridex || 0.0 == stridey) { 
        fprintf (stderr, "Unable to calculate a valid stride\n");
        return; 
    }

    int hmaxx = 0, hmaxy = 0;
    std::vector< int > xbins(o.nbins + 1), ybins(o.nbins + 1);
    std::vector< PointData >::iterator PIT = pts.begin (), PEND = pts.end ();
    for (; PIT != PEND; ++PIT) {
        int bin = static_cast< int >(floorf ((PIT->np.X () - lowx) / stridex));
        xbins[bin] += 1;
        hmaxx = std::max (hmaxx, xbins[bin]);
        bin = static_cast< int >(floorf ((PIT->np.Y () - lowy) / stridey));
        ybins[bin] += 1;
        hmaxy = std::max (hmaxy, ybins[bin]);
    }

    /* Remove any prior calculation */
    PurgeRects ();

    /*
     * Use bin values to place bar heights at fraction of height of plot area
     */
    float xbar_px = GetWindowPlotWidth () / 4.0, 
          ybar_px = GetWindowPlotHeight () / 4.0;

    /*
     * TODO: Make this factions of the whole set, not counts per bin
     */
    float xbar_pxp = xbar_px / static_cast< float >(hmaxx),
          ybar_pxp = ybar_px / static_cast< float >(hmaxy);

    float xbdr = border_.left, ybdr = border_.top;

    for (int i = 0; i < o.nbins; ++i) {
        /* X-axis */
        float xa = lowx + (i * xpx), xb = xa + xpx - 1;
        float ya = 0.0, yb = xbar_pxp * xbins[i];
        RectData rdx(Point (xa, ybdr + ya), Point (xb, ybdr + yb), o);
        rects_.push_back (rdx);

        /* Y-axis */
        xa = 0.0;
        xb = ybar_pxp * ybins[i];
        ya = lowy + (i * ypx);
        yb = ya + ypx - 1;
        RectData rdy(Point (xbdr + xa, Height () - ya), 
                Point (xbdr + xb, Height () - yb), o);
        rects_.push_back (rdy);
    }
}

void PlotArea::Box () { Box (opt_); }

void PlotArea::Box (const Options &o) {
    GrabFocus ();
    /* TODO:
     * In order to get this to persist without having to call it each refresh
     * (which would require caching the fact that a border is requested)
     * we draw it by individual lines instead of directly calling the 
     * drawing primitives
     */
    Point bl (opt_.xlim.low, opt_.ylim.low), 
          tl (opt_.xlim.low, opt_.ylim.high), 
          tr (opt_.xlim.high, opt_.ylim.high), 
          br (opt_.xlim.high, opt_.ylim.low);
    DrawLine (bl, tl, o);
    DrawLine (tl, tr, o);
    DrawLine (tr, br, o);
    DrawLine (br, bl, o);
}

void PlotArea::Grid () { Grid (opt_); }

void PlotArea::Grid (const Options &o) {
    float xlow = opt_.xlim.low, 
          xhigh = opt_.xlim.high,
          xdelta = opt_.xlim.Delta () / opt_.xticks,
          ylow = opt_.ylim.low,
          yhigh = opt_.ylim.high,
          ydelta = opt_.ylim.Delta () / opt_.yticks;
    for (float x = xlow; x <= xhigh; x += xdelta) {
        Point a(x, ylow), b(x, yhigh);
        DrawLine (a, b, o);
    }
    for (float y = ylow; y <= yhigh; y += ydelta) {
        Point a(xlow, y), b(xhigh, y);
        DrawLine (a, b, o);
    }
}

void PlotArea::DrawText (const Point &p, const std::string &txt) {
    DrawText (p, txt, opt_);
}

void PlotArea::DrawText (const Point &p, const std::string &txt,
        const Options &o) {

    Point np = Normalize (p);
    TextData td (p, np, o, txt);
    strings_.push_back (td);

    al_draw_text (o.font, o.col, np.X (), Height () - np.Y (), 
            o.align, txt.c_str ());
}

void PlotArea::Axis (int which) { Axis (which, opt_); }

void PlotArea::Axis (int which, const Options &o) {
    float x = 0.0, y = 0.0, off = opt_.font_px / 2.0, delta = 0.0;
    float txt_h = opt_.font_px;
    char txt[255] = {0};
    switch (which) {
        case 1: /* bottom */
            delta = opt_.xlim.Delta () / opt_.xticks;
            for (x = opt_.xlim.low + delta; x < opt_.xlim.high; x += delta) {
                DrawLine (Point (x, 0), Point (x, off), o);
                snprintf (txt, sizeof (txt), "%d", 
                        static_cast< int >(roundf (x)));
                DrawText (Point (border_.left + x, border_.bottom - txt_h), 
                        std::string (txt), o);
            }
            break;
        case 2: /* left */
            delta = opt_.ylim.Delta () / opt_.yticks;
            for (y = opt_.ylim.low + delta; y < opt_.ylim.high; y += delta) {
                DrawLine (Point (0, y), Point (off, y), o);
                snprintf (txt, sizeof (txt), "%d", 
                        static_cast< int >(roundf (y)));
                DrawText (
                        Point (border_.left - txt_h, 
                            border_.bottom + y + txt_h), 
                        std::string (txt), o);
            }
            break;
        case 3: /* top */
            delta = opt_.xlim.Delta () / opt_.xticks;
            for (x = opt_.xlim.low + delta; x < opt_.xlim.high; x += delta) {
                DrawLine (Point (x, Height ()), Point (x, Height () - off), o);
            }
            break;
        case 4: /* right */
            delta = opt_.ylim.Delta () / opt_.yticks;
            for (y = opt_.ylim.low + delta; y < opt_.ylim.high; y += delta) {
                DrawLine (Point (Width (), y), Point (Width () - off, y), o);
            }
            break;
        default:
            break;
    }
}

void PlotArea::ClearSelection () { selection_ = false; }

void PlotArea::DrawSelection (const Point &p1, const Point &p2) {
    sp1_ = p1;
    sp2_ = p2;
    selection_ = true;
}

/* 
 * XXX: Move this inside the class?
 */
bool point_in_plot (const PlotArea &plot, const Point &p) {
    Point origin = plot.GetWindowOrigin (),
          corner (origin.X () + plot.GetWindowPlotWidth (), 
                  origin.Y () - plot.GetWindowPlotHeight ());
    return (p.X () >= origin.X () &&
            p.X () <= corner.X () &&
            p.Y () >= corner.Y () && // y-axis in window coords has 0 at
            p.Y () <= origin.Y ());  // top; hence reversal here
}
