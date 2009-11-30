#ifndef MTHREAD_VIEWER
#define MTHREAD_VIEWER

#include "simple_viewer.h"
#include "gobj_solid_fill.h"
#include <map>

class MThreadViewer : public SimpleViewer {
  public:

    MThreadViewer(GObj * o);
    ~MThreadViewer();

    void updater(const iRect & r);
    void on_done_signal();
    void draw(const iRect & r);

    void set_fast_obj();
    void set_fast_obj(GObj * o);
    GObj * get_fast_obj() const;

  private:
    GObj *fast_obj;
    GObjSolidFill default_fast_obj;

    std::map<iPoint,iImage> done_cache;
    Glib::Dispatcher        done_signal;
    Glib::Mutex            *mutex;
};

#endif
