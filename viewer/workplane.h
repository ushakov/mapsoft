#ifndef WORKPLANE_H
#define WORKPLANE_H

#include <sigc++/sigc++.h>
#include <image.h>
#include <point.h>
#include <rect.h>
#include "layer.h"

class Workplane {
public:
    Workplane (int _tile_size=256, int _scale_nom=1, int _scale_denom=1):
	tile_size(_tile_size),
	scale_nom(_scale_nom),
	scale_denom(_scale_denom)
     {}
    Image<int>  get_image(Point<int> tile_key){

	// сделаем image
        Image<int>  image(tile_size,tile_size, 0xFF000000);
	Rect<int> dst_rect = image.range();
	Rect<int> src_rect = (tile_key*tile_size + dst_rect)*scale_denom;
        src_rect = tiles_on_rect(src_rect, scale_nom);

//	std::cerr << "Workplane: " << src_rect << " -> " << dst_rect << "\n";

	for (std::multimap<int, Layer *>::reverse_iterator itl = layers.rbegin();
	     itl != layers.rend();  ++itl){
	    std::cout << "WP: layer " << itl->second << std::endl;
	    if (layers_active[itl->second]) {
		std::cout << "WP: layer selected " << itl->second << std::endl;
		itl->second->draw (src_rect, image, image.range());
	    }
	}

	return image;
    }

    std::multimap<int, Layer *>::iterator find_layer (Layer * layer) {
	for (std::multimap<int, Layer *>::iterator itl = layers.begin();
	     itl != layers.end(); ++itl)
	{
	    if (itl->second == layer)
	    {
		return itl;
	    }
	}
	return layers.end();
    }

    void
    add_layer (Layer * layer, int depth)
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
    }

    void
    remove_layer (Layer * layer)
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
    }

    void
    set_layer_depth (Layer * layer, int newdepth)
    {
	std::cout << "Setting depth of layer " << layer << " to " << newdepth << std::endl;
	if (find_layer(layer) == layers.end()) {
	    std::cout << "No such layer " << layer << std::endl;
	    assert(0);
	    return;
	}
	remove_layer (layer);
	add_layer (layer, newdepth);
    }

    int
    get_layer_depth (Layer * layer)
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
    
    int get_scale_nom () {
	return scale_nom;
    }
    int get_scale_denom () {
	return scale_denom;
    }

    int set_scale_nom (int s) {
       scale_nom=s;
    }
    int set_scale_denom (int s) {
       scale_denom=s;
    }

    void set_scale (int _scale_nom, int _scale_denom) {
	scale_nom = _scale_nom;
	scale_denom = _scale_denom;
	for (std::multimap<int, Layer *>::reverse_iterator itl = layers.rbegin();
	     itl != layers.rend();
	     ++itl)
	{
	    Layer * l = itl->second;
//	    l->set_scale (scale);
//	    itl->second->set_scale (scale);
	}
    }

  

    void set_tile_size(int s){
	tile_size=s;
    }

    int get_tile_size(){return tile_size;}


private:
    std::multimap <int, Layer *> layers;
    std::map <Layer *, bool> layers_active;
    int scale_nom, scale_denom;
    int tile_size;
};


#endif /* WORKPLANE_H */
