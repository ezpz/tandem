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

/* TODO: Make this better. simple struct with a 'Next' interface? something */
#define PLOT_SCATTER   0
#define PLOT_BOX_H     1
#define PLOT_BOX_V     2
#define PLOT_HIST_L    3
#define PLOT_HIST_R    4
#define PLOT_HIST_B    5
#define PLOT_HIST_T    6
#define PLOT_HEXBIN    7
#define MAX_PLOT       8

void change_plot (BasicPlot **plots, int *plot_type, 
        ALLEGRO_DISPLAY **screens, ALLEGRO_DISPLAY *source, Dataset& data,
        FloatType minx, FloatType maxx, FloatType miny, FloatType maxy) {
    int i = -1;
    if (source == screens[0]) { i = 0; }
    else if (source == screens[1]) { i = 1; }
    else if (source == screens[2]) { i = 2; }

    if (-1 == i) {
        return;
    }

    Parameters par;
    int old_type = plot_type[i];
    int new_type = (old_type + 1) % MAX_PLOT;
    plot_type[i] = new_type;

    delete plots[i];

    switch (new_type) {
        case PLOT_SCATTER:
            plots[i] = new ScatterPlot (screens[i]);
            plots[i]->Xlim (minx, maxx);
            plots[i]->Ylim (miny, maxy);
            plots[i]->Clear ();
            plots[i]->Grid ();
            plots[i]->Plot (data);
            plots[i]->XTicks ();
            plots[i]->YTicks ();
            plots[i]->Box ();
            plots[i]->Update ();
            break;
        case PLOT_BOX_H:
            plots[i] = new BoxPlot (screens[i]);
            plots[i]->Xlim (minx, maxx);
            plots[i]->Ylim (miny, maxy);
            plots[i]->Clear ();
            par = plots[i]->Par ();
            par.side = HORIZONTAL;
            plots[i]->Plot (data, par);
            plots[i]->XTicks ();
            plots[i]->XGrid ();
            //plots[i]->YTicks ();
            plots[i]->Box ();
            plots[i]->Update ();
            break;
        case PLOT_BOX_V:
            plots[i] = new BoxPlot (screens[i]);
            plots[i]->Xlim (minx, maxx);
            plots[i]->Ylim (miny, maxy);
            plots[i]->Clear ();
            par = plots[i]->Par ();
            par.side = VERTICAL;
            plots[i]->Plot (data, par);
            plots[i]->YTicks ();
            plots[i]->YGrid ();
            plots[i]->Box ();
            plots[i]->Update ();
            break;
        case PLOT_HIST_L:
            plots[i] = new HistogramPlot (screens[i]);
            plots[i]->Xlim (0.0, 1.0);
            plots[i]->Ylim (miny, maxy);
            plots[i]->Clear ();
            par = plots[i]->Par ();
            par.side = SIDE_LEFT;
            plots[i]->Plot (data, par);
            plots[i]->XTicks ();
            plots[i]->XGrid ();
            plots[i]->Box ();
            plots[i]->Update ();
            break;
        case PLOT_HIST_R:
            plots[i] = new HistogramPlot (screens[i]);
            plots[i]->Xlim (1.0, 0.0);
            plots[i]->Ylim (miny, maxy);
            plots[i]->Clear ();
            par = plots[i]->Par ();
            par.side = SIDE_RIGHT;
            plots[i]->Plot (data, par);
            plots[i]->XTicks ();
            plots[i]->XGrid ();
            plots[i]->Box ();
            plots[i]->Update ();
            break;
        case PLOT_HIST_B:
            plots[i] = new HistogramPlot (screens[i]);
            plots[i]->Xlim (minx, maxx);
            plots[i]->Ylim (1.0, 0.0);
            plots[i]->Clear ();
            par = plots[i]->Par ();
            par.side = SIDE_TOP;
            plots[i]->Plot (data, par);
            plots[i]->YTicks ();
            plots[i]->YGrid ();
            plots[i]->Box ();
            plots[i]->Update ();
            break;
        case PLOT_HIST_T:
            plots[i] = new HistogramPlot (screens[i]);
            plots[i]->Xlim (minx, maxx);
            plots[i]->Ylim (0, 1.0);
            plots[i]->Clear ();
            par = plots[i]->Par ();
            par.side = SIDE_BOTTOM;
            plots[i]->Plot (data, par);
            plots[i]->YTicks ();
            plots[i]->YGrid ();
            plots[i]->Box ();
            plots[i]->Update ();
            break;
        case PLOT_HEXBIN:
            plots[i] = new HexBinPlot (screens[i]);
            plots[i]->Xlim (minx, maxx);
            plots[i]->Ylim (miny, maxy);
            plots[i]->Clear ();
            plots[i]->Plot (data);
            plots[i]->YTicks ();
            plots[i]->XTicks ();
            plots[i]->Box ();
            plots[i]->Update ();
            break;
        default:
            throw GeneralException("Unknown plot type", __FILE__, __LINE__);
    }
}

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
    int plot_type[3] = {0};
    BasicPlot *plots[3] = {0};

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

    for (int j = 0; j < 3; ++j) {
        plots[j] = new ScatterPlot (screens[j]);
        if (NULL == plots[j]) {
            throw GeneralException ("Memory error", __FILE__, __LINE__);
        }
        plots[j]->Xlim (minx, maxx);
        plots[j]->Ylim (miny, maxy);
        plots[j]->Clear ();
        plots[j]->Grid ();
        plots[j]->Plot (data);
        plots[j]->YTicks ();
        plots[j]->XTicks ();
        plots[j]->Box ();
        plots[j]->Update ();
    }

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


    /*
     * FIXME: This should happen on every event in the system
     * capturing changes from the user.
     */

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
                if (ALLEGRO_KEY_N == event.keyboard.keycode) {
                    change_plot (plots, plot_type, screens, 
                            event.keyboard.display, 
                            data, minx, maxx, miny, maxy);
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



