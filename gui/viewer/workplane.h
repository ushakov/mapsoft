#ifndef WORKPLANE_H
#define WORKPLANE_H

#include <sigc++/sigc++.h>
#include <boost/shared_ptr.hpp>

#include <lib2d/image.h>
#include <utils/cache.h>
#include "layers/layer.h"


class Workplane {
    static const int CacheCapacity = 200;
public:

    sigc::signal<void> signal_refresh;

    Workplane (int _tile_size=256);

    Image<int>  get_image(Point<int> tile_key);

    std::multimap<int, Layer *>::iterator find_layer (Layer * layer);

    void add_layer (Layer * layer, int depth);

    void remove_layer (Layer * layer);

    void set_layer_depth (Layer * layer, int newdepth);

    int get_layer_depth (Layer * layer);

    void refresh_layer (Layer * layer);

    void set_layer_active (Layer * layer, bool active);

    bool get_layer_active (Layer * layer);

    Workplane & operator/= (double k);
    Workplane & operator*= (double k);
    Workplane & operator-= (Point<double> k);
    Workplane & operator+= (Point<double> k);

    void set_tile_size(int s);
    int get_tile_size();

    inline void clear_tile_cache();

private:
    std::multimap <int, Layer *> layers;
    std::map <Layer *, bool> layers_active;
    int tile_size;

    typedef Cache<Point<int>,Image<int> > LayerCache;
    std::map<Layer *, boost::shared_ptr<LayerCache> > tile_cache;
};


#endif /* WORKPLANE_H */
