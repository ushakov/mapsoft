#ifndef WORKPLANE_H
#define WORKPLANE_H

#include <glibmm.h>
#include <sigc++/sigc++.h>
#include <boost/shared_ptr.hpp>

#include "2d/image.h"
#include "2d/cache.h"

#include "gred/gobj.h"


class Workplane : public GObj {
    static const int CacheCapacity = 200;
public:

    sigc::signal<void> signal_refresh;

    Workplane();

    // functions for gred/gobj interface
    int draw(iImage &img, const iPoint &origin);

    void add_layer (GObj * layer, int depth);

    void remove_layer (GObj * layer);

    bool exists (GObj * layer);

    void clear();

    void set_layer_depth (GObj * layer, int newdepth);

    int get_layer_depth (GObj * layer);

    void refresh_layer (GObj * layer, bool redraw = true);

    void set_layer_active (GObj * layer, bool active);

    bool get_layer_active (GObj * layer);

    void rescale(double k);

    inline void clear_tile_cache();

    void set_cnv(Conv * cnv, int hint=-1);

    Conv * get_cnv() const;

private:
    std::multimap<int, GObj *>::iterator find_layer (GObj * layer);

    std::multimap <int, GObj *> layers;
    std::map <GObj *, bool> layers_active;

    typedef Cache<iRect,iImage> LayerCache;
    std::map<GObj *, boost::shared_ptr<LayerCache> > tile_cache;

    Glib::Mutex draw_mutex;
    bool stop_drawing;

    Conv * cnv;
    int cnv_hint;
};


#endif /* WORKPLANE_H */
