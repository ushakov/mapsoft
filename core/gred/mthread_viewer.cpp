#include "mthread_viewer.h"

MThreadViewer::MThreadViewer(GObj * pl) : SimpleViewer(pl){

  if (!Glib::thread_supported()) Glib::thread_init();
  done_signal.connect(sigc::mem_fun(*this, &MThreadViewer::on_done_signal));

  mutex = new(Glib::Mutex);
}

MThreadViewer::~MThreadViewer(){
  delete(mutex);
}

void MThreadViewer::updater(const iRect & r){
  int e=get_epoch();

  iImage img(r.w, r.h, get_bgcolor());
  GObj * obj = get_obj();
  if (obj) obj->draw(img, r.TLC());

  std::pair<iPoint, iImage> p(r.TLC(), img);
  if (e==get_epoch()){
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

  iImage img(r.w, r.h, get_bgcolor());
  draw_image(img, r.TLC());

  Glib::Thread::create(
    sigc::bind<1>(sigc::mem_fun(*this, &MThreadViewer::updater),
      r + get_origin()),
    false);
}

