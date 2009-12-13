#include "am_refmap.h"
#include "../generic_dialog.h"

using namespace std;

void RefMap::activate() {
    refpoints.clear();
    working = false;
    Options opt;
    opt.put("filename", "");
    state->gend.activate(get_name(), opt,
                         sigc::mem_fun(this, &RefMap::on_result));
}

void RefMap::on_result(int r, const Options & o) {
    if (r == 0) {  // OK
        map_layer = NULL;
        for (int i=0; i<state->map_layers.size(); i++){
            LayerGeoMap *current_layer =
                dynamic_cast<LayerGeoMap *> (state->map_layers[i].get());
            if (state->viewer.workplane.get_layer_active(current_layer)) {
                map_layer = current_layer;
                break;
            }
        }
        if (map_layer == NULL) return;
        g_map ref = map_layer->get_ref();
        convs::map2pt screen2ll(ref,
                                Datum("WGS84"), Proj("lonlat"), Options());

        // Make screen rect in latlon
        iPoint origin = state->viewer.get_origin();
        iPoint winsize = state->viewer.get_window_size();
        dRect screen_geo(origin, origin + winsize);
        screen_geo = screen2ll.bb_frw(screen_geo);
        cerr << "screen_geo " << screen_geo << endl;
	
        // Make initial (empty) reference
        g_map newref;
        newref.file = o.get("filename", string());
        newref.map_proj = ref.map_proj;
        mymap.reset(new geo_data);
        mymap->maps.push_back(newref);

        size = image_r::size(newref.file.c_str());
        dPoint screen_width_geo = screen_geo.TRC() - screen_geo.TLC();
        cerr << "size " << size << endl;
        default_dpp = screen_width_geo.x / size.x;
        default_origin = screen_geo.BLC();

        // Make the default reference (from 0 refpoints)
        remake_ref();

        // Add the map
        layer.reset(new LayerGeoMap(mymap.get()));
        layer->set_ref(ref);
        layer->set_alpha(0x80);
        state->data.push_back(mymap);
        state->map_layers.push_back(layer);
        state->add_layer(layer.get(), 250, "ref: " + mymap->maps[0].file);
        current_conv.reset(
            new convs::map2pt(mymap->maps[0], Datum("WGS84"),
                              Proj("lonlat"), Options()));

        // phew, done! now the real action begins!
        context = state->statusbar.get_context_id("am_refmap");
        state->statusbar.push("Refmap: pick a point in picture", context);
        refpoints.clear();
        last_refpoint_partial = false;
        state->viewer.refresh();
        working = true;
    }
}

void RefMap::handle_click(iPoint p) {
    if (!working) return;
    
    convs::map2pt screen2ll(map_layer->get_ref(),
                            Datum("WGS84"), Proj("lonlat"), Options());
    dPoint geo = p;
    screen2ll.frw(geo);

    if (!last_refpoint_partial) {
        // we should start a new refpoint; point clicked is in the
        // picture layer
        dPoint pictpoint = geo;
        current_conv->bck(pictpoint);
    
        refpoints.push_back(g_refpoint());
        refpoints[refpoints.size()-1].xr = pictpoint.x;
        refpoints[refpoints.size()-1].yr = pictpoint.y;
        last_refpoint_partial = true;
        state->statusbar.pop(context);
        state->statusbar.push("Refmap: pick a point on map", context);
    } else {
        // This click completes the refpoint. Add it and recompute the
        // reference.
        refpoints.back().x = geo.x;
        refpoints.back().y = geo.y;
        last_refpoint_partial = false;
        state->statusbar.pop(context);
        state->statusbar.push("Refmap: pick a point in picture", context);
        remake_ref();
        // Redo the referecing in the layer and refresh the view
        layer->set_ref(layer->get_ref());
        state->viewer.refresh();
    }
}

void RefMap::abort() {}


void RefMap::remake_ref() {
    g_map newref;
    newref = mymap->maps[0];

    newref.clear();
    for (int i = 0; i < refpoints.size(); ++i) {
        newref.push_back(refpoints[i]);
    }
    if (refpoints.size() == 0) {
        newref.push_back(g_refpoint(default_origin, iPoint(0,0)));
        newref.push_back(g_refpoint(default_origin - dPoint(0, size.y)*default_dpp, iPoint(0,size.y)));
        newref.push_back(g_refpoint(default_origin + dPoint(size.x, 0)*default_dpp, iPoint(size.x,0)));
    } else if (refpoints.size() == 1) {
        dPoint p0(refpoints[0].x, refpoints[0].y);
        dPoint pr0(refpoints[0].xr, refpoints[0].yr);
        dPoint p1, p2;
        dPoint pr1, pr2;
        if (pr0.x < size.x/2) {
            pr1 = iPoint(size.x, 0);
            pr2 = size;
        } else {
            pr1 = iPoint(0, 0);
            pr2 = iPoint(0, size.y);
        }
        dPoint d1 = pr1 - pr0;
        d1.y = -d1.y;
        dPoint d2 = pr2 - pr0;
        d2.y = -d2.y;
        p1 = p0 + default_dpp * d1; 
        p2 = p0 + default_dpp * d2;
        
        newref.push_back(refpoints[0]);
        newref.push_back(g_refpoint(p1, pr1));
        newref.push_back(g_refpoint(p2, pr2));
    } else if (refpoints.size() == 2) {
        dPoint p0(refpoints[0].x, refpoints[0].y);
        dPoint pr0(refpoints[0].xr, refpoints[0].yr);
        dPoint p1(refpoints[1].x, refpoints[1].y);
        dPoint pr1(refpoints[1].xr, refpoints[1].yr);

        dPoint p2;
        dPoint pr2;

        dPoint dr = pr1 - pr0;
        dPoint drn(dr.y, -dr.x);
        pr2 = pr0 + drn;

        p2 = p0 + default_dpp * (pr2 - pr0);

        newref.push_back(refpoints[0]);
        newref.push_back(refpoints[1]);
        newref.push_back(g_refpoint(p2, pr2));
    }
    
    mymap->maps[0] = newref;
    current_conv.reset(
        new convs::map2pt(newref, Datum("WGS84"), Proj("lonlat"), Options()));
}
