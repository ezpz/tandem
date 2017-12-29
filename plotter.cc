#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <zmq.hpp>

#include "plot_util.h"
#include "plot.h"

static int DEFAULT_WIDTH = 0;
static int DEFAULT_HEIGHT = 0;

void draw_border (ALLEGRO_DISPLAY *scr, int w, int h, ALLEGRO_COLOR col) {
    al_set_target_bitmap (al_get_backbuffer (scr));
    int off = 2; 
    float lwd = 2.0;
    al_draw_rectangle (off, off, w - off, h - off, col, lwd);
}

int draw_crosshair (const Point& c, 
        const Point& o, const Point& xmax, const Point& ymax) {
    ALLEGRO_COLOR col = mkcol (36, 255, 36, 120);
    int in_cnt = 0;
    if (c.X () >= o.X () && c.X () <= xmax.X ()) {
        /* draw vertical line */
        ++in_cnt;
        al_draw_line(c.X (), o.Y (), c.X (), ymax.Y (), col, 1);
    }
    if (c.Y () <= o.Y () && c.Y () >= ymax.Y ()) {
        /* draw horizontal line */
        ++in_cnt;
        al_draw_line(o.X (), c.Y (), xmax.X (), c.Y (), col, 1);
    }
    return in_cnt;
}

void draw_coords (const Point& p, ALLEGRO_FONT *font) {
    ALLEGRO_COLOR col = mkcol (200, 200, 32, 200);
    int text_height = al_get_font_line_height (font), offset = text_height / 2;
    al_draw_textf (font, col,
            p.X () + offset, p.Y () - text_height, ALLEGRO_ALIGN_LEFT,
            "%d,%d", static_cast< int >(p.X ()), 
            static_cast< int >(fabs(p.Y () - DEFAULT_HEIGHT)));
}

void draw_labels (const Point& o, 
        const Point& x, const Point& y, ALLEGRO_FONT *font) {
    ALLEGRO_COLOR col = mkcol (200, 200, 200, 128);
    const int n = 10;
    int text_height = al_get_font_line_height (font);
    double xstep = (x.X () - o.X ()) / n;
    double ystep = (o.Y () - y.Y ()) / n;
    for (double d = o.X () + xstep; d <= x.X (); d += xstep) {
        al_draw_textf (font, col, 
                d, o.Y () + text_height, ALLEGRO_ALIGN_CENTER, 
                "%d", static_cast< int >(floor (d)));
    }
    for (double d = o.Y () - ystep, l = y.Y () + ystep; 
            d >= y.Y (); d -= ystep, l += ystep) {
        al_draw_textf (font, col, 
                o.X () - text_height, d - text_height / 2, ALLEGRO_ALIGN_RIGHT, 
                "%d", static_cast< int >(floor (l)));
    }
}

void draw_axes (const Point& o, const Point& x, const Point& y) {
    ALLEGRO_COLOR col = mkcol (36, 36, 255, 120);
    al_draw_line(o.X (), o.Y (), o.X (), y.Y (), col, 3);
    al_draw_line(o.X (), o.Y (), x.X (), o.Y (), col, 3);
}

void draw_grid (const Point& o, const Point& x, const Point& y) {
    ALLEGRO_COLOR col = mkcol (200, 200, 200, 32);
    const int n = 10;
    double xstep = (x.X () - o.X ()) / n;
    double ystep = (o.Y () - y.Y ()) / n;
    for (double d = o.X (); d <= x.X (); d += xstep) {
        al_draw_line (d, o.Y (), d, y.Y (), col, 0);
    }
    for (double d = o.Y (); d >= y.Y (); d -= ystep) {
        al_draw_line (o.X (), d, x.X (), d, col, 0);
    }
}

