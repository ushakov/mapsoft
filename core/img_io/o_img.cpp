#include <fstream>
#include <string>

#include "img_io/gobj_map.h"
#include "img_io/gobj_trk.h"
#include "img_io/gobj_wpt.h"
#include "img_io/gobj_srtm.h"
#include "img_io/gobj_vmap.h"
#include "loaders/image_r.h"

#include "geo/geo_convs.h"
#include "geo_io/geo_refs.h"
#include "2d/line_utils.h"

#include "utils/cairo_wrapper.h"

#include "geo_io/geofig.h"
#include "geo_io/io_oe.h"
#include "err/err.h"

#include "gobj_comp.h"

using namespace std;

namespace img{

bool write_file (const char* filename, geo_data & world, vmap::world & vm, Options opt){

  // Default scale from vmap or 1:100000.
  if (!opt.exists("rscale")){
    if (!vm.empty()) opt.put("rscale", vm.rscale);
    else opt.put("rscale", 100000.0);
  }
  double rscale=opt.get("rscale", 100000.0);

  // We want to use source map scale if there is no dpi option
  bool do_rescale = false;
  if (!opt.exists("dpi")){
//     opt.put("dpi", 100.0);
//     do_rescale = true;
  }
  double dpi=opt.get("dpi", 100.0);


  // set geometry if no --wgs_geom, --wgs_brd, --geom, --nom, --google options
  bool need_marg=false;
  if (!opt.exists("geom") && !opt.exists("wgs_geom") &&
      !opt.exists("nom") && !opt.exists("google") &&
      !opt.exists("wgs_brd") && !opt.exists("trk_brd")){
    dRect wgs_geom = world.range_geodata();

    if (!wgs_geom.empty() && opt.exists("data_marg")) need_marg=true;

    // fallback: vmap range
    if (wgs_geom.empty()) wgs_geom=vm.range();

    // fallback: map range
    if (wgs_geom.empty()) wgs_geom=world.range_map();

    if (wgs_geom.empty()) throw Err() << "Can't get map geometry.";

    opt.put("wgs_geom", wgs_geom);
  }

  // create g_map
  g_map ref = mk_ref(opt);
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
      ref*=maxscale * opt.get("mag", 1.0);
    }
    else if (opt.exists("srtm_mode")){
      double mpp = 6380000.0 * M_PI /180/1200;
      double dpi = rscale/mpp / 100.0 * 2.54;
      double sc=dpi/opt.get("dpi", 100.0) * opt.get("mag", 1.0);
      ref*=sc;
    }
  }

  // data margin -- only if the range was set from the data bbox
  if (need_marg){
    dRect geom = rect_pump(ref.border.range(), opt.get("data_marg", 0.0));
    ref.border=rect2line(geom);
  }

  // set margins for text
  int tm=0, bm=0, lm=0, rm=0;
  if (opt.get<int>("draw_name", 0) ||
      opt.get<int>("draw_date", 0) ||
      (opt.get<string>("draw_text") != "")) {
    tm=dpi/3;
    bm=lm=rm=dpi/6;
  }
  int grid_labels = opt.get<int>("grid_labels", 0);
  if (grid_labels){
    bm+=dpi/6;
    tm+=dpi/6;
    rm+=dpi/6;
    lm+=dpi/6;
  }

  // image size
  dRect geom = ref.border.range();
  geom.x = geom.y = 0;
  geom.w+=lm+rm; if (geom.w<0) geom.w=0;
  geom.h+=tm+bm; if (geom.h<0) geom.h=0;
  ref+=dPoint(lm,tm);

  // is output image not too large
  iPoint max_image = opt.get("max_image", Point<int>(10000,10000));
  cerr << "Image size: " << geom.w << "x" << geom.h << "\n";
  if ((geom.w>max_image.x) || (geom.h>max_image.y))
    throw Err() << "Error: image is too large ("
                << geom.w << "x" << geom.h << ") pixels. "
                << "You may change max_image option to pass this test.\n";

  // create gobj
  GObjComp gobj;

  SRTM3 s(opt.get<string>("srtm_dir"));
  if (opt.exists("srtm_mode"))
    gobj.push_back(new GObjSRTM(&s));

  for (int i=0; i<world.maps.size(); i++)
    gobj.push_back(new GObjMAP(&(world.maps[i]), opt));

  if (!vm.empty()) gobj.push_back(new GObjVMAP(&vm, opt));

  for (int i=0; i<world.trks.size(); i++)
    gobj.push_back(new GObjTRK(&(world.trks[i]), opt));

  for (int i=0; i<world.wpts.size(); i++)
    gobj.push_back(new GObjWPT(&(world.wpts[i]), opt));

  gobj.set_ref(ref);
  gobj.set_opt(opt);


  // draw
  int bgcolor   = opt.get("bgcolor", 0xFFFFFFFF);
  int tmap      = opt.exists("tiles");
  int skipempty = opt.exists("tiles_skipempty");
  int tsize     = 256;

  if (!tmap){
    iImage im(geom.w,geom.h, bgcolor);
    gobj.draw(im, iPoint(0,0));
    if (ref.border.size()>2){
      // clear image outside border
      CairoWrapper cr(im);
      cr->render_border(geom, ref.border, bgcolor);
    }
    image_r::save(im, filename, opt);
  }
  else{ // tiles
    string fname(filename);
    string dir = fname;
    string ext = ".jpg";
    int pos    = fname.rfind('.');
    if ((pos>0) && (pos < fname.length()-1)){
       ext = fname.substr(pos);
       dir = fname.substr(0, pos);
    }
    int res = mkdir(dir.c_str(), 0755);

    iRect trect = tiles_on_rect(geom,tsize);
    iPoint p0(0,0);
    if (opt.get<string>("tiles_origin", "image") == string("proj")){
      dPoint dp0(opt.get("lon0", 0), 0);
      gobj.cnv.bck(dp0);
      p0 = iPoint(dp0);
      trect = tiles_on_rect(geom-p0,tsize);
    }

    for (int y=trect.TLC().y; y<=trect.BRC().y; y++){
      for (int x=trect.TLC().x; x<=trect.BRC().x; x++){

        iImage im(tsize, tsize, bgcolor);
        iPoint org = iPoint(x,y)*tsize + p0;
        int res = gobj.draw(im, org);
        if (res == GObj::FILL_NONE && skipempty) continue;
        //if (ref.border.size()>2){
        //  // clear image outside border
        //  CairoWrapper cr(im);
        //  cr->render_border(geom, ref.border-org, bgcolor);
        //}

        int wx=0,wy=0;
        //if (mx > 1) wx = (int) floor (log (1.0 * mx) / log (10.0)) + 1;
        //if (my > 1) wy = (int) floor (log (1.0 * my) / log (10.0)) + 1;

        ostringstream fn;
        fn << dir << "/"
           << setw(wx) << setfill('0') << x << "_"
           << setw(wy) << setfill('0') << y << ext;
        cout << "Writing " << fn.str() << "\n";
        image_r::save(im, fn.str().c_str(), opt);
      }
    }
  }

  // clear gobj
  for (GObjComp::iterator i = gobj.begin(); i!=gobj.end(); i++) free(*i);


  // writing html map if needed
  string htmfile = opt.get<string>("htm");
  if (!tmap && htmfile != ""){
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
  if (!tmap && figfile != ""){
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
  if (!tmap && mapfile != ""){
    /* simplify border */
    ref.border.push_back(*ref.border.begin());
    ref.border=generalize(ref.border,1,-1); // 1pt accuracy
    ref.border.resize(ref.border.size()-1);
    try {oe::write_map_file(mapfile.c_str(), ref);}
    catch (Err e) {cerr << e.get_error() << endl;}
  }
}
} //namespace
