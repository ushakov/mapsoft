#ifndef DTHREAD_VIEWER
#define DTHREAD_VIEWER

#include "simple_viewer.h"
#include <map>
#include <set>
#include <queue>

///\addtogroup gred
///@{
///\defgroup dthread_viewer
///Double-threaded viewer with square tiles.
///@{

/**
Двухпотоковый вьюер с квадратными плитками.
*/
class DThreadViewer : public SimpleViewer {
  public:

    DThreadViewer(GObj * pl);
    ~DThreadViewer();

    iRect tile_to_rect(const iPoint & key) const;
    void updater();
    void on_done_signal();
    void draw(const iRect & r);

    void redraw (void);

  private:
    const static int TILE_SIZE=256;
    const static int TILE_MARG=2;

    std::map<iPoint,iImage> tiles_cache;
    std::set<iPoint>        tiles_todo;
    std::queue<iPoint>      tiles_done;

    Glib::Thread           *updater_thread;
    Glib::Mutex            *updater_mutex;
    Glib::Cond             *updater_cond;
    Glib::Dispatcher        done_signal;

    bool updater_needed; // to stop updater on exit
    bool stop_drawing; // to avoid caching of old tiles
};

#endif
