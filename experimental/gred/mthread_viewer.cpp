#include "mthread_viewer.h"

MThreadViewer::MThreadViewer(GPlane * pl) :
    SimpleViewer(&default_fast_plane),
    slow_plane(pl){

  Glib::thread_init();
  done_signal.connect(sigc::mem_fun(*this, &MThreadViewer::on_done_signal));

  mutex = new(Glib::Mutex);
}

MThreadViewer::~MThreadViewer(){
  delete(mutex);
}

void MThreadViewer::set_fast_plane(){
  SimpleViewer::set_plane( & default_fast_plane);
}

void MThreadViewer::set_fast_plane(GPlane * pl){
  SimpleViewer::set_plane(pl);
}

void MThreadViewer::set_slow_plane(GPlane * pl){
  slow_plane=pl;
}

GPlane * MThreadViewer::get_fast_plane() const {
  return SimpleViewer::get_plane();
}

GPlane * MThreadViewer::get_slow_plane() const {
  return slow_plane;
}

void MThreadViewer::updater(const iRect & r){
  std::pair<iPoint, iImage> p(r.TLC(), slow_plane->draw(r));

  mutex->lock();
  done_cache.insert(p);
  mutex->unlock();

  done_signal.emit();
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
  draw_image(get_plane()->draw(r + get_origin()), r.TLC());

  Glib::Thread * tile_updater_thread =
    Glib::Thread::create(
      sigc::bind<1>(sigc::mem_fun(*this, &MThreadViewer::updater),
        r + get_origin()),
      false);
}

