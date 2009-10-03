#include "mthread_viewer.h"

MThreadViewer::MThreadViewer(GPlane * pl) :
    SimpleViewer(pl),
    fast_plane(&default_fast_plane){

  Glib::thread_init();
  done_signal.connect(sigc::mem_fun(*this, &MThreadViewer::on_done_signal));

  mutex = new(Glib::Mutex);
}

MThreadViewer::~MThreadViewer(){
  delete(mutex);
}

void MThreadViewer::set_fast_plane(){
  fast_plane = &default_fast_plane;
}

void MThreadViewer::set_fast_plane(GPlane * pl){
  fast_plane=pl;
}

GPlane * MThreadViewer::get_fast_plane() const {
  return fast_plane;
}

void MThreadViewer::updater(const iRect & r){
  int e=epoch;
  std::pair<iPoint, iImage> p(r.TLC(), SimpleViewer::get_plane()->draw(r));
  if (e==epoch){
    mutex->lock();
    done_cache.insert(p);
    mutex->unlock();
    done_signal.emit();
  }
}

void MThreadViewer::on_done_signal(){
  mutex->lock();
  for (std::map<iPoint,iImage>::const_iterator
       i=done_cache.begin(); i!=done_cache.end(); i++){

    draw_image(i->second, i->first-get_origin());
  }
  done_cache.clear();
  mutex->unlock();
}

void MThreadViewer::draw(const iRect & r){
  if (r.empty()) return;
  draw_image(fast_plane->draw(r + get_origin()), r.TLC());

  Glib::Thread::create(
    sigc::bind<1>(sigc::mem_fun(*this, &MThreadViewer::updater),
      r + get_origin()),
    false);
}

