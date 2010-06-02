#ifndef WORKPLANE_H
#define WORKPLANE_H

#include <sigc++/sigc++.h>
#include <boost/shared_ptr.hpp>

#include "lib2d/image.h"
#include "utils/cache.h"
#include "layers/layer.h"

#include "iface/gobj.h"


class Workplane : public GObj {
    static const int CacheCapacity = 200;
public:

    sigc::signal<void> signal_refresh;

    Workplane();

    // functions for gred/gobj interface
    iImage  draw(const iRect & tile);
    iRect   range(void);

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
    Workplane & operator-= (dPoint k);
    Workplane & operator+= (dPoint k);

    inline void clear_tile_cache();

private:
    std::multimap <int, Layer *> layers;
    std::map <Layer *, bool> layers_active;

    typedef Cache<iRect,iImage> LayerCache;
    std::map<Layer *, boost::shared_ptr<LayerCache> > tile_cache;
};


#endif /* WORKPLANE_H */
