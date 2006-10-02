#ifndef WORKPLANE_H
#define WORKPLANE_H

//#include <image-gdk/image.h>
#include <sigc++/sigc++.h>
#include <image.h>
#include <point.h>
#include <rect.h>

// a plane that shows workspace: maps, tracks, waypoints
// plane has integer cartesian coordinate system; the correspondance of
// these coordinates to geodesic coordinates is left to lower layers
// (and controller)
// these lower layers:
//  * draw maps on the plane
//  * put waypoints on plane
//  * draw tracks on the plane
//  * etc.

class Workplane {
public:
    Workplane (int tile_size_=256, double scale_=1.0):
	tile_size(tile_size_),
	scale(scale_)
     {}
    Image<int>  get_image(Point<int> tile_key){

	// сделаем image
        Image<int>  image(tile_size,tile_size);

	//подождем чуть
	sleep(1);

	int x1 = tile_key.x*tile_size;
	int y1 = tile_key.y*tile_size;

        for (int j=0; j<tile_size; j++){
          for (int i=0; i<tile_size; i++){
	    if (((i+x1)%50==0)||((j+y1)%30==0)) image.set(i,j,0xFFFFFFFF);
	    else image.set(i,j,0xFF000000);
          }
        }

	// отправим окончательный результат!
	return image;

/*
	for (std::multimap<int, Layer *>::reverse_iterator itl = layers.rbegin();
	     itl != layers.rend();
	     ++itl)
	{
	    itl->second->draw (img, x, y);
	}
*/
    }

///////////////////
/*    void
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

    double
    get_scale () {
	return scale;
    }

    void set_scale (double _scale) {
	scale = _scale;
	for (std::multimap<int, Layer *>::reverse_iterator itl = layers.rbegin();
	     itl != layers.rend();
	     ++itl)
	{
	    Layer * l = itl->second;
	    l->set_scale (scale);
//	    itl->second->set_scale (scale);
	}
    }

  */

    void set_tile_size(int s){
	tile_size=s;
    }
    int get_tile_size(){return tile_size;}

    void set_scale(double s){
	scale=s;
    }
    double get_scale(){return scale;}


private:
//    std::multimap <int, Layer *> layers;
    double scale;
    int tile_size;
};


#endif /* WORKPLANE_H */
