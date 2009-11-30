#ifndef DTHREAD_VIEWER
#define DTHREAD_VIEWER

#include "simple_viewer.h"
#include "gobj_solid_fill.h"
#include <map>
#include <set>
#include <queue>

// double-threaded viewer with square tiles

class DThreadViewer : public SimpleViewer {
  public:

    DThreadViewer(GObj * pl);
    ~DThreadViewer();

    iRect tile_to_rect(const iPoint & key) const;
    void updater();
    void on_done_signal();
    void draw(const iRect & r);

    void redraw (void);

    void set_fast_obj();
    void set_fast_obj(GObj * o);
    GObj * get_fast_obj() const;

  private:
    const static int TILE_SIZE=256;
    const static int TILE_MARG=2;

    GObj *fast_obj;
    GObjSolidFill default_fast_obj;

    std::map<iPoint,iImage> tiles_cache;
    std::set<iPoint>        tiles_todo;
    std::queue<iPoint>      tiles_done;

    Glib::Thread           *updater_thread;
    Glib::Mutex            *updater_mutex;
    Glib::Cond             *updater_cond;
    Glib::Dispatcher        done_signal;

    bool updater_needed;
};

#endif
