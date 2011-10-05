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
#include "geo_io/geofig.h"
#include "2d/line_utils.h"

#include "io_oe.h"

using namespace std;

namespace img{

bool write_file (const char* filename, const geo_data & world, Options opt){

  Proj  proj(opt.get<string>("proj", "tmerc"));
  Datum datum(opt.get<string>("datum", "pulkovo"));

  dRect geom;
  if (opt.exists("geom")){
    geom = opt.get<dRect>("geom");


    if ((proj==Proj("tmerc"))&&(geom.x>1e6)){
      opt.put("lon0", convs::lon_pref2lon0(geom.x));
      geom.x=convs::lon_delprefix(geom.x);
    }
  }


  if (!opt.exists("lon0"))
    opt.put<double>("lon0", convs::lon2lon0(world.range().CNT().x));

  double scale  = opt.get("scale",  0.0);
  double rscale = opt.get("rscale", 0.0);
  double dpi    = opt.get("dpi",    0.0);
  double factor = opt.get("factor", 1.0);
  if (rscale!=0) scale=1.0/rscale;

  // Conversion to target coordiates
  convs::pt2pt c(Datum("wgs84"), Proj("lonlat"), Options(), datum, proj, opt);

  // setting default geometry from geodata
  if (geom.empty()){
    if (! world.range_geodata().empty()){
      geom=c.bb_frw(world.range_geodata(), world.range_geodata().w/1000);
    }
    if (! world.range().empty()){
      geom=c.bb_frw(world.range(), world.range().w/1000);
    }
    else {
      cerr << "Empty geometry! Use -g option.\n";
      exit(1);
    }
  }

  int ks_zoom = opt.get("ks",     -1);
  int gg_zoom = opt.get("google", -1);

  // creating initial map reference with borders
  g_map ref; // unscaled ref
  dLine brd;
  brd.push_back(geom.BLC());
  brd.push_back(geom.BRC());
  brd.push_back(geom.TRC());
  brd.push_back(geom.TLC());
  for (dLine::const_iterator p=brd.begin(); p!=brd.end(); p++){
    dPoint pg=*p;
    dPoint pr=*p;
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
    if (world.maps.size()>0){
      for (vector<g_map_list>::const_iterator i = world.maps.begin();
           i!=world.maps.end(); i++){
        if (i->size()>0){
          g_map orig_ref=convs::mymap(*i);
          k=1.0/convs::map_mpp(orig_ref, ref.map_proj);
          break;
        }
      }
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
    LayerGeoMap l(&d, draw_borders);
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
