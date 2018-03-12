
#include <graph/range.h>
#include <graph/exceptions.h>

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

FloatType transform (const FloatType& x, const Range& from, const Range& to) {
    return ((x - from.X ()) * 
            ((to.Y () - to.X ())/(from.Y () - from.X ())) 
            + to.X ());
}

