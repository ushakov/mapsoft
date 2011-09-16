#define DEBUG_LAYER_GEOMAP

#include <fstream>

#include "layers/layer_geomap.h"
#include "loaders/image_r.h"
#include "geo_io/io.h"
#include "geo/geo_convs.h"
#include "geo_io/geofig.h"

#include "options/read_conf.h"
#include "2d/line_utils.h"

using namespace std;

void usage(const char *fname){
  cerr << "Usage: "<< fname << " <in1> ... <inN> -O option1[=value1] ... -o <out>\n";

cerr <<
""<<
"  -s <string>\n" <<
"  --skip=<string>\n" <<
"  -O skip=<string>\n" <<
"                    -- Skipping data\n" <<
"  -g <rect>\n"
"  --geom=<rect>\n"
"  -O geom=<rect>\n"
"                    -- Geometry\n" <<
"  -p <proj>\n"
"  --proj=<proj>\n"
"  -O proj=<proj>\n"
"                    -- Projection (tmerc)\n" <<
"  -d <datum>\n"
"  --datum=<datum>\n"
"  -O datum=<datum>\n"
"                    -- Datum (pulkovo)\n" <<
"\n" <<
"  If <string> contains letter:\n" <<
"    \"m\" - skip maps\n" <<
"    \"w\" - skip waypoints\n" <<
"    \"t\" - skip tracks\n" <<
"    \"a\" - skip ACTIVE LOG track\n" <<
"    \"o\" - skip all but ACTIVE LOG tracks\n" <<
"\n";

// не все так просто, надо будет написать подробнее...
  exit(0);
}


int
main(int argc, char **argv){

  Options opts;

  if (!read_conf(argc, argv, opts)) usage(argv[0]);
  if (opts.exists("help")) usage(argv[0]);

  string outfile = opts.get("out", string());
  string figfile = opts.get("fig", string());
  string htmfile = opts.get("htm", string());
  string mapfile = opts.get("map", string());

  dRect geom = opts.get<dRect>("geom");
  if (geom.empty()){
    cerr << "Empty geometry! Use -g option.\n";
    exit(1);
  }
  if (geom.x>1e6){
    opts.put("lon0", convs::lon_pref2lon0(geom.x));
    geom.x=convs::lon_delprefix(geom.x);
  }

  Proj  proj(opts.get("proj", string("tmerc")));
  Datum datum(opts.get("datum", string("pulkovo")));

  double scale  = opts.get("scale",  0.0);
  double rscale = opts.get("rscale", 0.0);
  double dpi    = opts.get("dpi",    0.0);
  double factor = opts.get("factor", 1.0);
  if (rscale!=0) scale=1.0/rscale;

  g_map ref; // unscaled ref
  convs::pt2pt c(datum, proj, opts, Datum("wgs84"), Proj("lonlat"), Options());
  dLine brd = rect2line(geom);
  for (dLine::const_iterator p=brd.begin(); p!=brd.end(); p++){
    dPoint pg=*p;
    dPoint pr=*p;
    c.frw(pg);
    pr.y=(geom.y+geom.h)-(pr.y-geom.y);
    ref.push_back(g_refpoint(pg, pr));
    ref.border.push_back(pr);
    cerr << "ref: " << pg << " " << pr << "\n";
  }
  ref.map_proj=boost::lexical_cast<string>(proj);
  ref.file=outfile;

  double k=0; // scale for our ref
  if ((scale!=0) && (dpi!=0)) k = scale/2.54e-2*dpi;

  // fallbacks
  if (dpi==0) dpi=200;
  if (scale==0) scale=1e-5;

  // если нам надо читать какие-то карты:
  if (opts.exists("cmdline_args")){

    geo_data world;

    //чтение файлов из командной строки:
    StrVec infiles = opts.get("cmdline_args", StrVec());
    for(StrVec::const_iterator i=infiles.begin(); i!=infiles.end(); i++)
      io::in(*i, world, opts);

    // put all maps into one map_list
    g_map_list maps;
    for (std::vector<g_map_list>::const_iterator ml = world.maps.begin();
      ml!=world.maps.end(); ml++) maps.insert(maps.end(), ml->begin(), ml->end());

    bool draw_borders = opts.get("draw_borders", false);
    LayerGeoMap ml(&maps, draw_borders);
    g_map orig_ref=ml.get_ref();

    g_map map=ml.get_ref();
    if (k==0) k=1.0/convs::map_mpp(map, map.map_proj);
    if (k==0) k=scale/2.54e-2*dpi;

    if (outfile != ""){
      cerr << "writing image: " << iRect(geom*k*factor) << "\n";
      ml.set_ref(ref*k*factor);
      iImage im = ml.get_image(iRect(geom*k*factor));
      image_r::save(im, outfile.c_str(), opts);
    }
    // ml.dump_maps("out1.fig");

    if (htmfile != ""){
      ofstream f(htmfile.c_str());
      f << "<html><body>\n"
        << "<img border=\"0\" "
        <<      "src=\""    << outfile << "\" "
        <<      "width=\""  << int(rint(geom.w*k*factor)) << "\" " 
        <<      "height=\"" << int(rint(geom.h*k*factor)) << "\" " 
        <<      "usemap=\"#m\">\n"
        << "<map name=\"m\">\n";
      for (g_map_list::const_iterator i = maps.begin(); i!=maps.end(); i++){
        convs::map2map cnv(*i, ref*k*factor);
        dLine brd=cnv.line_frw(i->border) - geom.TLC()*k*factor;
        f << "<area shape=\"poly\" " 
          <<       "href=\""   << i->file << "\" "
          <<       "alt=\""    << i->comm << "\" "
          <<       "title=\""  << i->comm << "\" "
          <<       "coords=\"" << iLine(brd) << "\">\n";
      }
      f << "</map>\n"
        << "</body></html>";
      f.close();
    }
  }

  if (k==0) k=scale/2.54e-2*dpi;


  if (figfile != ""){
    fig::fig_world W;
    g_map fig_ref= (ref - dPoint(ref[0].xr, ref[0].yr)) * k * 2.54 / dpi * factor * fig::cm2fig;
    fig::set_ref(W, fig_ref, Options());

    if (outfile != ""){
      fig::fig_object o = fig::make_object("2 5 0 1 0 -1 500 -1 -1 0.000 0 0 -1 0 0 *");

      for (g_map::iterator i=fig_ref.begin(); i!=fig_ref.end(); i++){
        o.push_back(iPoint(int(i->xr), int(i->yr)));
      }
      o.push_back(iPoint(int(fig_ref[0].xr), int(fig_ref[0].yr)));

      o.image_file = outfile;
      o.comment.push_back("MAP " + outfile);
      W.push_back(o);
    }

    fig::write(figfile, W);
  }
  if (mapfile != ""){
    ofstream f(mapfile.c_str());
    oe::write_map_file(f, (ref-dPoint(ref[0].xr, ref[0].yr)) * k * factor, Options());
  }
}
