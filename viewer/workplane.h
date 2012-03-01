#ifndef WORKPLANE_H
#define WORKPLANE_H

#include <glibmm.h>
#include <sigc++/sigc++.h>
#include <boost/shared_ptr.hpp>

#include "2d/image.h"
#include "2d/cache.h"
#include "layers/layer_geo.h"

#include "gred/iface/gobj.h"


class Workplane : public GObj {
    static const int CacheCapacity = 200;
public:

    sigc::signal<void> signal_refresh;

    Workplane();

    // functions for gred/gobj interface
    int draw(iImage &img, const iPoint &origin);

    std::multimap<int, LayerGeo *>::iterator find_layer (LayerGeo * layer);

    void add_layer (LayerGeo * layer, int depth);

    void remove_layer (LayerGeo * layer);

    void clear();

    void set_layer_depth (LayerGeo * layer, int newdepth);

    int get_layer_depth (LayerGeo * layer);

    void refresh_layer (LayerGeo * layer);

    void set_layer_active (LayerGeo * layer, bool active);

    bool get_layer_active (LayerGeo * layer);

    void set_scale(const double k);

    double get_scale() const;

    inline void clear_tile_cache();

    void set_ref(const g_map & reference);

private:
    std::multimap <int, LayerGeo *> layers;
    std::map <LayerGeo *, bool> layers_active;

    typedef Cache<iRect,iImage> LayerCache;
    std::map<LayerGeo *, boost::shared_ptr<LayerCache> > tile_cache;
    double sc;

    Glib::Mutex draw_mutex;
    bool stop_drawing;
};


#endif /* WORKPLANE_H */
