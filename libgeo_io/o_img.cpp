#include <fstream>
#include <string>

#include "../layers/layer_geomap.h"
#include "../layers/layer_geodata.h"
#include "../layers/layer_google.h"
#include "../layers/layer_ks.h"

#include "../libgeo/geo_convs.h"
#include "../libgeo/geo_refs.h"
#include "../libgeo_io/geofig.h"
#include "../lib2d/line_utils.h"

#include "io_oe.h"

using namespace std;

namespace img{

bool write_file (const char* filename, const geo_data & world, const Options & opt){

  Rect<double> geom;      opt.get("geom", geom);
  Proj  proj("tmerc");    opt.get("proj", proj);
  Datum datum("pulkovo"); opt.get("datum", datum);

  double scale=0, rscale=0, dpi=0, factor=1;
  opt.get("scale",  scale);
  opt.get("rscale", rscale);
  opt.get("dpi",    dpi);
  opt.get("factor", factor);
  if (rscale!=0) scale=1.0/rscale;


  // Conversion to target coordiates
  convs::pt2pt c(Datum("wgs84"), Proj("lonlat"), Options(), datum, proj, opt);

  // setting default geometry from geodata
  if (geom.empty()){
    if (! world.range_geodata().empty()){
      geom=c.bb_frw(world.range_geodata(), world.range_geodata().w/1000);
    } else {
      cerr << "Empty geometry! Use -g option.\n";
      exit(1);
    }
  }

  int ks_zoom=-1; opt.get("ks",     ks_zoom);
  int gg_zoom=-1; opt.get("google", gg_zoom);

  // creating initial map reference with borders
  g_map ref; // unscaled ref
  g_line brd;
  brd.push_back(geom.BLC());
  brd.push_back(geom.BRC());
  brd.push_back(geom.TRC());
  brd.push_back(geom.TLC());
  for (g_line::const_iterator p=brd.begin(); p!=brd.end(); p++){
    g_point pg=*p;
    g_point pr=*p;
    c.bck(pg);
    pr.y=(geom.y+geom.h)-(pr.y-geom.y);
    ref.push_back(g_refpoint(pg, pr));
    ref.border.push_back(pr);
  }
  ref.map_proj=boost::lexical_cast<string>(proj);
  ref.file=filename;


  // calculating rescale factor for reference
  double k=0; // scale for our ref
  if ((scale!=0) && (dpi!=0)) k = scale/2.54e-2*dpi;

  // fallbacks
  if (dpi==0) dpi=200;
  if (scale==0) scale=1e-5;

  if (k==0){
    if (!world.maps.empty()){
      g_map orig_ref=convs::mymap(world);
      k=1.0/convs::map_mpp(orig_ref);
    }
    else if (gg_zoom>0){
      double mpp = 6380000.0 * 2*M_PI /256.0/(2 << (gg_zoom-1));
      k=1.0/mpp;
    }
    else if (ks_zoom>0){
      double width = 4 * 256*(1<<(ks_zoom-2));
      double deg_per_pt = 180.0/width; // ~188
      double mpp   = deg_per_pt * M_PI/180 * 6378137.0;
      k=1.0/mpp;
    }
    else {
      k=scale/2.54e-2*dpi;
    }
  }

  // rescale reference
  ref  *= k*factor;
  geom *= k*factor;
  ref  -= geom.TLC();
  geom -= geom.TLC();

  // is output image not too large
  Point<int> max_image(5000,5000);
  opt.get("max_image", max_image);
  cerr << "Image size: " << geom.w << "x" << geom.h << "\n";
  if ((geom.w>max_image.x) || (geom.h>max_image.y)){
     cerr << "Error: image is too large ("
          << geom.w << "x" << geom.h << ") pixels. "
          << "You may change max_image option to pass this test.\n";
    exit(1);
  }

  Image<int> im(geom.w,geom.h,0x00FFFFFF);

  if (gg_zoom>=0){
    string dir=""; opt.get("google_dir", dir);
    bool download=false; opt.get("download", download);
    LayerGoogle l(dir, gg_zoom);
    l.set_ref(ref);
    l.set_downloading(download);
    Image<int> tmp_im = l.get_image(geom);
    if (!tmp_im.empty()) im.render(Point<int>(0,0), tmp_im);
  }

  if (ks_zoom>=0){
    string dir=""; opt.get("ks_dir", dir);
    bool download=false; opt.get("download", download);
    LayerKS l(dir, ks_zoom);
    l.set_ref(ref);
    l.set_downloading(download);
    Image<int> tmp_im = l.get_image(geom);
    if (!tmp_im.empty()) im.render(Point<int>(0,0), tmp_im);
  }

  if (!world.maps.empty()){
    bool draw_borders=false;  opt.get("draw_borders", draw_borders);
    LayerGeoMap  l(&world, draw_borders);
    l.set_ref(ref);
    Image<int> tmp_im = l.get_image(geom);
    if (!tmp_im.empty()) im.render(Point<int>(0,0), tmp_im);
  }

  if (!world.trks.empty() || !world.wpts.empty()){
    geo_data world1=world; // LayerGeoData can't get const world
    LayerGeoData l(&world1);
    l.set_ref(ref);
    Image<int> tmp_im = l.get_image(geom);
    if (!tmp_im.empty()) im.render(Point<int>(0,0), tmp_im);
  }

  image_r::save(im, filename, opt);

  // writing html map if needed
  string htmfile=""; opt.get("htm", htmfile);
  if (htmfile != ""){
    ofstream f(htmfile.c_str());
    f << "<html><body>\n"
      << "<img border=\"0\" "
      <<      "src=\""    << filename << "\" "
      <<      "width=\""  << geom.w << "\" " 
      <<      "height=\"" << geom.h << "\" " 
      <<      "usemap=\"#m\">\n"
      << "<map name=\"m\">\n";
    for (vector<g_map>::const_iterator i=world.maps.begin(); i!=world.maps.end(); i++){
      convs::map2map cnv(*i, ref);
      g_line brd=cnv.line_frw(i->border);
      f << "<area shape=\"poly\" " 
        <<       "href=\""   << i->file << "\" "
        <<       "alt=\""    << i->comm << "\" "
        <<       "title=\""  << i->comm << "\" "
        <<       "coords=\"" << Line<int>(brd) << "\">\n";
    }
    f << "</map>\n"
      << "</body></html>";
    f.close();
  }

  // writing fig if needed
  string figfile=""; opt.get("fig", figfile);
  if (figfile != ""){
    fig::fig_world W;
    g_map fig_ref= ref * 2.54 / dpi * fig::cm2fig;
    fig::set_ref(W, fig_ref, Options());

    fig::fig_object o = fig::make_object("2 5 0 1 0 -1 500 -1 -1 0.000 0 0 -1 0 0 *");

    for (g_map::iterator i=fig_ref.begin(); i!=fig_ref.end(); i++){
      o.push_back(Point<int>(int(i->xr), int(i->yr)));
    }
    o.push_back(Point<int>(int(fig_ref[0].xr), int(fig_ref[0].yr)));

    o.image_file = filename;
    o.comment.push_back(string("MAP ") + filename);
    W.push_back(o);

    ofstream f(figfile.c_str());
    fig::write(f, W);
  }

  // writing map if needed
  string mapfile=""; opt.get("map", mapfile);
  if (mapfile != ""){
    ofstream f(mapfile.c_str());
    oe::write_map_file(f, ref, Options());
  }
}
} //namespace
