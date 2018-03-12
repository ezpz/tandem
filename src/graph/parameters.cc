
#include <graph/parameters.h>
#include <graph/exceptions.h>
#include <graph/util.h>

void Parameters::Reset () {

    font = al_load_font ("fonts/FreeMono.ttf", 8, 0);
    if (NULL == font) { 
        throw GeneralException ("Missing font", __FILE__, __LINE__);
    }

    lwd = 1.0;
    cex = 1.0;

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
    font_px = al_get_font_line_height (font);
}
