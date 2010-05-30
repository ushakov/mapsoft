#include <assert.h>

#include "workplane.h"


Workplane::Workplane (int _tile_size): tile_size(_tile_size) { }

iImage  Workplane::get_image(iPoint tile_key){
	iImage image(tile_size,tile_size, 0xff000000); // черная картинка-основа
	iRect src_rect = image.range() + tile_key*tile_size;

//	std::cerr << "WP: drawing " << src_rect << "\n";
	for (std::multimap<int, Layer *>::reverse_iterator itl = layers.rbegin(); itl != layers.rend();  ++itl){
	    Layer * layer = itl->second;
	    if (layers_active[layer]) {
		if (!tile_cache[layer]->contains(tile_key)) {
		    tile_cache[layer]->add(tile_key, layer->get_image(src_rect));
		}
		iImage& tile = tile_cache[layer]->get(tile_key);
		if (tile.w != 0) {
		    image.render(0,0,tile);
		}
	    }
	}
	return image;
}

std::multimap<int, Layer *>::iterator Workplane::find_layer (Layer * layer) {
	for (std::multimap<int, Layer *>::iterator itl = layers.begin();
	     itl != layers.end(); ++itl) {
	    if (itl->second == layer) { return itl; }
	}
	return layers.end();
}

void Workplane::add_layer (Layer * layer, int depth) {
	std::cout << "Adding layer " << layer << " at depth " << depth << std::endl;
	if (find_layer(layer) != layers.end()) {
	    std::cout << "Already have this layer!" << std::endl;
	    set_layer_depth (layer, depth);
	    assert(0);
	    return;
	}
	layers.insert (std::make_pair (depth, layer));
	layers_active[layer] = true;
	tile_cache[layer].reset(new LayerCache(CacheCapacity));
}

void Workplane::remove_layer (Layer * layer){
	std::cout << "Removing layer " << layer << std::endl;
	std::multimap<int, Layer *>::iterator itl = find_layer(layer);	
	if (itl == layers.end()) {
	    std::cout << "No such layer " << layer << std::endl;
	    assert(0);
	    return;
	}
	layers_active.erase(layer);
	layers.erase(itl);
	tile_cache.erase(layer);
}

void Workplane::set_layer_depth (Layer * layer, int newdepth){
	std::cout << "Setting depth of layer " << layer << " to " << newdepth << std::endl;
	std::multimap<int, Layer *>::iterator itl = find_layer(layer);
	if (itl == layers.end()) {
	    std::cout << "No such layer " << layer << std::endl;
	    assert(0);
	    return;
	}
	layers.erase(itl);
	layers.insert(std::make_pair(newdepth, layer));
}

int Workplane::get_layer_depth (Layer * layer){
	std::cout << "Getting depth of layer " << layer << std::endl;
	std::multimap<int, Layer *>::iterator itl = find_layer(layer);	
	if (itl == layers.end()) {
	    std::cout << "No such layer " << layer << std::endl;
	    assert(0);
	    return -1;
	}
	return itl->first;
}

void Workplane::refresh_layer (Layer * layer){
	layer->refresh();
	tile_cache[layer]->clear();
        signal_refresh.emit();
}

void Workplane::set_layer_active (Layer * layer, bool active) {
	if (find_layer(layer) == layers.end()) {
	    std::cout << "No such layer " << layer << std::endl;
	    assert(0);
	    return;
	}
	layers_active[layer] = active;
}

bool Workplane::get_layer_active (Layer * layer) {
	if (find_layer(layer) == layers.end()) {
	    std::cout << "No such layer " << layer << std::endl;
	    assert(0);
	    return false;
	}
	return layers_active[layer];
}

Workplane & Workplane::operator/= (double k){ 
	for (std::multimap<int, Layer *>::iterator itl = layers.begin();
	     itl != layers.end(); ++itl) {
            (*itl->second)/=k;
	    tile_cache[itl->second]->clear();
	}
}
Workplane & Workplane::operator*= (double k){ 
	for (std::multimap<int, Layer *>::iterator itl = layers.begin();
	     itl != layers.end(); ++itl) {
            (*itl->second)*=k;
	    tile_cache[itl->second]->clear();
	}
}
Workplane & Workplane::operator-= (dPoint k){ 
	for (std::multimap<int, Layer *>::iterator itl = layers.begin();
	     itl != layers.end(); ++itl) {
            (*itl->second)-=k;
	    tile_cache[itl->second]->clear();
	}
}
Workplane & Workplane::operator+= (dPoint k){ 
	for (std::multimap<int, Layer *>::iterator itl = layers.begin();
	     itl != layers.end(); ++itl) {
            (*itl->second)+=k;
	    tile_cache[itl->second]->clear();
	}
}

void Workplane::set_tile_size(int s){
	if (tile_size != s) {
	    clear_tile_cache();
	}
	tile_size=s;
}

int Workplane::get_tile_size() const{
	return tile_size;
}

inline void Workplane::clear_tile_cache() {
	for (std::multimap<int, Layer *>::iterator itl = layers.begin();
	     itl != layers.end(); ++itl) {
	    tile_cache[itl->second].reset(new LayerCache(CacheCapacity));
	}
}

