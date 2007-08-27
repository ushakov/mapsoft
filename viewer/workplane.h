#ifndef WORKPLANE_H
#define WORKPLANE_H

#include <sigc++/sigc++.h>
#include <assert.h>

#include <utils/image.h>
#include <utils/point.h>
#include <utils/rect.h>
#include <utils/cache.h>
#include "layers/layer.h"


class Workplane {
    static const int CacheCapacity = 200;
public:

    sigc::signal<void> signal_refresh;

    Workplane (int _tile_size=256): tile_size(_tile_size) { }

    Image<int>  get_image(Point<int> tile_key){
	Image<int> image(tile_size,tile_size, 0xff000000); // черная картинка-основа
	Rect<int> src_rect = image.range() + tile_key*tile_size;

//	std::cerr << "WP: drawing " << src_rect << "\n";
	for (std::multimap<int, Layer *>::reverse_iterator itl = layers.rbegin(); itl != layers.rend();  ++itl){
	    Layer * layer = itl->second;
	    if (layers_active[layer]) {
		if (!tile_cache[layer]->contains(tile_key)) {
		    tile_cache[layer]->add(tile_key, layer->get_image(src_rect));
		}
		image.render(0,0,tile_cache[layer]->get(tile_key));
	    }
	}
	return image;
    }

    std::multimap<int, Layer *>::iterator find_layer (Layer * layer) {
	for (std::multimap<int, Layer *>::iterator itl = layers.begin();
	     itl != layers.end(); ++itl) {
	    if (itl->second == layer) { return itl; }
	}
	return layers.end();
    }

    void add_layer (Layer * layer, int depth)
    {
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

    void remove_layer (Layer * layer)
    {
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

    void set_layer_depth (Layer * layer, int newdepth)
    {
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

    int get_layer_depth (Layer * layer)
    {
	std::cout << "Getting depth of layer " << layer << std::endl;
	std::multimap<int, Layer *>::iterator itl = find_layer(layer);	
	if (itl == layers.end()) {
	    std::cout << "No such layer " << layer << std::endl;
	    assert(0);
	    return -1;
	}
	return itl->first;
    }

    void refresh_layer (Layer * layer){
	layer->refresh();
	tile_cache[layer]->clear();
        signal_refresh.emit();
    }

    void set_layer_active (Layer * layer, bool active) {
	if (find_layer(layer) == layers.end()) {
	    std::cout << "No such layer " << layer << std::endl;
	    assert(0);
	    return;
	}
	layers_active[layer] = active;
    }

    bool get_layer_active (Layer * layer) {
	if (find_layer(layer) == layers.end()) {
	    std::cout << "No such layer " << layer << std::endl;
	    assert(0);
	    return false;
	}
	return layers_active[layer];
    }

    Workplane & operator/= (double k){ 
	for (std::multimap<int, Layer *>::iterator itl = layers.begin();
	     itl != layers.end(); ++itl) {
            (*itl->second)/=k;
	    tile_cache[itl->second]->clear();
	}
    }
    Workplane & operator*= (double k){ 
	for (std::multimap<int, Layer *>::iterator itl = layers.begin();
	     itl != layers.end(); ++itl) {
            (*itl->second)*=k;
	    tile_cache[itl->second]->clear();
	}
    }

    Workplane & operator-= (g_point k){ 
	for (std::multimap<int, Layer *>::iterator itl = layers.begin();
	     itl != layers.end(); ++itl) {
            (*itl->second)-=k;
	    tile_cache[itl->second]->clear();
	}
    }
    Workplane & operator+= (g_point k){ 
	for (std::multimap<int, Layer *>::iterator itl = layers.begin();
	     itl != layers.end(); ++itl) {
            (*itl->second)+=k;
	    tile_cache[itl->second]->clear();
	}
    }
 

    void set_tile_size(int s){
	if (tile_size != s) {
	    clear_tile_cache();
	}
	tile_size=s;
    }

    int get_tile_size() {
	return tile_size;
    }

    inline void clear_tile_cache() {
	for (std::multimap<int, Layer *>::iterator itl = layers.begin();
	     itl != layers.end(); ++itl) {
	    tile_cache[itl->second].reset(new LayerCache(CacheCapacity));
	}
    }

private:
    std::multimap <int, Layer *> layers;
    std::map <Layer *, bool> layers_active;
    int tile_size;

    typedef Cache<Point<int>,Image<int> > LayerCache;
    std::map<Layer *, boost::shared_ptr<LayerCache> > tile_cache;
};


#endif /* WORKPLANE_H */
