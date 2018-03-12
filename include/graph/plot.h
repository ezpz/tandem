#ifndef PLOT_H__
#define PLOT_H__

#include <string>
#include <vector>
#include <graph/primitives.h>
#include <graph/parameters.h>
#include <graph/exceptions.h>
#include <graph/range.h>
#include <graph/types.h>

class ViewPort {

    Range xrange_, yrange_;

public:

    inline const Range& XRange () const { return xrange_; }
    inline const Range& YRange () const { return yrange_; }

    ViewPort () : xrange_(0.0, 1.0), yrange_(0.0, 1.0) {}

    void SetXRange (FloatType xmin, FloatType xmax) {
        xrange_.Reset (xmin, xmax);
    }

    void SetYRange (FloatType ymin, FloatType ymax) {
        yrange_.Reset (ymin, ymax);
    }

};


class BasicPlot {

    ALLEGRO_DISPLAY *win_;
    Parameters par_; 
    ViewPort view_;

    BasicPlot ();
    BasicPlot (const BasicPlot&);

    void Initialize ();

protected:

    ALLEGRO_DISPLAY* Display () const { return win_; }
    FloatType DisplayWidth () const { return al_get_display_width (win_); }
    FloatType DisplayHeight () const { return al_get_display_height (win_); }
    Parameters& Par () { return par_; }
    ViewPort& PlotArea () { return view_; }

public:

    BasicPlot (ALLEGRO_DISPLAY *win) : win_(win) {
        if (NULL == win) {
            throw GeneralException ("Missing display", __FILE__, __LINE__);
        }
        Initialize ();
    }

    ~BasicPlot () {}

    void GrabFocus () const;
    void Update () const;

    virtual void Clear () const;
    virtual void Box () const;

    virtual void Points (const std::vector< Point >& points);
    virtual void Lines (const std::vector< Line >& points);
    
};

class ScatterPlot : public BasicPlot {

    Range xdomain_, ydomain_;

    ScatterPlot ();
    ScatterPlot (const ScatterPlot&);

public:

    ScatterPlot (ALLEGRO_DISPLAY *win) : BasicPlot(win) {}

    inline const Range& XRange () const { return xdomain_; }
    inline const Range& YRange () const { return ydomain_; }

    void Xlim (FloatType low, FloatType high);
    void Ylim (FloatType low, FloatType high);

    void Points (const std::vector< Point >& points);
    void Lines (const std::vector< Line >& lines);

};


#endif /*PLOT_H__*/
