#include <assert.h>
#include "workplane.h"


Workplane::Workplane(void) : cnv(NULL),stop_drawing(false){ }

int
Workplane::draw(iImage &img, const iPoint &origin){

        draw_mutex.lock();
        stop_drawing=false;
        iRect tile(origin.x, origin.y, img.w, img.h );
	for (std::multimap<int, GObj *>::reverse_iterator
                itl = layers.rbegin(); itl != layers.rend();  ++itl){
	    GObj * layer = itl->second;
	    if (layers_active[layer]) {
                // copy to prevent deleting from another thread
                boost::shared_ptr<LayerCache> cache = tile_cache[layer];
		if (!cache->contains(tile)) {
		    cache->add(tile, layer->get_image(tile));
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
Workplane::find_layer (GObj * layer) {
	for (std::multimap<int, GObj *>::iterator itl = layers.begin();
	     itl != layers.end(); ++itl) {
	    if (itl->second == layer) { return itl; }
	}
	return layers.end();
}

void
Workplane::add_layer (GObj * layer, int depth) {
//      std::cout << "Adding layer " << layer << " at depth " << depth << std::endl;
	if (find_layer(layer) != layers.end()) {
	    std::cout << "Already have this layer!" << std::endl;
	    set_layer_depth (layer, depth);
	    assert(0);
	    return;
	}
        Glib::Mutex::Lock lock(draw_mutex);
        stop_drawing=true;
	layers.insert (std::make_pair (depth, layer));
	layer->set_cnv(cnv, cnv_hint);
	layers_active[layer] = true;
	tile_cache[layer].reset(new LayerCache(CacheCapacity));
}

void
Workplane::remove_layer (GObj * layer){
//	std::cout << "Removing layer " << layer << std::endl;
	std::multimap<int, GObj *>::iterator itl = find_layer(layer);
	if (itl == layers.end()) {
	    std::cout << "No such layer " << layer << std::endl;
	    assert(0);
	    return;
	}
        Glib::Mutex::Lock lock(draw_mutex);
        stop_drawing=true;
	layers_active.erase(layer);
	layers.erase(itl);
	tile_cache.erase(layer);
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
Workplane::set_layer_depth (GObj * layer, int newdepth){
	std::multimap<int, GObj *>::iterator itl = find_layer(layer);
	if (itl == layers.end()) {
	    std::cout << "No such layer " << layer << std::endl;
	    assert(0);
	    return;
	}
        Glib::Mutex::Lock lock(draw_mutex);
        stop_drawing=true;
	layers.erase(itl);
	layers.insert(std::make_pair(newdepth, layer));
}

int
Workplane::get_layer_depth (GObj * layer){
	std::multimap<int, GObj *>::iterator itl = find_layer(layer);
	if (itl == layers.end()) {
	    std::cout << "No such layer " << layer << std::endl;
	    assert(0);
	    return -1;
	}
	return itl->first;
}

void
Workplane::refresh_layer (GObj * layer, bool redraw){
	layer->refresh();
        tile_cache[layer]->clear();
        if (redraw) signal_refresh.emit();
}

void
Workplane::set_layer_active (GObj * layer, bool active) {
	if (find_layer(layer) == layers.end()) {
	    std::cout << "No such layer " << layer << std::endl;
	    assert(0);
	    return;
	}
	layers_active[layer] = active;
}

bool Workplane::get_layer_active (GObj * layer) {
	if (find_layer(layer) == layers.end()) {
	    std::cout << "No such layer " << layer << std::endl;
	    assert(0);
	    return false;
	}
	return layers_active[layer];
}

bool Workplane::exists(GObj * layer) {
  return find_layer(layer) != layers.end();
}

void
Workplane::refresh(){
  Glib::Mutex::Lock lock(draw_mutex);
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
    for (std::multimap<int, GObj *>::iterator itl = layers.begin();
      itl != layers.end(); ++itl) {
      itl->second->set_cnv(cnv, cnv_hint);
      refresh_layer(itl->second);
    }
  }
  signal_refresh.emit();
}

Conv *
Workplane::get_cnv() const{
  return cnv;
}
