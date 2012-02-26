#include <fstream>
#include <string>

#include "layers/layer_geomap.h"
#include "layers/layer_trk.h"
#include "layers/layer_wpt.h"
#include "layers/layer_google.h"
#include "layers/layer_ks.h"
#include "loaders/image_r.h"

#include "geo/geo_convs.h"
#include "geo/geo_refs.h"
#include "2d/line_utils.h"

#include "utils/cairo_wrapper.h"

#include "geo_io/geofig.h"
#include "geo_io/io_oe.h"

using namespace std;

namespace img{

bool write_file (const char* filename, const geo_data & world, Options opt){

  int ks_zoom = opt.get("ks_zoom",     -1);
  int gg_zoom = opt.get("google_zomm", -1);


  // set default dpi according to raster source resolutions
  double rscale=opt.get("rscale", 100000.0);

  if (!opt.exists("dpi")){
    double mpp=-1; // size of raster point, m
    if (world.maps.size()>0){
      for (vector<g_map_list>::const_iterator i = world.maps.begin();
           i!=world.maps.end(); i++){
        if (i->size()>0){
          g_map orig_ref=convs::mymap(*i);
          mpp=convs::map_mpp(orig_ref, Proj("tmerc"));
          break;
        }
      }
    }
    else if (gg_zoom>0){
      mpp = 6380000.0 * 2*M_PI /256.0/(2 << (gg_zoom-1));
    }
    else if (ks_zoom>0){
      double width = 4 * 256*(1<<(ks_zoom-2));
      double deg_per_pt = 180.0/width; // ~188
      mpp = deg_per_pt * M_PI/180 * 6378137.0;
    }
    if (mpp>0){
      opt.put("dpi", rscale/mpp / 100.0 * 2.54);
    }
  }

  bool need_marg=false;
  // set geometry if no --wgs_geom, --wgs_brd, --geom, --nom, --google options
  if (!opt.exists("geom") && !opt.exists("wgs_geom") &&
      !opt.exists("nom") && !opt.exists("google") &&
      !opt.exists("wgs_brd")){
    dRect geom = world.range_geodata();

    if (!geom.empty() && opt.exists("data_marg")) need_marg=true;

    // fallback: map range
    if (geom.empty()) geom=world.range_map();
    opt.put("wgs_geom", geom);
  }

  // create g_map
  g_map ref = mk_ref(opt);
  dRect geom = ref.border.range();
  ref.file=filename;

  if (need_marg){
    geom = rect_pump(geom, opt.get("data_marg", 0.0));
    ref.border=rect2line(geom);
  }

  // is output image not too large
  iPoint max_image = opt.get("max_image", Point<int>(10000,10000));
  cerr << "Image size: " << geom.w << "x" << geom.h << "\n";
  if ((geom.w>max_image.x) || (geom.h>max_image.y)){
     cerr << "Error: image is too large ("
          << geom.w << "x" << geom.h << ") pixels. "
          << "You may change max_image option to pass this test.\n";
    exit(1);
  }

  iImage im(geom.w,geom.h,0x00FFFFFF);

  if (gg_zoom>=0){
    string dir    = opt.get<string>("google_dir");
    bool download = opt.get<bool>("download", false);
    LayerGoogle l(dir, gg_zoom);
    l.set_ref(ref);
    l.set_downloading(download);
    iImage tmp_im = l.get_image(geom);
    if (!tmp_im.empty()) im.render(iPoint(0,0), tmp_im);
  }

  if (ks_zoom>=0){
    string dir    = opt.get<string>("ks_dir");
    bool download = opt.get("download", false);
    LayerKS l(dir, ks_zoom);
    l.set_ref(ref);
    l.set_downloading(download);
    iImage tmp_im = l.get_image(geom);
    if (!tmp_im.empty()) im.render(iPoint(0,0), tmp_im);
  }

  bool draw_borders = opt.get("draw_borders", false);
  for (int i=0; i<world.maps.size(); i++){
    g_map_list d(world.maps[i]);
    LayerGeoMap l(&d);
    if (draw_borders) l.show_brd();
    l.set_ref(ref);
    iImage tmp_im = l.get_image(geom);
    if (!tmp_im.empty()) im.render(iPoint(0,0), tmp_im);
  }

  for (int i=0; i<world.trks.size(); i++){
    g_track d(world.trks[i]);
    LayerTRK l(&d);
    l.set_ref(ref);
    iImage tmp_im = l.get_image(geom);
    if (!tmp_im.empty()) im.render(iPoint(0,0), tmp_im);
  }

  for (int i=0; i<world.wpts.size(); i++){
    g_waypoint_list d(world.wpts[i]);
    LayerWPT l(&d);
    l.set_ref(ref);
    iImage tmp_im = l.get_image(geom);
    if (!tmp_im.empty()) im.render(iPoint(0,0), tmp_im);
  }

  // clear image outside border
  CairoWrapper cr(im);
  cr->set_operator(Cairo::OPERATOR_DEST_ATOP);
  cr->set_color_a(0xFFFFFFFF);
  if (ref.border.size()>0) cr->move_to(ref.border[0]);
  for (dLine::iterator i = ref.border.begin(); i!=ref.border.end(); i++)
    cr->line_to(*i);
  cr->close_path();
  cr->fill();

  image_r::save(im, filename, opt);

  // writing html map if needed
  string htmfile = opt.get<string>("htm");
  if (htmfile != ""){
    ofstream f(htmfile.c_str());
    f << "<html><body>\n"
      << "<img border=\"0\" "
      <<      "src=\""    << filename << "\" "
      <<      "width=\""  << geom.w << "\" " 
      <<      "height=\"" << geom.h << "\" " 
      <<      "usemap=\"#m\">\n"
      << "<map name=\"m\">\n";
    for (vector<g_map_list>::const_iterator li=world.maps.begin();
                                            li!=world.maps.end(); li++){
      for (g_map_list::const_iterator i=li->begin(); i!=li->end(); i++){
        convs::map2map cnv(*i, ref);
        dLine brd=cnv.line_frw(i->border);
        f << "<area shape=\"poly\" " 
          <<       "href=\""   << i->file << "\" "
          <<       "alt=\""    << i->comm << "\" "
          <<       "title=\""  << i->comm << "\" "
          <<       "coords=\"" << iLine(brd) << "\">\n";
      }
    }
    f << "</map>\n"
      << "</body></html>";
    f.close();
  }

  // writing fig if needed
  string figfile = opt.get<string>("fig");
  if (figfile != ""){
    fig::fig_world W;
    double dpi=opt.get<double>("dpi");
    g_map fig_ref= ref * 2.54 / dpi * fig::cm2fig;
    fig::set_ref(W, fig_ref, Options());

    fig::fig_object o = fig::make_object("2 5 0 1 0 -1 500 -1 -1 0.000 0 0 -1 0 0 *");

    for (g_map::iterator i=fig_ref.begin(); i!=fig_ref.end(); i++){
      o.push_back(iPoint(int(i->xr), int(i->yr)));
    }
    o.push_back(iPoint(int(fig_ref[0].xr), int(fig_ref[0].yr)));

    o.image_file = filename;
    o.comment.push_back(string("MAP ") + filename);
    W.push_back(o);

    fig::write(figfile, W);
  }

  // writing map if needed
  string mapfile = opt.get<string>("map");
  if (mapfile != ""){
    ofstream f(mapfile.c_str());
    oe::write_map_file(f, ref, Options());
  }
}
} //namespace
