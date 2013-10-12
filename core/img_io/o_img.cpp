#include <fstream>
#include <string>

#include "img_io/gobj_map.h"
#include "img_io/draw_trk.h"
#include "img_io/draw_wpt.h"
#include "img_io/gobj_srtm.h"
#include "loaders/image_r.h"

#include "geo/geo_convs.h"
#include "geo/geo_refs.h"
#include "2d/line_utils.h"

#include "utils/cairo_wrapper.h"

#include "geo_io/geofig.h"
#include "geo_io/io_oe.h"
#include "utils/err.h"

using namespace std;

namespace img{

bool write_file (const char* filename, const geo_data & world, Options opt){

  // Default scale is 1:100000.
  double rscale=opt.get("rscale", 100000.0);

  // We want to use source map scale if there is no dpi option
  bool do_rescale = false;
  if (!opt.exists("dpi")){
     opt.put("dpi", 100.0);
     do_rescale = true;
  }

  // set geometry if no --wgs_geom, --wgs_brd, --geom, --nom, --google options
  bool need_marg=false;
  if (!opt.exists("geom") && !opt.exists("wgs_geom") &&
      !opt.exists("nom") && !opt.exists("google") &&
      !opt.exists("wgs_brd")){
    dRect wgs_geom = world.range_geodata();

    if (!wgs_geom.empty() && opt.exists("data_marg")) need_marg=true;

    // fallback: map range
    if (wgs_geom.empty()) wgs_geom=world.range_map();

    if (wgs_geom.empty()) throw MapsoftErr("O_IMG_NO_RANGE")
                    << "Can't get map geometry.";

    opt.put("wgs_geom", wgs_geom);
  }

  // create g_map
  g_map ref = mk_ref(opt);
  dRect geom = ref.border.range();
  ref.file=filename;

  // set map scale from source maps if needed
  if (do_rescale){
    if (world.maps.size()>0){
      double maxscale=-1;
      dRect box = ref.range();
      vector<g_map_list>::const_iterator i;
      vector<g_map>::const_iterator j;
      for (i = world.maps.begin(); i!=world.maps.end(); i++){
        for (j = i->begin(); j!=i->end(); j++){
          if (j->size()<1) continue;
          convs::map2map cnv(*j, ref);
          dRect sbox = cnv.bb_bck(box);
          double sx = sbox.w/box.w;
          double sy = sbox.h/box.h;
          if (maxscale<sx) maxscale = sx;
          if (maxscale<sy) maxscale = sy;
        }
      }
      ref*=maxscale;
      geom*=maxscale;
    }
    else if (opt.exists("srtm_mode")){
      double mpp = 6380000.0 * M_PI /180/1200;
      double dpi = rscale/mpp / 100.0 * 2.54;
      double sc=dpi/opt.get("dpi", 100.0);
      ref*=sc;
      geom*=sc;
    }
  }

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

  if (opt.exists("srtm_mode")){
    srtm3 s(opt.get<string>("srtm_dir"));
    GObjSRTM l(&s);
    l.set_opt(opt);
    l.set_ref(ref);
    iImage tmp_im = l.get_image(geom);
    if (!tmp_im.empty()) im.render(iPoint(0,0), tmp_im);
  }

  for (int i=0; i<world.maps.size(); i++){
    g_map_list d(world.maps[i]);
    GObjMAP l(&d, opt);
    l.set_ref(ref);
    iImage tmp_im = l.get_image(geom);
    if (!tmp_im.empty()) im.render(iPoint(0,0), tmp_im);
  }

  convs::map2wgs cnv(ref);

  for (int i=0; i<world.trks.size(); i++){
    draw_trk(im, geom.TLC(), cnv, world.trks[i], opt);
  }

  for (int i=0; i<world.wpts.size(); i++){
    draw_wpt(im, geom.TLC(), cnv, world.wpts[i], opt);
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
    try {oe::write_map_file(mapfile.c_str(), ref);}
    catch (MapsoftErr e) {cerr << e.str() << endl;}
  }
}
} //namespace
