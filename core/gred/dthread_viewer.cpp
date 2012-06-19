#include "dthread_viewer.h"

DThreadViewer::DThreadViewer(GObj * pl) :
    SimpleViewer(pl),
    updater_needed(true) {

  if (!Glib::thread_supported()) Glib::thread_init();
  done_signal.connect(sigc::mem_fun(*this, &DThreadViewer::on_done_signal));

  updater_mutex = new(Glib::Mutex);
  updater_cond = new(Glib::Cond);
  updater_thread =
    Glib::Thread::create(sigc::mem_fun(*this, &DThreadViewer::updater), true);
}

DThreadViewer::~DThreadViewer(){
  updater_mutex->lock();
  updater_needed = false;
  updater_cond->signal();
  updater_mutex->unlock();
  updater_thread->join(); // waiting for our thread to exit
  delete(updater_mutex);
  delete(updater_cond);
}

void
DThreadViewer::redraw (void){
  updater_mutex->lock();
  stop_drawing=true;
  tiles_cache.clear();
  updater_mutex->unlock();
  draw(iRect(0, 0, get_width(), get_height()));
}

iRect
DThreadViewer::tile_to_rect(const iPoint & key) const{
  return iRect(key, key + iPoint(1,1))*TILE_SIZE;
}

void
DThreadViewer::updater(){
  do {

    // generate tiles
    updater_mutex->lock();
    if (!tiles_todo.empty()){

      iPoint key = *tiles_todo.begin();

      stop_drawing=false;
      updater_mutex->unlock();

      iImage tile(TILE_SIZE, TILE_SIZE, 0xFF000000 | get_bgcolor());
      GObj * o = get_obj();
      if (o) o->draw(tile, tile_to_rect(key).TLC());

      updater_mutex->lock();
      if (!stop_drawing){
        if (tiles_cache.count(key)>0) tiles_cache.erase(key);
        tiles_cache.insert(std::pair<iPoint,iImage>(key, tile));
        tiles_done.push(key);
        tiles_todo.erase(key);
        done_signal.emit();
      }
    }
    updater_mutex->unlock();

    // cleanup queue
    iRect tiles_to_keep = tiles_on_rect(
        iRect(get_origin().x, get_origin().y,  get_width(), get_height()), TILE_SIZE);

    updater_mutex->lock();
    std::set<iPoint>::iterator qit=tiles_todo.begin(), qit1;
    while (qit!=tiles_todo.end()) {
      if (point_in_rect(*qit, tiles_to_keep)) qit++;
      else {
        qit1=qit; qit1++;
        tiles_todo.erase(qit);
        qit=qit1;
      }
    }
    updater_mutex->unlock();

    // cleanup cache
    tiles_to_keep = rect_pump(tiles_to_keep, TILE_MARG);

    updater_mutex->lock();
    std::map<iPoint,iImage>::iterator it=tiles_cache.begin(), it1;
    while (it!=tiles_cache.end()) {
      if (point_in_rect(it->first, tiles_to_keep)) it++;
      else {
        it1=it; it1++;
        tiles_cache.erase(it);
        it=it1;
      }
    }
    updater_mutex->unlock();

    updater_mutex->lock();
    if (tiles_todo.empty()) updater_cond->wait(*updater_mutex);
    updater_mutex->unlock();
  }
  while (updater_needed);
}


void DThreadViewer::on_done_signal(){

  while (!tiles_done.empty()){
    iPoint key=tiles_done.front();

    if (tiles_cache.count(key))
      draw_image(tiles_cache.find(key)->second,
        iRect(0,0,1,1)*TILE_SIZE,
        key*TILE_SIZE-get_origin());

    updater_mutex->lock();
    tiles_done.pop();
    updater_mutex->unlock();
  }
  if (tiles_todo.empty()) signal_idle().emit();
}


void DThreadViewer::draw(const iRect & r){
  if (r.empty()) return;
  iRect tiles = tiles_on_rect(r + get_origin(), TILE_SIZE);
  iPoint key;

  updater_mutex->lock();
  if (tiles_todo.empty()) signal_busy().emit();
  updater_mutex->unlock();

  for (key.y = tiles.y; key.y<tiles.y+tiles.h; key.y++){
    for (key.x = tiles.x; key.x<tiles.x+tiles.w; key.x++){

      iRect rect=tile_to_rect(key);

      if (tiles_cache.count(key)==0){ // if there is no tile in cache
        iImage img(TILE_SIZE, TILE_SIZE, get_bgcolor());
        draw_image(img, rect.TLC()-get_origin());
        if (tiles_todo.count(key)==0){
          updater_mutex->lock();
          tiles_todo.insert(key);
          updater_cond->signal();
          updater_mutex->unlock();
        }
      }
      else {
        draw_image(tiles_cache.find(key)->second,
          rect - key*TILE_SIZE, rect.TLC()-get_origin());
      }
    }
  }
  updater_mutex->lock();
  if (tiles_todo.empty()) signal_idle().emit();
  updater_mutex->unlock();
}

