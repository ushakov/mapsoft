#ifndef WORKPLANE_H
#define WORKPLANE_H

#include <glibmm.h>
#include <sigc++/sigc++.h>
#include <boost/shared_ptr.hpp>

#include "2d/image.h"
#include "2d/cache.h"
#include "layers/layer.h"

#include "gred/gobj.h"


class Workplane : public GObj {
    static const int CacheCapacity = 200;
public:

    sigc::signal<void> signal_refresh;

    Workplane();

    // functions for gred/gobj interface
    int draw(iImage &img, const iPoint &origin);

    void add_layer (Layer * layer, int depth);

    void remove_layer (Layer * layer);

    bool exists (Layer * layer);

    void clear();

    void set_layer_depth (Layer * layer, int newdepth);

    int get_layer_depth (Layer * layer);

    void refresh_layer (Layer * layer, bool redraw = true);

    void set_layer_active (Layer * layer, bool active);

    bool get_layer_active (Layer * layer);

    void set_scale(const double k);

    double get_scale() const;

    inline void clear_tile_cache();

    void set_cnv(Conv * cnv, int hint=-1);

    Conv * get_cnv() const;

private:
    std::multimap<int, Layer *>::iterator find_layer (Layer * layer);

    std::multimap <int, Layer *> layers;
    std::map <Layer *, bool> layers_active;

    typedef Cache<iRect,iImage> LayerCache;
    std::map<Layer *, boost::shared_ptr<LayerCache> > tile_cache;
    double sc;

    Glib::Mutex draw_mutex;
    bool stop_drawing;
    Conv * cnv;
    int cnv_hint;
};


#endif /* WORKPLANE_H */
