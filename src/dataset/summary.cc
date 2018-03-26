
#include <vector>
#include <algorithm>
#include <dataset/summary.h>
#include <graph/exceptions.h> /* TODO: move to general include level */

/*
 * Assumes xs are sorted ascending
 * Finds median of values from start to end - 1
 */
static FloatType median (const std::vector< FloatType >& xs, 
        std::size_t start, std::size_t end) {
    std::size_t n = end - start, mid = start + n / 2;
    if (0 == n % 2) {
        return xs[mid - 1] + (xs[mid] - xs[mid - 1]) / 2.0;
    } else {
        return xs[mid];
    }
}

BoxPlotSummary::BoxPlotSummary (std::vector< FloatType >& xs) {

    if (xs.empty ()) {
        throw GeneralException ("Empty dataset", __FILE__, __LINE__);
    }

    if (1 == xs.size ()) {
        min_ = max_ = median_ = uq_ = lq_ = xs[0];
        qrange_ = 0.0;
        return;
    }

    std::size_t mid = xs.size () / 2;

    std::sort (xs.begin (), xs.end ());

    min_ = xs[0];
    max_ = xs[xs.size () - 1];
    median_ = median (xs, 0, xs.size ());
    lq_ = median (xs, 0, mid);
    if (0 == mid % 2) {
        uq_ = median (xs, mid, xs.size ());
    } else {
        uq_ = median (xs, mid + 1, xs.size ());
    }
    qrange_ = uq_ - lq_;
}
