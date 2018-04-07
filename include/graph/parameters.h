#ifndef PARAMETERS_H__
#define PARAMETERS_H__

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <graph/types.h>
#include <graph/range.h>

#define ALIGN_LEFT ALLEGRO_ALIGN_LEFT
#define ALIGN_CENTER ALLEGRO_ALIGN_CENTER
#define ALIGN_RIGHT ALLEGRO_ALIGN_RIGHT


/*
 * pixel measurements of distance from window border to
 * viewport edge
 */
struct Margin {
    FloatType top, bottom, left, right;
};


struct Parameters {

    ColorType           col;        /* default point/line color */
    ColorType           fill;       /* default fill color */
    ColorType           sfill;      /* fill color for selection */
    FloatType           lwd;        /* line width */
    FloatType           cex;        /* expansion factor for points/text */
    FloatType           rad;        /* radius of points */
    ALLEGRO_FONT        *font;      /* default font */
    FloatType           font_px;    /* height of font in px */
    int                 xticks;     /* number of tick marks on x axis */
    int                 yticks;     /* number of tick marks on y axis */
    int                 align;      /* text alignment */
    int                 nbins;      /* number of histogram bins */
    
    Orientation         side;       /* which side of the plot */

    Range               xdomain;    /* The x domain of the dataset values */
    Range               ydomain;    /* The y domain of the dataset values */

    Margin              oma;        /* outer margins */

    /* Reset all default values */
    void Reset ();

    void SetXDomain (FloatType low, FloatType high);
    void SetYDomain (FloatType low, FloatType high);

    void LoadFont (FloatType cex);
    
    Parameters () { Reset (); }

    static const Parameters& Defaults () {
        static Parameters defaults;
        return defaults;
    }

};


#endif /* PARAMETERS_H__ */
