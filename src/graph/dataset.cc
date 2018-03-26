

#include <graph/dataset.h> /* TODO: move to new location */

void Dataset::XData (std::vector< FloatType >& dest) const {
    const_iterator PIT = Begin (), PEND = End ();
    dest.clear ();
    dest.reserve (Size ());
    for (; PIT != PEND; ++PIT) {
        dest.push_back (PIT->X ());
    }
}

void Dataset::YData (std::vector< FloatType >& dest) const {
    const_iterator PIT = Begin (), PEND = End ();
    dest.clear ();
    dest.reserve (Size ());
    for (; PIT != PEND; ++PIT) {
        dest.push_back (PIT->Y ());
    }
}

