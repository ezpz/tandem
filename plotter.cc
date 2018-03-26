#include <cstdio>
#include <cerrno>
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

extern "C" {
#include <unistd.h>
#include <libgen.h>
#include <sys/stat.h>
}

#include <graph/plot.h>
#include <graph/util.h>
#include <graph/dataset.h>

enum button_state {
    BUTTON_DOWN = 0,
    BUTTON_UP
};

void load (const char *csv, std::vector< Point > &pts) {
    FILE *fin = fopen (csv, "rb");
    if (NULL == fin) { return; }
    while (! ferror (fin) && ! feof (fin)) {
        FloatType x = 0, y = 0;
        if (2 != fscanf (fin, "%lf,%lf", &x, &y)) {
            return;
        }
        pts.push_back (Point (x,y));
        if (feof (fin)) { break; }
    }
}

void usage (const char *prog) {
    fprintf (stderr, "USAGE: %s <csv>\n", prog);
    fprintf (stderr, "-------------------\n");
    fprintf (stderr, " csv  CSV file with pairs of points\n");
    fprintf (stderr, "\n");
    exit(42);
}

bool valid_file (const char *path) {
    struct stat sb;
    if (-1 == stat (path, &sb)) {
        fprintf (stderr, "Invalid CSV argument: %s\n", strerror (errno));
        return false;
    }
    return true;
}

int main (int argc, char **argv) {

    ALLEGRO_EVENT_QUEUE *events = NULL;
    ALLEGRO_DISPLAY *screens[3] = { NULL };

    int adapter_count = 0;
    int monitor_x = 0, monitor_y = 0, screen_x = 0, screen_y = 0;
    FloatType minx = 0, miny = 0, maxx = 0, maxy = 0;
    const char *csv = NULL;
    /*
    button_state bstate = BUTTON_UP;
    */

    if (2 != argc) {
        char prog[1024] = {0};
        strncpy (prog, argv[0], 1024);
        usage (basename (prog));
    }

    csv = argv[1];

    if (! valid_file (csv)) {
        return 1;
    }

    srand (time (NULL));

    if (! al_init ()) {
        fprintf (stderr, "Failed to init allegro\n");
        return 1;
    }

    /* TODO: get aspect ratio to force/enable square graphs */
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

    /*
    Point cursor (0, 0), orig_cursor = cursor;
    */

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

    ScatterPlot scatterplot(screens[1]); 
    //HistogramPlot top_hist(screens[0]);
    BoxPlot top_hist(screens[0]);
    HistogramPlot bottom_hist(screens[2]);

    std::vector< Point > xs;
    load (csv, xs);
    Dataset data(xs);

    std::vector< Point >::const_iterator PIT = xs.begin (), PEND = xs.end ();
    minx = maxx = PIT->X ();
    miny = maxy = PIT->Y ();
    for (; PIT != PEND; ++PIT) {
        minx = std::min (minx, PIT->X ());
        maxx = std::max (maxx, PIT->X ());
        miny = std::min (miny, PIT->Y ());
        maxy = std::max (maxy, PIT->Y ());
    }

    /* Add buffers outside data so points dont appear on the plot edge */
    minx -= data.XDomain ().Distance () * 0.05;
    maxx += data.XDomain ().Distance () * 0.05;
    miny -= data.YDomain ().Distance () * 0.05;
    maxy += data.YDomain ().Distance () * 0.05;

    fprintf (stderr, "MAIN xmin:%0.2f xmax:%0.2f\n", minx, maxx);

    scatterplot.Xlim (minx, maxx);
    scatterplot.Ylim (miny, maxy);
    top_hist.Xlim (minx, maxx);
    top_hist.Ylim (miny, maxy);
    bottom_hist.Xlim (1.0, 0);
    bottom_hist.Ylim (miny, maxy);

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


    Parameters par;
    /*
     * FIXME: This should happen on every event in the system
     * capturing changes from the user.
     */
    scatterplot.Clear ();
    scatterplot.Grid ();
    scatterplot.Plot (data);
    scatterplot.XTicks ();
    scatterplot.YTicks ();
    scatterplot.Box ();
    scatterplot.Update ();

    top_hist.Clear ();
    par = top_hist.Par ();
    par.side = VERTICAL;
    top_hist.Plot (data, par);
    top_hist.XTicks ();
    top_hist.YTicks ();
    top_hist.YGrid ();
    top_hist.Box ();
    top_hist.Update ();

    bottom_hist.Clear ();
    par = bottom_hist.Par ();
    par.side = SIDE_RIGHT;
    bottom_hist.Plot (data, par);
    bottom_hist.XTicks ();
    bottom_hist.XGrid ();
    bottom_hist.Box ();
    bottom_hist.Update ();

    while (true) {

        ALLEGRO_EVENT event;
        al_wait_for_event (events, &event);

        /* Not the best place to have this */
        /*
        scatterplot.Clear ();
        */
        /*
        plot_bottom.Clear ();
        plot_both.Clear ();
        */

next_event:
        switch (event.type) {
            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                /*
                cursor.X (event.mouse.x);
                cursor.Y (event.mouse.y);
                */
                /*
                if (point_in_plot (scatterplot, cursor)) {
                    orig_cursor = cursor;
                    scatterplot.DrawSelection (orig_cursor, cursor);
                    plot_bottom.DrawSelection (orig_cursor, cursor);
                    plot_both.DrawSelection (orig_cursor, cursor);
                    plot_both.Histogram ();
                }
                */
                /*
                bstate = BUTTON_DOWN;
                */
                break;
            case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
                /*
                cursor.X (event.mouse.x);
                cursor.Y (event.mouse.y);
                */
                /*
                if (point_in_plot (scatterplot, cursor)) {
                    if (BUTTON_DOWN == bstate && !(cursor == orig_cursor)) {
                        scatterplot.DrawSelection (orig_cursor, cursor);
                        plot_bottom.DrawSelection (orig_cursor, cursor);
                        plot_both.DrawSelection (orig_cursor, cursor);
                        plot_both.Histogram ();
                    } else {
                        scatterplot.ClearSelection ();
                        plot_bottom.ClearSelection ();
                        plot_both.ClearSelection ();
                        plot_both.Histogram ();
                    }
                }
                */
                /*
                bstate = BUTTON_UP;
                */
                break;
            case ALLEGRO_EVENT_MOUSE_AXES:
                /*
                cursor.X (event.mouse.x);
                cursor.Y (event.mouse.y);
                */
                /*
                if (point_in_plot (scatterplot, cursor)) {
                    if (BUTTON_DOWN == bstate) {
                        scatterplot.DrawSelection (orig_cursor, cursor);
                        plot_bottom.DrawSelection (orig_cursor, cursor);
                        plot_both.DrawSelection (orig_cursor, cursor);
                        plot_both.Histogram ();
                    }
                }
                */
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

        if (! al_is_event_queue_empty (events)) {
            al_get_next_event (events, &event);
            goto next_event;
        }


        /*
        scatterplot.Update ();
        plot_bottom.Update ();
        plot_both.Update ();
         */
    }

outly:

    /* TODO: Display cleanup etrc. */
    return 0;
}



