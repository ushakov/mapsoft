#include "am_refmap.h"
#include "../generic_dialog.h"

using namespace std;

void RefMap::activate() {
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
    if (current_layer == NULL) return;
    g_map ref = current_layer->get_ref();
    convs::map2pt screen2ll(ref,
			    Datum("WGS84"), Proj("lonlat"), Options());

    // Make screen rect in latlon
    iPoint origin = state->viewer.get_origin();
    iPoint winsize = state->viewer.get_window_size();
    dRect screen_geo(origin, origin + winsize);
    screen_geo = screen2ll.bb_frw(screen_geo);
    cerr << "screen_geo " << screen_geo << endl;
	
    // Make initial reference
    g_map newref;
    newref.file = o.get("filename", string());
    iPoint size = image_r::size(newref.file.c_str());
    newref.map_proj = ref.map_proj;
    newref.push_back(g_refpoint(screen_geo.BLC(), iPoint(0,0)));
    newref.push_back(g_refpoint(screen_geo.BRC(), iPoint(size.x,0)));
    newref.push_back(g_refpoint(screen_geo.TLC(), iPoint(0,size.y)));
    newref.push_back(g_refpoint(screen_geo.TRC(), size));
    newref.ensure_border();
    cerr << "size " << size << endl;

    mymap.reset(new geo_data);
    mymap->maps.push_back(newref);
    // add the map
    layer.reset(new LayerGeoMap(mymap.get()));
    layer->set_ref(ref);
    state->data.push_back(mymap);
    state->map_layers.push_back(layer);
    state->add_layer(layer.get(), 350, "ref: " + newref.file);
    current_conv = map2pt(newref,
			  Datum("WGS84"), Proj("lonlat"), Options())

    // phew, done! now the real action begins!
    context = state->statusbar.get_context_id("am_refmap");
    state->statusbar.push(context, "Refmap: pick a point in picture");
    refpoints.clear();
    last_refpoint_partial = false;
  }
}

void RefMap::handle_click(iPoint p) {
  if (!last_refpoint_partial) {
    // we should start a new refpoint; point clicked is in the picture layer
    convs::map2pt screen2ll(map_layer->get_ref(),
			    Datum("WGS84"), Proj("lonlat"), Options());
    dPoint geo = p;
    screen2ll.frw(geo);

    iPoint pictpoint = geo;
    current_conv.bck(pictpoint);
    
    refpoints.push_back(g_refpoint());
    refpoints[refpoints.size()-1].xr = pictpoint.x;
    refpoints[refpoints.size()-1].yr = pictpoint.y;
    last_refpoint_partial = true;
    state->statusbar.pop(context);
  } else {
  }
}

void RefMap::abort() {}

