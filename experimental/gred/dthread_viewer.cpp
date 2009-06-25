#include "dthread_viewer.h"

const int TILE_SIZE=256;
const int TILE_MARG=2;

DThreadViewer::DThreadViewer(GPlane * pl) :
    SimpleViewer(pl),
    fast_plane(&default_fast_plane),
    updater_needed(true) {

  Glib::thread_init();
  done_signal.connect(sigc::mem_fun(*this, &DThreadViewer::on_done_signal));

  updater_mutex = new(Glib::Mutex);
  updater_cond = new(Glib::Cond);
  updater_thread = Glib::Thread::create(sigc::mem_fun(*this, &DThreadViewer::updater), true);
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

void DThreadViewer::set_fast_plane(){
  fast_plane = &default_fast_plane;
}

void DThreadViewer::set_fast_plane(GPlane * pl){
  fast_plane=pl;
}

GPlane * DThreadViewer::get_fast_plane() const {
  return fast_plane;
}



iRect tile_to_rect(iPoint key){
  return iRect(key, key + iPoint(1,1))*TILE_SIZE;
}

void DThreadViewer::updater(){
  do {

    // generate tiles
    while (!tiles_todo.empty()){

      updater_mutex->lock();
      last_tile = *tiles_todo.begin();
      tiles_todo.erase(last_tile);
      updater_mutex->unlock();

      iImage tile = get_plane()->draw(tile_to_rect(last_tile));

      updater_mutex->lock();
      if (tiles_done.count(last_tile)>0) tiles_done.erase(last_tile);
      tiles_done.insert(std::pair<iPoint,iImage>(last_tile, tile));
      updater_mutex->unlock();

      done_signal.emit();
    }

    // cleanup cache
    iRect tiles_to_keep = rect_pump( tiles_on_rect(
        iRect(get_origin().x, get_origin().y,  get_width(), get_height()), TILE_SIZE), TILE_MARG);

    std::map<iPoint,iImage>::iterator it=tiles_done.begin(), it1;
    while (it!=tiles_done.end()) {
      if (point_in_rect(it->first, tiles_to_keep)) it++;
      else {
        it1=it; it1++;
        updater_mutex->lock();
        tiles_done.erase(it);
        updater_mutex->unlock();
        it=it1;
      }
    }

    updater_cond->wait(*updater_mutex);
    updater_mutex->unlock();
  }
  while (updater_needed);
}


void DThreadViewer::on_done_signal(){
    draw(tile_to_rect(last_tile)-get_origin());
std::cerr << "SIZE " << tiles_done.size() << "\n";
}


void DThreadViewer::draw(const iRect & r){
  if (r.empty()) return;
  iRect tiles = tiles_on_rect(r + get_origin(), TILE_SIZE);
  iPoint key;

  for (key.y = tiles.y; key.y<tiles.y+tiles.h; key.y++){
    for (key.x = tiles.x; key.x<tiles.x+tiles.w; key.x++){

      iRect rect=tile_to_rect(key);
      clip_rect_to_rect(rect,r + get_origin());
      if (rect.empty()) continue;

      if (tiles_done.count(key)==0){ // if there is no tile in cache
        draw_image(fast_plane->draw(rect), rect.TLC()-get_origin());
        updater_mutex->lock();
        tiles_todo.insert(key);
        updater_cond->signal();
        updater_mutex->unlock();
      }
      else {
        draw_image(tiles_done.find(key)->second,
          rect - key*TILE_SIZE, rect.TLC()-get_origin());
      }
    }
  }
}

