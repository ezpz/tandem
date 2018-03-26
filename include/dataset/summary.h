#ifndef SUMMARY_H__
#define SUMMARY_H__

#include <vector>
#include <graph/types.h> /* TODO: move to top-level include */

class BoxPlotSummary {

    FloatType median_, max_, min_, uq_, lq_, qrange_;

    BoxPlotSummary ();
    BoxPlotSummary (const BoxPlotSummary&);

public:

    BoxPlotSummary (std::vector< FloatType >& xs);

    FloatType Median () const { return median_; }
    FloatType UpperQ () const { return uq_; }
    FloatType LowerQ () const { return lq_; }
    FloatType Max () const { return max_; }
    FloatType Min () const { return min_; }

    inline FloatType LowerBound () const { return lq_ - (1.5 * qrange_); }
    inline FloatType UpperBound () const { return uq_ + (1.5 * qrange_); }

    inline bool Outlier (FloatType x) const {
        return (x < LowerBound () || x > UpperBound ());
    }

};


#endif /* SUMMARY_H__ */
