#ifndef MTHREAD_VIEWER
#define MTHREAD_VIEWER

#include "simple_viewer.h"
#include <map>

class MThreadViewer : public SimpleViewer {
  public:

    MThreadViewer(GPlane * pl);
    ~MThreadViewer();

    void updater(const iRect & r);
    void on_done_signal();
    void draw(const iRect & r);

    void set_fast_plane();
    void set_fast_plane(GPlane * pl);
    GPlane * get_fast_plane() const;

  private:
    GPlane *fast_plane;
    GPlaneSolidFill default_fast_plane;

    std::map<iPoint,iImage> done_cache;
    Glib::Dispatcher        done_signal;
    Glib::Mutex            *mutex;
};

#endif
