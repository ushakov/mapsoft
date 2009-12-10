#include "am_refmap.h"
#include "../generic_dialog.h"

void RefMap::activate() {
  Options opt;
  opt.put("filename", "");
  state->gend.activate(get_name(), opt,
		       sigc::mem_fun(this, &RefMap::on_result));
}

void RefMap::on_result(int r, const Options & o) {
  if (r == 0) {  // OK
    LayerGeoMap *current_layer = NULL;
    for (int i=0; i<state->map_layers.size(); i++){
      LayerGeoMap *layer =
	dynamic_cast<LayerGeoMap *> (state->map_layers[i].get());
      if (state->viewer.workplane.get_layer_active(layer)) {
	current_layer = layer;
	break;
      }
    }
    if (current_layer == NULL) return;
    g_map ref = current_layer->get_ref();
    convs::map2pt screen2ll(ref,
			    Datum("WGS84"), Proj("lonlat"), Options());

    // Make screen rect in latlon
    iPoint origin = state->viewer.get_origin();
    iPoint winsize = state->viewer.get_window_size();
    dRect screen_geo(origin, origin + winsize);
    screen_geo = screen2ll.bb_frw(screen_geo);
	
    // Make initial reference
    g_map newref;
    newref.file = o.get("filename", string());
    iPoint size = image_r::size(newref.file.c_str());
    newref.proj = ref.proj;
    newref.push_back(g_refpoint(screen_geo.TLC(), iPoint(0,0)));
    newref.push_back(g_refpoint(screen_geo.TRC(), iPoint(size.x,0)));
    newref.push_back(g_refpoint(screen_geo.BLC(), iPoint(0,size.y)));
    newref.push_back(g_refpoint(screen_geo.BRC(), size));

    mymap.clear();
    mymap.maps.push_back(newref);
    // add the map and prompt user
    layer.reset(LayerGeoMap(&mymap));
    state->map_layers.push_back(layer);
    state->add_layer(layer.get(), 250, "ref: " + newref.file);
  }
}
