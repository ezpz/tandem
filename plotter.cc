#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <ctime>
#include <string>
#include <algorithm>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <zmq.hpp>

#include "plot_util.h"
#include "plot.h"

int main () {
    ALLEGRO_EVENT_QUEUE *events = NULL;
    ALLEGRO_DISPLAY *screens[3] = { NULL };

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
    plot_bottom.SetXlim (xlim);
    plot_bottom.SetYlim (ylim);

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

    /* Test points to specific plot */
    Options o = Options::Defaults ();
    o.col = mkcol (200, 180, 20, 100);
    o.cex = 2.5;
    for (int i = 0; i < screen_x; i += 5) {
        float y = (screen_y / 2 ) + sinf (i * 0.0174) * (screen_y / 2);
        plot_top.DrawPoint (Point (i, y), o);
        plot_bottom.DrawPoint (Point (i, y), o);
    }

    /* Test line clipping */
    Line l(Point (-30, 3 * (screen_y / 4.0)), 
            Point (2 * screen_x, screen_y / 4));
    o.lwd = 3.0;
    plot_top.DrawLine (l, o);
    plot_bottom.DrawLine (l, o);

    o = Options::Defaults ();
    o.lwd = 0.0;
    o.col = mkcol (233, 233, 233, 20);
    plot_top.Grid (o);
    plot_bottom.Grid (o);

    o.lwd = 1.0;
    plot_top.Box (o);
    plot_bottom.Box (o);

    o.lwd = 3.0;
    o.col = mkcol (20, 80, 255, 200);
    o.align = ALIGN_CENTER;
    plot_top.Axis (1, o);
    plot_bottom.Axis (1, o);
    o.align = ALIGN_RIGHT;
    plot_top.Axis (2, o);
    plot_bottom.Axis (2, o);


    while (true) {

        ALLEGRO_EVENT event;
        al_wait_for_event (events, &event);

        /* Not the best place to have this */
        plot_top.Clear ();
        plot_bottom.Clear ();

        switch (event.type) {
            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                cursor.X (event.mouse.x);
                cursor.Y (screen_y - event.mouse.y);
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

        plot_top.Update ();
        plot_bottom.Update ();
    }

outly:

    /* TODO: Display cleanup etrc. */
    return 0;
}



