
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
   return Point (p.X () * xpxpp, p.Y () * ypxpp);
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
        al_draw_filled_circle (PIT->p.X (), Height () - PIT->p.Y (),
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

void PlotArea::DrawPoint (const Point &p) { DrawPoint (p, opt_); }

void PlotArea::DrawPoint (const Point &p, const Options &o) {

    if (! Contains (p)) { return; }

    GrabFocus ();
    Point np = Normalize (p);
    /* Adjust normalized point for margins */
    np.X (np.X () + border_.left);
    np.Y (np.Y () + border_.bottom);
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

    Point a = nl.Start (), b = nl.End ();
    a.X (a.X () + border_.left);
    a.Y (a.Y () + border_.bottom);
    b.X (b.X () + border_.left);
    b.Y (b.Y () + border_.bottom);

    nl = Line(a, b);

    LineData ld(l, nl, o);
    lines_.push_back (ld);

    GrabFocus ();
    al_draw_line (nl.Start ().X (), Height () - nl.Start ().Y (),
            nl.End ().X (), Height () - nl.End ().Y (),
            o.col, o.lwd);
}

void PlotArea::Box () { Box (opt_); }

void PlotArea::Box (const Options &o) {
    GrabFocus ();
    al_draw_rectangle (border_.left, Height () -  border_.bottom,
            Width () - border_.right, border_.top, 
            o.col, o.lwd);
}

void PlotArea::Grid () { Grid (opt_); }

void PlotArea::Grid (const Options &o) {
    float xlow = opt_.xlim.low, 
          xhigh = opt_.xlim.high,
          xdelta = opt_.xlim.Delta () / 7.0,
          ylow = opt_.ylim.low,
          yhigh = opt_.ylim.high,
          ydelta = opt_.ylim.Delta () / 7.0;
    for (float x = xlow; x <= xhigh; x += xdelta) {
        Point a(x, ylow), b(x, yhigh);
        DrawLine (a, b, o);
    }
    for (float y = ylow; y <= yhigh; y += ydelta) {
        Point a(xlow, y), b(xhigh, y);
        DrawLine (a, b, o);
    }
}


//void PlotArea::DrawRectangle (const Point &ll, const Point &ur);
//void PlotArea::DrawRectangle (const Point &ll, const Point &ur, const Options &o);

