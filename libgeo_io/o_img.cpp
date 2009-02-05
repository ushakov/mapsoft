#include <fstream>
#include <string>

#include "../layers/layer_geomap.h"
#include "../layers/layer_geodata.h"
#include "../libgeo/geo_convs.h"
#include "../libgeo_io/geofig.h"
#include "../lib2d/line_utils.h"

#include "io_oe.h"

using namespace std;

namespace img{

bool write_file (const char* filename, const geo_data & world, const Options & opt){

  string figfile="";   opt.get("fig", figfile);
  string htmfile="";   opt.get("htm", htmfile);
  string mapfile="";   opt.get("map", mapfile);

  Rect<double> geom;   opt.get("geom", geom);


  Proj  proj("tmerc");    opt.get("proj", proj);
  Datum datum("pulkovo"); opt.get("datum", datum);

  double scale=0, rscale=0, dpi=0, factor=1;
  opt.get("scale",  scale);
  opt.get("rscale", rscale);
  opt.get("dpi",    dpi);
  opt.get("factor", factor);
  if (rscale!=0) scale=1.0/rscale;


// Building reference

  g_map ref; // unscaled ref
  convs::pt2pt c(Datum("wgs84"), Proj("lonlat"), Options(), datum, proj, opt);

  // setting geometry
  if (geom.empty()){
    if (! world.range_geodata().empty()){
      geom=c.bb_frw(world.range_geodata(), world.range_geodata().w/1000);
    } else {
      cerr << "Empty geometry! Use -g option.\n";
      exit(1);
    }
  }

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
    cerr << "ref: " << pg << " " << pr << "\n";
  }
  ref.map_proj=boost::lexical_cast<string>(proj);
  ref.file=filename;

  double k=0; // scale for our ref
  if ((scale!=0) && (dpi!=0)) k = scale/2.54e-2*dpi;

  // fallbacks
  if (dpi==0) dpi=200;
  if (scale==0) scale=1e-5;

  // if we need layers for maps or data:
  if (!world.range().empty()){

    bool draw_borders=false;  opt.get("draw_borders", draw_borders);

    geo_data world1=world; // LayerGeoData can't get const world
    LayerGeoMap  ml(&world, draw_borders);
    LayerGeoData dl(&world1);
    g_map orig_ref=ml.get_ref();

    if (k==0) k=1.0/convs::map_mpp(orig_ref);
    if (k==0) k=scale/2.54e-2*dpi; // setting scale from map layer if it is not set yet

    if (filename != ""){
      cerr << "writing image: " << Rect<int>(geom*k*factor) << "\n";
      ml.set_ref(ref*k*factor);
      dl.set_ref(ref*k*factor);
      Image<int> im1 = ml.get_image(Rect<int>(geom*k*factor));
      Image<int> im2 = dl.get_image(Rect<int>(geom*k*factor));
      im1.render(Point<int>(0,0), im2);
      image_r::save(im1, filename, opt);
    }
    // ml.dump_maps("out1.fig");

    // writing html map if needed
    if (htmfile != ""){
      ofstream f(htmfile.c_str());
      f << "<html><body>\n"
        << "<img border=\"0\" "
        <<      "src=\""    << filename << "\" "
        <<      "width=\""  << int(rint(geom.w*k*factor)) << "\" " 
        <<      "height=\"" << int(rint(geom.h*k*factor)) << "\" " 
        <<      "usemap=\"#m\">\n"
        << "<map name=\"m\">\n";
      for (vector<g_map>::const_iterator i=world.maps.begin(); i!=world.maps.end(); i++){
        convs::map2map cnv(*i, ref*k*factor);
        g_line brd=cnv.line_frw(i->border) - geom.TLC()*k*factor;
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
  }

  if (k==0) k=scale/2.54e-2*dpi;

  // writing fig if needed
  if (figfile != ""){
    fig::fig_world W;
    g_map fig_ref= (ref - g_point(ref[0].xr, ref[0].yr)) * k * 2.54 / dpi * factor * fig::cm2fig;
    fig::set_ref(W, fig_ref, Options());

    if (filename != ""){
      fig::fig_object o = fig::make_object("2 5 0 1 0 -1 500 -1 -1 0.000 0 0 -1 0 0 *");

      for (g_map::iterator i=fig_ref.begin(); i!=fig_ref.end(); i++){
        o.push_back(Point<int>(int(i->xr), int(i->yr)));
      }
      o.push_back(Point<int>(int(fig_ref[0].xr), int(fig_ref[0].yr)));

      o.image_file = filename;
      o.comment.push_back(string("MAP ") + filename);
      W.push_back(o);
    }

    ofstream f(figfile.c_str());
    fig::write(f, W);
  }

  // writing map if needed
  if (mapfile != ""){
    ofstream f(mapfile.c_str());
    oe::write_map_file(f, (ref-g_point(ref[0].xr, ref[0].yr)) * k * factor, Options());
  }
}
} //namespace
