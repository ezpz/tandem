#ifndef PLOT_H__
#define PLOT_H__

#include <string>
#include <vector>
#include "plot_util.h"

class PlotArea {

    ALLEGRO_DISPLAY *win_;
    Options opt_;
    Border border_;

    Range xlim, ylim;

    float width_, height_;

    std::vector< PointData > points_;
    std::vector< LineData > lines_;
    std::vector< TextData > strings_;

    float Width () const { return width_; }
    float Height () const { return height_; }

    void DrawAllPoints () const;
    void DrawAllLines () const;
    void DrawAllText () const;

    /*
     * Clip line to plot area 
     */
    Line ClipLine (const Line &line);

    bool Contains (const Point &p) const;

    /*
     * Ploat area is normalized to [0.0,1.0], this converts a point or line
     * in absolute space to the normalized space
     */
    Point Normalize (const Point &p) const;
    Line Normalize (const Line &l) const;

    /*
     * General orientation and basic geometric settings established
     * during initial plot instantiation
     */
    void Setup (const Options& o);

    inline void GrabFocus () const {
        if (win_ != al_get_current_display ()) {
            al_set_target_bitmap (al_get_backbuffer (win_));
        }
    }

    PlotArea ();
    PlotArea (const PlotArea&);

public:

    PlotArea (ALLEGRO_DISPLAY *win) : win_(win), opt_(Options::Defaults ()) {
        assert(NULL != win);
        width_ = al_get_display_width (win);
        height_ = al_get_display_height (win);
        Setup (opt_);
    }

    PlotArea (ALLEGRO_DISPLAY *win, const Options &o) : win_(win), opt_(o) {
        assert(NULL != win);
        width_ = al_get_display_width (win);
        height_ = al_get_display_height (win);
        Setup (opt_);
    }

    inline void Clear () const {
        GrabFocus ();
        al_clear_to_color (al_map_rgb (0, 0, 0));
    }

    inline void Update () const {
        GrabFocus ();
        DrawAllPoints ();
        DrawAllLines ();
        DrawAllText ();
        al_flip_display ();
    }

    void SetXlim (const Range& rng);
    void SetYlim (const Range& rng);

    /*
     * Draw a point within the plot area
     */
    void DrawPoint (const Point &p);
    void DrawPoint (const Point &p, const Options &o);

    /*
     * Draw a clipped line in the plot area
     */
    void DrawLine (const Point &a, const Point &b);
    void DrawLine (const Point &a, const Point &b, const Options &o);
    void DrawLine (const Line &l);
    void DrawLine (const Line &l, const Options &o);

    /*
     * Draw a bounding box within the plot window
     */
    void DrawRectangle (const Point &ll, const Point &ur);
    void DrawRectangle (const Point &ll, const Point &ur, const Options &o);

    /*
     * Draw a string on the plot
     */
    void DrawText (const Point &p, const std::string &txt);
    void DrawText (const Point &p, const std::string &txt, const Options &o);

    /*
     * Draw a box defining the plot area
     */
    void Box ();
    void Box (const Options &o);

    /*
     * Draw a grid on the plot surface
     */
    void Grid ();
    void Grid (const Options &o);

    /*
     * Place axis ticks/labels on the selected axis
     * 1 - bottom
     * 2 - left
     * 3 - top
     * 4 - right
     */
    void Axis (int which);
    void Axis (int which, const Options &o);
};

#endif /*PLOT_H__*/
