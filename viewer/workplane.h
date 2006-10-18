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
        src_rect = rect_intdiv(src_rect, scale_nom);
//	std::cerr << "Workplane: " << src_rect << " -> " << dst_rect << "\n";

	for (std::multimap<int, Layer *>::reverse_iterator itl = layers.rbegin();
	     itl != layers.rend();  ++itl){
	     itl->second->draw (image, src_rect);
	}

	return image;
    }

    void
    add_layer (Layer * layer, int depth)
    {
	layers.insert (std::make_pair (depth, layer));
    }

    void
    remove_layer (Layer * layer)
    {
	for (std::multimap<int, Layer *>::iterator itl = layers.begin();
	     itl != layers.end();)
	{
	    if (itl->second == layer)
	    {
		std::multimap<int, Layer *>::iterator tmp = itl++;
		layers.erase (itl);
		itl = tmp;
	    }
	    else
	    {
		++itl;
	    }
	}
    }

    void
    change_depth (Layer * layer, int newdepth)
    {
	remove_layer (layer);
	add_layer (layer, newdepth);
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
    int scale_nom, scale_denom;
    int tile_size;
};


#endif /* WORKPLANE_H */
