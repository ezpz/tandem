#ifndef PRIMITIVES_H__
#define PRIMITIVES_H__

#include <graph/types.h>

class Point {

    FloatType x_, y_;

public:

    Point (const Point& other) : x_(other.X ()), y_(other.Y ()) {}
    Point (const FloatType& x, const FloatType& y) : x_(x), y_(y) {}
    Point () : x_(0.0), y_(0.0) {}

    inline FloatType X () const { return x_; }
    inline FloatType Y () const { return y_; }

    inline bool operator== (const Point& that) const {
        return (X () == that.X () && Y () == that.Y ());
    }
};

class Line {

    Point start_, end_;

public:

    Line (const Line& other) : start_(other.Start ()), end_(other.End ()) {}
    Line (const Point& start, const Point& end) : start_(start), end_(end) {}

    inline const Point& Start () const { return start_; }
    inline const Point& End () const { return end_; }

};

#endif /* PRIMITIVES_H__ */
