#ifndef DATASET_H__
#define DATASET_H__

#include <vector>
#include <algorithm>

#include <graph/primitives.h>
#include <graph/range.h>

/*
 * General container for manipulating data
 * For now, this will deal exclusively with X/Y values; eventually
 * this can become more generic in what it supports
 */
class Dataset {

    std::vector< Point > points_;
    Range xdomain_, ydomain_;

public:

    typedef std::vector< Point >::iterator iterator;
    typedef std::vector< Point >::const_iterator const_iterator;
    typedef std::vector< Point >::size_type size_type;

    Dataset () {}
    Dataset (const std::vector< Point >& pts) : points_(pts) {
        std::vector< Point >::const_iterator PIT = pts.begin (),
            PEND = pts.end ();
        if (pts.empty ()) { return; }
        FloatType xmin, xmax, ymin, ymax;
        xmin = xmax = PIT->X ();
        ymin = ymax = PIT->Y ();
        for (; PIT != PEND; ++PIT) {
            xmin = std::min (xmin, PIT->X ());
            xmax = std::max (xmax, PIT->X ());
            ymin = std::min (ymin, PIT->Y ());
            ymax = std::max (ymax, PIT->Y ());
        }
        xdomain_.Reset (xmin, xmax);
        ydomain_.Reset (ymin, ymax);
    }

    /* TODO: Provide const read-only version */
    void XData (std::vector< FloatType >& dest) const;
    void YData (std::vector< FloatType >& dest) const;

    /* 
     * TODO: This should resize range dynamically or a method
     * should be exposed to do it prior to the next plot
     * That or just remove this call
     */
    void Add (const Point& p) { points_.push_back (p); }
    size_type Size () const { return points_.size (); }

    const Range& XDomain () const { return xdomain_; }
    const Range& YDomain () const { return ydomain_; }

    const_iterator Begin () const { return points_.begin (); }
    const_iterator End () const { return points_.end (); }
};

#endif /* DATASET_H__ */
