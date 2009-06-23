#ifndef MTHREAD_VIEWER
#define MTHREAD_VIEWER

#include "simple_viewer.h"
#include <map>

class MThreadViewer : public SimpleViewer {
  public:

    MThreadViewer(GPlane * pl1, GPlane * pl2);
    ~MThreadViewer();

    void updater(const Rect<int> & r);
    void on_done_signal();
    void draw(const Rect<int> & r);

  private:
    GPlane *slow_plane;

    std::map<Point<int>,Image<int> > done_cache;
    Glib::Dispatcher          done_signal;
    Glib::Mutex              *mutex;
};

#endif
