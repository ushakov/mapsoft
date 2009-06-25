#ifndef DTHREAD_VIEWER
#define DTHREAD_VIEWER

#include "simple_viewer.h"
#include <map>
#include <set>

// double-threaded viewer with square tiles

class DThreadViewer : public SimpleViewer {
  public:

    DThreadViewer(GPlane * pl);
    ~DThreadViewer();

    void updater();
    void on_done_signal();
    void draw(const iRect & r);

    void set_fast_plane();
    void set_fast_plane(GPlane * pl);
    GPlane * get_fast_plane() const;

//    void set_origin (const iPoint & new_origin);
    void draw_tile(const iPoint & key);

  private:
    GPlane *fast_plane;
    GPlaneSolidFill default_fast_plane;

    std::map<iPoint,iImage> tiles_done;
    std::set<iPoint>        tiles_todo;
    iPoint                  last_tile;

    Glib::Thread           *updater_thread;
    Glib::Mutex            *updater_mutex;
    Glib::Cond             *updater_cond;
    Glib::Dispatcher        done_signal;

    bool updater_needed;
};

#endif
