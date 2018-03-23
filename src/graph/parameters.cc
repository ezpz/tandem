
#include <graph/parameters.h>
#include <graph/exceptions.h>
#include <graph/util.h>

void Parameters::LoadFont (FloatType cex) {
    int sz = static_cast< int >(floor (cex * DEFAULT_FONT_SIZE));
    if (sz <= 0) { sz = 1; }
    font = al_load_font ("fonts/FreeMono.ttf", sz, 0);
    if (NULL == font) { 
        throw GeneralException ("Failed to load font", __FILE__, __LINE__);
    }
    font_px = al_get_font_line_height (font);
}

void Parameters::SetXDomain (FloatType low, FloatType high) { 
    xdomain.Reset (low, high); 
}

void Parameters::SetYDomain (FloatType low, FloatType high) { 
    ydomain.Reset (low, high); 
}

void Parameters::Reset () {

    lwd = 1.0;
    cex = 1.0;
    rad = 2.0;

    LoadFont (cex);

    oma.bottom = 3.0;
    oma.left = 5.0;
    oma.top = 7.0;
    oma.right = 2.0;

    xticks = 7;
    yticks = 5;

    nbins = 20;

    align = ALIGN_CENTER;

    col = mkcol (50, 50, 255, 255);
    fill = mkcol (50, 50, 255, 50);
    sfill = mkcol (50, 220, 50, 50);
}
