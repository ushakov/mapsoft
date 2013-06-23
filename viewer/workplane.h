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

    void add_gobj (GObj * gobj, int depth);

    void remove_gobj (GObj * gobj);

    bool exists (GObj * gobj);

    void clear();

    void set_gobj_depth (GObj * gobj, int newdepth);

    int get_gobj_depth (GObj * gobj);

    void refresh_gobj (GObj * gobj, bool redraw = true);

    void set_gobj_active (GObj * gobj, bool active);

    bool get_gobj_active (GObj * gobj);

    void refresh();

    inline void clear_tile_cache();

    void set_cnv(Conv * c, int hint=-1);

private:
    std::multimap<int, GObj *>::iterator find_gobj (GObj * gobj);

    std::multimap <int, GObj *> layers;
    std::map <GObj *, bool> layers_active;

    typedef Cache<iRect,iImage> LayerCache;
    std::map<GObj *, boost::shared_ptr<LayerCache> > tile_cache;

    Glib::Mutex draw_mutex;
    bool stop_drawing;
};


#endif /* WORKPLANE_H */
