#include <gtkmm.h>

#include <queue>
#include <map>

#include "simple_viewer.h"


// define this to reduce information transfer from workplane during scrolling
#define MANUAL_SCROLL


class MThreadViewer : public SimpleViewer {
  public:

    MThreadViewer(GPlane * pl1, GPlane * pl2) :
        SimpleViewer(pl1),
        slow_plane(pl2){

      Glib::thread_init();
      done_signal.connect(sigc::mem_fun(*this, &MThreadViewer::on_done_signal));

      mutex = new(Glib::Mutex);
    }
    ~MThreadViewer(){
      delete(mutex);
    }

  void updater(const Rect<int> & r){
    std::pair<Point<int>, Image<int> > p(r.TLC(), slow_plane->draw(r));

    mutex->lock();
    done_cache.insert(p);
    mutex->unlock();

    done_signal.emit();
  }

  void on_done_signal(){
    mutex->lock();
    for (std::map<Point<int>,Image<int> >::const_iterator
         i=done_cache.begin(); i!=done_cache.end(); i++){

      draw_image(i->second, i->first-origin);
    }
    done_cache.clear();
    mutex->unlock();
  }

  void draw(const Rect<int> & r){
    if (r.empty()) return;
    draw_image(plane->draw(r + origin), r.TLC());

    Glib::Thread * tile_updater_thread =
      Glib::Thread::create(
        sigc::bind<1>(sigc::mem_fun(*this, &MThreadViewer::updater),
          r + origin),
        false);
  }

  private:
    GPlane *slow_plane;

    std::map<Point<int>,Image<int> > done_cache;
    Glib::Dispatcher          done_signal;
    Glib::Mutex              *mutex;
};

