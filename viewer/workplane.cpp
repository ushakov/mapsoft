#include <assert.h>
#include "workplane.h"


Workplane::Workplane(void): stop_drawing(false){ }

int
Workplane::draw(iImage &img, const iPoint &origin){

        draw_mutex.lock();
        stop_drawing=false;
        iRect tile(origin.x, origin.y, img.w, img.h );
	for (std::multimap<int, GObj *>::reverse_iterator
                itl = layers.rbegin(); itl != layers.rend();  ++itl){
	    GObj * gobj = itl->second;
	    if (layers_active[gobj]) {
                // copy to prevent deleting from another thread
                boost::shared_ptr<LayerCache> cache = tile_cache[gobj];
		if (!cache->contains(tile)) {
		    cache->add(tile, gobj->get_image(tile));
		}
		iImage& tile_img = cache->get(tile);
                draw_mutex.unlock();
		if (tile_img.w != 0) img.render(0,0,tile_img);

                draw_mutex.lock();
                // in this moment layers can be change, so we need this:
                if (stop_drawing){
                  stop_drawing=false;
                  draw_mutex.unlock();
                  return GOBJ_FILL_NONE;
                }
	    }
	}
        draw_mutex.unlock();
        // кажется, что когда будет нечто, использующее код возврата,
        // workplane можно будет им заменить! Так что не смотрим, нарисовали ли мы что-нибудь.
	return GOBJ_FILL_PART;
}

std::multimap<int, GObj *>::iterator
Workplane::find_gobj (GObj * gobj) {
	for (std::multimap<int, GObj *>::iterator itl = layers.begin();
	     itl != layers.end(); ++itl) {
	    if (itl->second == gobj) { return itl; }
	}
	return layers.end();
}

void
Workplane::add_gobj (GObj * gobj, int depth) {
//      std::cout << "Adding gobj " << gobj << " at depth " << depth << std::endl;
	if (find_gobj(gobj) != layers.end()) {
	    std::cout << "Already have this gobj!" << std::endl;
	    set_gobj_depth (gobj, depth);
	    assert(0);
	    return;
	}
        Glib::Mutex::Lock lock(draw_mutex);
        stop_drawing=true;
	layers.insert (std::make_pair (depth, gobj));
	gobj->set_cnv(cnv, cnv_hint);
	layers_active[gobj] = true;
	tile_cache[gobj].reset(new LayerCache(CacheCapacity));
}

void
Workplane::remove_gobj (GObj * gobj){
//	std::cout << "Removing gobj " << gobj << std::endl;
	std::multimap<int, GObj *>::iterator itl = find_gobj(gobj);
	if (itl == layers.end()) {
	    std::cout << "No such gobj " << gobj << std::endl;
	    assert(0);
	    return;
	}
        Glib::Mutex::Lock lock(draw_mutex);
        stop_drawing=true;
	layers_active.erase(gobj);
	layers.erase(itl);
	tile_cache.erase(gobj);
}

void
Workplane::clear(){
        Glib::Mutex::Lock lock(draw_mutex);
        stop_drawing=true;
	layers_active.clear();
	layers.clear();
	tile_cache.clear();
}

void
Workplane::set_gobj_depth (GObj * gobj, int newdepth){
	std::multimap<int, GObj *>::iterator itl = find_gobj(gobj);
	if (itl == layers.end()) {
	    std::cout << "No such gobj " << gobj << std::endl;
	    assert(0);
	    return;
	}
        Glib::Mutex::Lock lock(draw_mutex);
        stop_drawing=true;
	layers.erase(itl);
	layers.insert(std::make_pair(newdepth, gobj));
}

int
Workplane::get_gobj_depth (GObj * gobj){
	std::multimap<int, GObj *>::iterator itl = find_gobj(gobj);
	if (itl == layers.end()) {
	    std::cout << "No such gobj " << gobj << std::endl;
	    assert(0);
	    return -1;
	}
	return itl->first;
}

void
Workplane::refresh_gobj (GObj * gobj, bool redraw){
        {
          Glib::Mutex::Lock lock(draw_mutex);
          stop_drawing=true;
          tile_cache[gobj]->clear();
          gobj->refresh();
        }
        if (redraw) signal_refresh.emit();
}

void
Workplane::set_gobj_active (GObj * gobj, bool active) {
	if (find_gobj(gobj) == layers.end()) {
	    std::cout << "No such gobj " << gobj << std::endl;
	    assert(0);
	    return;
	}
	layers_active[gobj] = active;
}

bool Workplane::get_gobj_active (GObj * gobj) {
	if (find_gobj(gobj) == layers.end()) {
	    std::cout << "No such gobj " << gobj << std::endl;
	    assert(0);
	    return false;
	}
	return layers_active[gobj];
}

bool Workplane::exists(GObj * gobj) {
  return find_gobj(gobj) != layers.end();
}

void
Workplane::refresh(){
  Glib::Mutex::Lock lock(draw_mutex);
  stop_drawing=true;
  std::multimap<int, GObj *>::iterator itl;
  for (itl = layers.begin(); itl != layers.end(); ++itl) {
    itl->second->refresh();
    tile_cache[itl->second]->clear();
  }
}

inline void
Workplane::clear_tile_cache() {
  std::multimap<int, GObj *>::iterator itl;
  for ( itl = layers.begin(); itl != layers.end(); ++itl)
    tile_cache[itl->second].reset(new LayerCache(CacheCapacity));
}

void
Workplane::set_cnv(Conv * c, int hint) {
  cnv=c;
  cnv_hint=hint;
  {
    Glib::Mutex::Lock lock(draw_mutex);
    stop_drawing=true;

    for (std::multimap<int, GObj *>::iterator itl = layers.begin();
      itl != layers.end(); ++itl) {
      itl->second->set_cnv(cnv, cnv_hint);
      tile_cache[itl->second]->clear();
      stop_drawing=true;
      itl->second->refresh();
    }
  }
  signal_refresh.emit();
}
