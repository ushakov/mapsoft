#ifndef MTHREAD_VIEWER
#define MTHREAD_VIEWER

#include "simple_viewer.h"
#include <map>

class MThreadViewer : public SimpleViewer {
  public:

    MThreadViewer(GObj * o);
    ~MThreadViewer();

    void updater(const iRect & r);
    void on_done_signal();
    void draw(const iRect & r);

  private:
    std::map<iPoint,iImage> done_cache;
    Glib::Dispatcher        done_signal;
    Glib::Mutex            *mutex;
};

#endif
