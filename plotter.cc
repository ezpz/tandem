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

#define DEFAULT_WIDTH   800.0
#define DEFAULT_HEIGHT  600.0

struct Point2d {
    double x, y;
};

std::vector< std::pair< int, int > > points;

ALLEGRO_COLOR mkcol (int r, int g, int b, int a) {
    float fr = (float)r / 255.0;
    float fg = (float)g / 255.0;
    float fb = (float)b / 255.0;
    float fa = (float)a / 255.0;
    return al_map_rgba_f (fr * fa, fg * fa, fb * fa, fa);
}

void draw_all_points () {
    ALLEGRO_COLOR col = mkcol (36, 200, 200, 120);
    std::vector< std::pair< int, int > >::iterator PIT = points.begin (), 
        PEND = points.end ();
    for (; PIT != PEND; ++PIT) {
        al_draw_filled_circle (PIT->first, PIT->second, 2, col);
    }
}

void draw_point (const Point2d& c,
        const Point2d& o, const Point2d& xmax, const Point2d& ymax) {
    if (c.x >= o.x && c.x <= xmax.x &&
            c.y <= o.y && c.y >= ymax.y) {
        points.push_back (std::make_pair (c.x, c.y));
    }
}

int draw_crosshair (const Point2d& c, 
        const Point2d& o, const Point2d& xmax, const Point2d& ymax) {
    ALLEGRO_COLOR col = mkcol (36, 255, 36, 120);
    int in_cnt = 0;
    if (c.x >= o.x && c.x <= xmax.x) {
        /* draw vertical line */
        ++in_cnt;
        al_draw_line(c.x, o.y, c.x, ymax.y, col, 1);
    }
    if (c.y <= o.y && c.y >= ymax.y) {
        /* draw horizontal line */
        ++in_cnt;
        al_draw_line(o.x, c.y, xmax.x, c.y, col, 1);
    }
    return in_cnt;
}

void draw_coords (const Point2d& p, ALLEGRO_FONT *font) {
    ALLEGRO_COLOR col = mkcol (200, 200, 32, 200);
    int text_height = al_get_font_line_height (font), offset = text_height / 2;
    al_draw_textf (font, col,
            p.x + offset, p.y - text_height, ALLEGRO_ALIGN_LEFT,
            "%d,%d", static_cast< int >(p.x), 
            static_cast< int >(fabs(p.y - DEFAULT_HEIGHT)));
}

void draw_labels (const Point2d& o, 
        const Point2d& x, const Point2d& y, ALLEGRO_FONT *font) {
    ALLEGRO_COLOR col = mkcol (200, 200, 200, 128);
    const int n = 10;
    int text_height = al_get_font_line_height (font);
    double xstep = (x.x - o.x) / n;
    double ystep = (o.y - y.y) / n;
    for (double d = o.x + xstep; d <= x.x; d += xstep) {
        al_draw_textf (font, col, 
                d, o.y + text_height, ALLEGRO_ALIGN_CENTER, 
                "%d", static_cast< int >(floor (d)));
    }
    for (double d = o.y - ystep, l = y.y + ystep; 
            d >= y.y; d -= ystep, l += ystep) {
        al_draw_textf (font, col, 
                o.x - text_height, d - text_height / 2, ALLEGRO_ALIGN_RIGHT, 
                "%d", static_cast< int >(floor (l)));
    }
}

void draw_axes (const Point2d& o, const Point2d& x, const Point2d& y) {
    ALLEGRO_COLOR col = mkcol (36, 36, 255, 120);
    al_draw_line(o.x, o.y, o.x, y.y, col, 3);
    al_draw_line(o.x, o.y, x.x, o.y, col, 3);
}

void draw_grid (const Point2d& o, const Point2d& x, const Point2d& y) {
    ALLEGRO_COLOR col = mkcol (200, 200, 200, 32);
    const int n = 10;
    double xstep = (x.x - o.x) / n;
    double ystep = (o.y - y.y) / n;
    for (double d = o.x; d <= x.x; d += xstep) {
        al_draw_line (d, o.y, d, y.y, col, 0);
    }
    for (double d = o.y; d >= y.y; d -= ystep) {
        al_draw_line (o.x, d, x.x, d, col, 0);
    }
}

int main () {
    ALLEGRO_EVENT_QUEUE *events = NULL;
    ALLEGRO_DISPLAY *screen = NULL;
    ALLEGRO_FONT *font = NULL;
    Point2d cursor, origin, xmax, ymax;

    double screen_width = DEFAULT_WIDTH, screen_height = DEFAULT_HEIGHT;

    double origin_x = screen_width / 4.0;
    double origin_y = screen_height - screen_height / 4.0;
    double yaxis_max = origin_y - screen_height / 2.0;
    double xaxis_max = origin_x + screen_width / 2.0;

    origin.x = origin_x;
    origin.y = origin_y;
    xmax.x = xaxis_max;
    xmax.y = origin_y;
    ymax.x = origin_x;
    ymax.y = yaxis_max;

    srand (time (NULL));

    if (! al_init ()) {
        fprintf (stderr, "Failed to init allegro\n");
        return 1;
    }

    screen = al_create_display (screen_width, screen_height);
    if (NULL == screen) {
        fprintf (stderr, "Failed to create display\n");
        return 1;
    }

    events = al_create_event_queue ();
    if (NULL == events) {
        fprintf (stderr, "Failed to create event queue\n");
        return 1;
    }

    al_init_primitives_addon ();
    al_install_keyboard ();
    al_install_mouse ();
    al_init_font_addon ();
    al_init_ttf_addon ();

    al_register_event_source (events, al_get_display_event_source (screen));
    al_register_event_source (events, al_get_keyboard_event_source ());
    al_register_event_source (events, al_get_mouse_event_source ());

    font = al_load_font ("fonts/FreeMono.ttf", 8, 0);
    if (NULL == font) { assert (0); }

    while (true) {

        ALLEGRO_EVENT event;
        int in_box = 0;
        al_wait_for_event (events, &event);

        /* Not the best place to have this */
        al_clear_to_color (al_map_rgb (0, 0, 0));

        switch (event.type) {
            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                cursor.x = event.mouse.x;
                cursor.y = event.mouse.y;
                draw_point (cursor, origin, xmax, ymax);
                break;
            case ALLEGRO_EVENT_MOUSE_AXES:
                cursor.x = event.mouse.x;
                cursor.y = event.mouse.y;
                if (! al_is_event_queue_empty (events)) {
                    /* TODO: jump to beginning of the switch */
                    while (! al_is_event_queue_empty (events)) {
                        al_get_next_event (events, &event);
                        cursor.x = event.mouse.x;
                        cursor.y = event.mouse.y;
                    }
                }
                break;
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                goto outly;
                break;
            default:
                /* */
                break;
        }

        draw_grid (origin, xmax, ymax);
        draw_axes (origin, xmax, ymax);
        draw_labels (origin, xmax, ymax, font);
        in_box = draw_crosshair (cursor, origin, xmax, ymax);
        draw_all_points ();
        if (2 == in_box) {
            draw_coords (cursor, font);
        }
        al_flip_display ();
    }

outly:
    return 0;
}



