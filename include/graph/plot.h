#ifndef PLOT_H__
#define PLOT_H__

#include <string>
#include <vector>
#include <graph/primitives.h>
#include <graph/parameters.h>
#include <graph/exceptions.h>
#include <graph/range.h>
#include <graph/types.h>
#include <graph/dataset.h>

class ViewPort {

    Range xrange_, yrange_;

public:

    ViewPort () : xrange_(0.0, 1.0), yrange_(0.0, 1.0) {}

    inline const Range& XRange () const { return xrange_; }
    inline const Range& YRange () const { return yrange_; }

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

    FloatType DisplayWidth () const { return al_get_display_width (win_); }
    FloatType DisplayHeight () const { return al_get_display_height (win_); }

    bool Selected () const;

    void Initialize ();

protected:

    ALLEGRO_DISPLAY* Display () const { return win_; }
    const Range& XRange () const { return view_.XRange (); }
    const Range& YRange () const { return view_.YRange (); }
    void GrabFocus () const;

public:

    BasicPlot (ALLEGRO_DISPLAY *win) : win_(win) {
        if (NULL == win) {
            throw GeneralException ("Missing display", __FILE__, __LINE__);
        }
        Initialize ();
    }

    ~BasicPlot () {}

    const Parameters& Par () const { return par_; }
    void Par (const Parameters& par);

    void Xlim (FloatType low, FloatType high);
    void Ylim (FloatType low, FloatType high);

    void Update () const;
    void Clear () const;

    void Box () const;
    void Box (const Parameters& par) const;

    void Grid () const;
    void Grid (const Parameters& par) const;
    
    void XTicks () const;
    void XTicks (const Parameters& par) const;

    void YTicks () const;
    void YTicks (const Parameters& par) const;

    void Lines (const std::vector< Line >& lines) const;
    void Lines (const std::vector< Line >& lines, const Parameters& par) const;

    void Text (const Point& at, const std::string& text) const;
    void Text (const Point& at, const std::string& text, 
            const Parameters& par) const;
    
    virtual void Plot (const Dataset& data) = 0;
    virtual void Plot (const Dataset& data, const Parameters& par) = 0;
};


class ScatterPlot : public BasicPlot {

    ScatterPlot ();
    ScatterPlot (const ScatterPlot&);

public:

    ScatterPlot (ALLEGRO_DISPLAY *win) : BasicPlot(win) {}

    void Plot (const Dataset& data);
    void Plot (const Dataset& data, const Parameters& par);

};


class HistogramPlot : public BasicPlot {

    HistogramPlot ();
    HistogramPlot (const HistogramPlot&);

    void HistBottom (const Dataset& data, const Parameters& par);
    void HistLeft (const Dataset& data, const Parameters& par);
    void HistRight (const Dataset& data, const Parameters& par);
    void HistTop (const Dataset& data, const Parameters& par);

public:

    HistogramPlot (ALLEGRO_DISPLAY *win) : BasicPlot(win) {}

    void Plot (const Dataset& data);
    void Plot (const Dataset& data, const Parameters& par);

};

#endif /*PLOT_H__*/
