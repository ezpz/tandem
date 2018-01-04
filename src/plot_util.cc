
#include "plot_util.h"

void Options::Reset () {

    font = al_load_font ("fonts/FreeMono.ttf", 8, 0);
    if (NULL == font) { assert (0); }

    lwd = 1.0;
    cex = 1.0;

    oma.bottom = 3.0;
    oma.left = 5.0;
    oma.top = 7.0;
    oma.right = 2.0;

    xlim.low = 0.0;
    xlim.high = 1.0;
    ylim.low = 0.0;
    ylim.high = 1.0;

    xticks = 7;
    yticks = 5;

    align = ALIGN_CENTER;

    col = mkcol (50, 50, 255, 255);
    fill = mkcol (50, 50, 255, 50);
    sfill = mkcol (50, 220, 50, 50);
    font_px = al_get_font_line_height (font);
}

ALLEGRO_COLOR mkcol (int r, int g, int b, int a) {
    float fr = (float)r / 255.0;
    float fg = (float)g / 255.0;
    float fb = (float)b / 255.0;
    float fa = (float)a / 255.0;
    return al_map_rgba_f (fr * fa, fg * fa, fb * fa, fa);
}
