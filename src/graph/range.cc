
#include <graph/range.h>
#include <graph/exceptions.h>

FloatType transform (const FloatType& x, const Range& from, const Range& to) {

    /*
     * FIXME: TODO: 
     * This function will handle values outside of the specified range
     * at some point it will be more flexible to move this check elsewhere
     */
    if (! from.Contains (x)) {
        throw NotInRange(x, from);
    }

    return ((x - from.X ()) * 
            ((to.Y () - to.X ())/(from.Y () - from.X ())) 
            + to.X ());
}

void Range::Reset (FloatType x, FloatType y) { 
        x_ = x; y_ = y; 
        low_ = std::min(x_, y_);
        high_ = std::max(x_, y_);
}

Range::Range (FloatType x, FloatType y) : x_(x), y_(y) {
    low_ = std::min(x_, y_);
    high_ = std::max(x_, y_);
    if (high_ == low_) {
        throw InvalidRange(x_, y_);
    }
}

