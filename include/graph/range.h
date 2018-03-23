#ifndef RANGE_H__
#define RANGE_H__

#include <cmath>
#include <graph/types.h>
#include <graph/primitives.h>

class Range {

    FloatType x_, y_, low_, high_;

    /*
     * equality of float types within an epsilon tolerance
     */
    bool MostlyEqual (FloatType a, FloatType b) const;

public:

    Range () : x_(0.0), y_(0.0), low_(0.0), high_(0.0) {}
    Range (FloatType x, FloatType y);

    inline FloatType Low () const { return low_; }
    inline FloatType High () const { return high_; }
    inline FloatType X () const { return x_; }
    inline FloatType Y () const { return y_; }

    inline FloatType Distance () const { return fabs (y_ - x_); }

    void Reset (FloatType x, FloatType y);

    bool Contains (const FloatType& x) const;

};

/*
 * Affine transform from one range to another
 */
FloatType transform (const FloatType& x, const Range& from, const Range& to);

#endif /* RANGE_H__ */
