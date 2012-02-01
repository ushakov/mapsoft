#include <assert.h>
#include "workplane.h"


Workplane::Workplane(void) : sc(1.0) { }

int
Workplane::draw(iImage &img, const iPoint &origin){

        draw_mutex.lock();
        iRect tile(origin.x, origin.y, img.w, img.h );
	for (std::multimap<int, LayerGeo *>::reverse_iterator
                itl = layers.rbegin(); itl != layers.rend();  ++itl){
	    LayerGeo * layer = itl->second;
	    if (layers_active[layer]) {
		if (!tile_cache[layer]->contains(tile)) {
		    tile_cache[layer]->add(tile, layer->get_image(tile));
		}
		iImage& tile_img = tile_cache[layer]->get(tile);
                draw_mutex.unlock();
		if (tile_img.w != 0) img.render(0,0,tile_img);
                draw_mutex.lock();
	    }
	}
        draw_mutex.unlock();
        // кажется, что когда будет нечто, использующее код возврата,
        // workplane можно будет им заменить! Так что не смотрим, нарисовали ли мы что-нибудь.
	return GOBJ_FILL_PART;
}

std::multimap<int, LayerGeo *>::iterator
Workplane::find_layer (LayerGeo * layer) {
	for (std::multimap<int, LayerGeo *>::iterator itl = layers.begin();
	     itl != layers.end(); ++itl) {
	    if (itl->second == layer) { return itl; }
	}
	return layers.end();
}

void
Workplane::add_layer (LayerGeo * layer, int depth) {
//      std::cout << "Adding layer " << layer << " at depth " << depth << std::endl;
	if (find_layer(layer) != layers.end()) {
	    std::cout << "Already have this layer!" << std::endl;
	    set_layer_depth (layer, depth);
	    assert(0);
	    return;
	}
        draw_mutex.lock();
	layers.insert (std::make_pair (depth, layer));
	layers_active[layer] = true;
	tile_cache[layer].reset(new LayerCache(CacheCapacity));
        draw_mutex.unlock();
}

void
Workplane::remove_layer (LayerGeo * layer){
//	std::cout << "Removing layer " << layer << std::endl;
	std::multimap<int, LayerGeo *>::iterator itl = find_layer(layer);	
	if (itl == layers.end()) {
	    std::cout << "No such layer " << layer << std::endl;
	    assert(0);
	    return;
	}
        draw_mutex.lock();
	layers_active.erase(layer);
	layers.erase(itl);
	tile_cache.erase(layer);
        draw_mutex.unlock();
}

void
Workplane::clear(){
        draw_mutex.lock();
	layers_active.clear();
	layers.clear();
	tile_cache.clear();
        draw_mutex.unlock();
}

void
Workplane::set_layer_depth (LayerGeo * layer, int newdepth){
	std::multimap<int, LayerGeo *>::iterator itl = find_layer(layer);
	if (itl == layers.end()) {
	    std::cout << "No such layer " << layer << std::endl;
	    assert(0);
	    return;
	}
        draw_mutex.lock();
	layers.erase(itl);
	layers.insert(std::make_pair(newdepth, layer));
        draw_mutex.unlock();
}

int
Workplane::get_layer_depth (LayerGeo * layer){
	std::multimap<int, LayerGeo *>::iterator itl = find_layer(layer);	
	if (itl == layers.end()) {
	    std::cout << "No such layer " << layer << std::endl;
	    assert(0);
	    return -1;
	}
	return itl->first;
}

void
Workplane::refresh_layer (LayerGeo * layer){
	layer->refresh();
	tile_cache[layer]->clear();
        signal_refresh.emit();
}

void
Workplane::set_layer_active (LayerGeo * layer, bool active) {
	if (find_layer(layer) == layers.end()) {
	    std::cout << "No such layer " << layer << std::endl;
	    assert(0);
	    return;
	}
	layers_active[layer] = active;
}

bool Workplane::get_layer_active (LayerGeo * layer) {
	if (find_layer(layer) == layers.end()) {
	    std::cout << "No such layer " << layer << std::endl;
	    assert(0);
	    return false;
	}
	return layers_active[layer];
}

void Workplane::set_scale(const double k){
        draw_mutex.lock();
	for (std::multimap<int, LayerGeo *>::iterator itl = layers.begin();
    					itl != layers.end(); ++itl) {
		(*itl->second)*=k/sc;
		tile_cache[itl->second]->clear();
	}
	sc=k;
        draw_mutex.unlock();
}

inline void
Workplane::clear_tile_cache() {
	for (std::multimap<int, LayerGeo *>::iterator itl = layers.begin();
	     itl != layers.end(); ++itl) {
	    tile_cache[itl->second].reset(new LayerCache(CacheCapacity));
	}
}

void
Workplane::set_ref(const g_map & reference) {
  draw_mutex.lock();
  for (std::multimap<int, LayerGeo *>::iterator itl = layers.begin();
    itl != layers.end(); ++itl) {
    itl->second->set_ref(reference);
    refresh_layer(itl->second);
  }
  draw_mutex.unlock();
}

