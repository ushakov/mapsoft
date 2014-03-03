#include <assert.h>
#include "workplane.h"

int
Workplane::draw(iImage &img, const iPoint &origin){

        draw_mutex.lock();
        stop_drawing=false;
        iRect tile(origin.x, origin.y, img.w, img.h );
	for (std::multimap<int, GObjGeo *>::reverse_iterator
                itl = layers.rbegin(); itl != layers.rend();  ++itl){
	    GObjGeo * obj = itl->second;
	    if (layers_active[obj]) {
                // copy to prevent deleting from another thread
                boost::shared_ptr<LayerCache> cache = tile_cache[obj];
		if (!cache->contains(tile)) {
		    cache->add(tile, obj->get_image(tile));
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

std::multimap<int, GObjGeo *>::iterator
Workplane::find_gobj (GObjGeo * obj) {
	for (std::multimap<int, GObjGeo *>::iterator itl = layers.begin();
	     itl != layers.end(); ++itl) {
	    if (itl->second == obj) { return itl; }
	}
	return layers.end();
}

void
Workplane::add_gobj (GObjGeo * obj, int depth) {
//      std::cout << "Adding obj " << obj << " at depth " << depth << std::endl;
	if (find_gobj(obj) != layers.end()) {
	    std::cout << "Already have this obj!" << std::endl;
	    set_gobj_depth (obj, depth);
	    assert(0);
	    return;
	}
        Glib::Mutex::Lock lock(draw_mutex);
        stop_drawing=true;
	layers.insert (std::make_pair (depth, obj));
	obj->set_ref(ref);
	layers_active[obj] = true;
	tile_cache[obj].reset(new LayerCache(CacheCapacity));
}

void
Workplane::remove_gobj (GObjGeo * obj){
//	std::cout << "Removing obj " << obj << std::endl;
	std::multimap<int, GObjGeo *>::iterator itl = find_gobj(obj);
	if (itl == layers.end()) {
	    std::cout << "No such obj " << obj << std::endl;
	    assert(0);
	    return;
	}
        Glib::Mutex::Lock lock(draw_mutex);
        stop_drawing=true;
	layers_active.erase(obj);
	layers.erase(itl);
	tile_cache.erase(obj);
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
Workplane::set_gobj_depth (GObjGeo * obj, int newdepth){
	std::multimap<int, GObjGeo *>::iterator itl = find_gobj(obj);
	if (itl == layers.end()) {
	    std::cout << "No such obj " << obj << std::endl;
	    assert(0);
	    return;
	}
        Glib::Mutex::Lock lock(draw_mutex);
        stop_drawing=true;
	layers.erase(itl);
	layers.insert(std::make_pair(newdepth, obj));
}

int
Workplane::get_gobj_depth (GObjGeo * obj){
	std::multimap<int, GObjGeo *>::iterator itl = find_gobj(obj);
	if (itl == layers.end()) {
	    std::cout << "No such obj " << obj << std::endl;
	    assert(0);
	    return -1;
	}
	return itl->first;
}

void
Workplane::refresh_gobj (GObjGeo * obj, bool redraw){
        {
          Glib::Mutex::Lock lock(draw_mutex);
          stop_drawing=true;
          tile_cache[obj]->clear();
          obj->refresh();
        }
        if (redraw) signal_redraw_me().emit(iRect());
}

void
Workplane::set_gobj_active (GObjGeo * obj, bool active) {
	if (find_gobj(obj) == layers.end()) {
	    std::cout << "No such obj " << obj << std::endl;
	    assert(0);
	    return;
	}
	layers_active[obj] = active;
}

bool Workplane::get_gobj_active (GObjGeo * obj) {
	if (find_gobj(obj) == layers.end()) {
	    std::cout << "No such obj " << obj << std::endl;
	    assert(0);
	    return false;
	}
	return layers_active[obj];
}

bool Workplane::exists(GObjGeo * obj) {
  return find_gobj(obj) != layers.end();
}

void
Workplane::refresh(){
  Glib::Mutex::Lock lock(draw_mutex);
  stop_drawing=true;
  std::multimap<int, GObjGeo *>::iterator itl;
  for (itl = layers.begin(); itl != layers.end(); ++itl) {
    itl->second->refresh();
    tile_cache[itl->second]->clear();
  }
}

inline void
Workplane::clear_tile_cache() {
  std::multimap<int, GObjGeo *>::iterator itl;
  for ( itl = layers.begin(); itl != layers.end(); ++itl)
    tile_cache[itl->second].reset(new LayerCache(CacheCapacity));
}

void
Workplane::set_ref(const g_map & ref) {
  GObjGeo::set_ref(ref);
  {
    Glib::Mutex::Lock lock(draw_mutex);
    stop_drawing=true;
    for (std::multimap<int, GObjGeo *>::iterator itl = layers.begin();
      itl != layers.end(); ++itl) {
      stop_drawing=true;
      itl->second->set_ref(ref);
      tile_cache[itl->second]->clear();
    }
  }
  signal_redraw_me().emit(iRect());
}
