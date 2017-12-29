
#include "plot_util.h"

void Options::Reset () {

    font = al_load_font ("fonts/FreeMono.ttf", 8, 0);
    if (NULL == font) { assert (0); }

    lwd = 1.0;
    cex = 1.0;

    oma.bottom = 12.0;
    oma.left = 12.0;
    oma.top = 13.0;
    oma.right = 11.0;

    xlim.low = 0.0;
    xlim.high = 1.0;
    ylim.low = 0.0;
    ylim.high = 1.0;

    col = mkcol (50, 50, 255, 255);
    font_px = al_get_font_line_height (font);
}

ALLEGRO_COLOR mkcol (int r, int g, int b, int a) {
    float fr = (float)r / 255.0;
    float fg = (float)g / 255.0;
    float fb = (float)b / 255.0;
    float fa = (float)a / 255.0;
    return al_map_rgba_f (fr * fa, fg * fa, fb * fa, fa);
}
