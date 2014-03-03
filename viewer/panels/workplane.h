#ifndef WORKPLANE_H
#define WORKPLANE_H

#include <glibmm.h>
#include <sigc++/sigc++.h>
#include <boost/shared_ptr.hpp>

#include "2d/image.h"
#include "2d/cache.h"

#include "img_io/gobj_geo.h"


class Workplane : public GObjGeo {
    static const int CacheCapacity = 200;
public:

    Workplane(void): stop_drawing(false){ }

    // functions for gred/gobj interface
    int draw(iImage &img, const iPoint &origin);

    void add_gobj (GObjGeo * obj, int depth);

    void remove_gobj (GObjGeo * obj);

    bool exists (GObjGeo * obj);

    void clear();

    void set_gobj_depth (GObjGeo * obj, int newdepth);

    int get_gobj_depth (GObjGeo * obj);

    void refresh_gobj (GObjGeo * obj, bool redraw = true);

    void set_gobj_active (GObjGeo * obj, bool active);

    bool get_gobj_active (GObjGeo * obj);

    void refresh();

    inline void clear_tile_cache();

    void set_ref(const g_map & ref);

private:
    std::multimap<int, GObjGeo *>::iterator find_gobj (GObjGeo * obj);

    std::multimap <int, GObjGeo *> layers;
    std::map <GObjGeo *, bool> layers_active;

    typedef Cache<iRect,iImage> LayerCache;
    std::map<GObjGeo *, boost::shared_ptr<LayerCache> > tile_cache;

    Glib::Mutex draw_mutex;
    bool stop_drawing;
};


#endif /* WORKPLANE_H */