int main () {
    ALLEGRO_EVENT_QUEUE *events = NULL;
    ALLEGRO_DISPLAY *screens[3] = { NULL };
    ALLEGRO_FONT *font = NULL;

    int adapter_count = 0;
    int monitor_x = 0, monitor_y = 0, screen_x = 0, screen_y = 0;

    srand (time (NULL));

    if (! al_init ()) {
        fprintf (stderr, "Failed to init allegro\n");
        return 1;
    }

    adapter_count = al_get_num_video_adapters ();
    for (int i = 0; i < adapter_count; ++i) {
        ALLEGRO_MONITOR_INFO minfo;
        al_get_monitor_info (i, &minfo);
        /* grab main display dims */
        if (0 == minfo.x1 && 0 == minfo.y1) {
            monitor_x = minfo.x2;
            monitor_y = minfo.y2;
            screen_x = (monitor_x / 2) * 0.95;
            screen_y = (monitor_y / 2) * 0.95;
        }
    }

    DEFAULT_WIDTH = screen_x;
    DEFAULT_HEIGHT = screen_y;

    double origin_x = screen_x / 4.0;
    double origin_y = screen_y - screen_y / 4.0;
    double yaxis_max = origin_y - screen_y / 2.0;
    double xaxis_max = origin_x + screen_x / 2.0;

    Point cursor (0, 0), 
          origin (origin_x, origin_y), 
          xmax (xaxis_max, origin_y), 
          ymax (origin_x, yaxis_max);

    al_set_new_display_flags (ALLEGRO_NOFRAME);

    al_init_primitives_addon ();
    al_install_keyboard ();
    al_install_mouse ();
    al_init_font_addon ();
    al_init_ttf_addon ();

    screens[0] = al_create_display (screen_x, screen_y);
    screens[1] = al_create_display (screen_x, screen_y);
    screens[2] = al_create_display (screen_x, screen_y);

    if (! screens[0] || ! screens[1] || ! screens[2]) {
        fprintf (stderr, "Failed to create displays\n");
        return 1;
    }

    al_set_window_position (screens[0], monitor_x - screen_x, 
            monitor_y - 2 * screen_y);
    al_set_window_position (screens[1], monitor_x - screen_x, 
            monitor_y - screen_y);
    al_set_window_position (screens[2], monitor_x - 2 * screen_x, 
            monitor_y - screen_y);

    PlotArea plot_top(screens[0]), 
             plot_bottom(screens[2]), 
             plot_both(screens[1]);

    Range xlim (0, screen_x), ylim (0, screen_y);

    plot_top.SetXlim (xlim);
    plot_top.SetYlim (ylim);

    events = al_create_event_queue ();
    if (NULL == events) {
        fprintf (stderr, "Failed to create event queue\n");
        return 1;
    }

    al_register_event_source (events, al_get_display_event_source (screens[0]));
    al_register_event_source (events, al_get_display_event_source (screens[1]));
    al_register_event_source (events, al_get_display_event_source (screens[2]));

    al_register_event_source (events, al_get_keyboard_event_source ());
    al_register_event_source (events, al_get_mouse_event_source ());

    font = al_load_font ("fonts/FreeMono.ttf", 8, 0);
    if (NULL == font) { assert (0); }

    Options o = Options::Defaults ();
    o.col = mkcol (200, 180, 20, 100);
    o.cex = 2.5;
    for (int i = 0; i < screen_x; ++i) {
        float y = (screen_y / 2 ) + sinf (i * 0.0174) * (screen_y / 2);
        plot_top.DrawPoint (Point (i, y), o);
    }

    while (true) {

        ALLEGRO_EVENT event;
        al_wait_for_event (events, &event);

        /* Not the best place to have this */
        plot_top.Clear ();

        switch (event.type) {
            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                cursor.X (event.mouse.x);
                cursor.Y (screen_y - event.mouse.y);
                plot_top.DrawPoint (cursor);
                plot_top.DrawLine (cursor, Point (screen_x, screen_y));
                break;
            case ALLEGRO_EVENT_MOUSE_AXES:
                cursor.X (event.mouse.x);
                cursor.Y (event.mouse.y);
                if (! al_is_event_queue_empty (events)) {
                    /* TODO: jump to beginning of the switch */
                    while (! al_is_event_queue_empty (events)) {
                        al_get_next_event (events, &event);
                        cursor.X (event.mouse.x);
                        cursor.Y (event.mouse.y);
                    }
                }
                break;
            case ALLEGRO_EVENT_KEY_DOWN:
                if (ALLEGRO_KEY_ESCAPE == event.keyboard.keycode) {
                    goto outly;
                }
                break;
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                goto outly;
                break;
            default:
                /* */
                break;
        }

        plot_top.Box ();
        plot_top.Grid ();
        plot_top.Update ();
    }

outly:

    /* TODO: Display cleanup etrc. */
    return 0;
}



