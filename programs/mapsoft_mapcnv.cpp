#define DEBUG_LAYER_GEOMAP

#include <fstream>

#include "../layers/layer_geomap.h"
#include "../libgeo_io/io.h"
#include "../libgeo/geo_convs.h"
#include "../libgeo_io/geofig.h"

#include "../utils/read_conf.h"
#include "../lib2d/line_utils.h"

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

  string outfile="";
  opts.get("out_file", outfile);
  if (outfile == "") usage(argv[0]);

  string figfile="";
  opts.get("fig_file", figfile);
  string htmfile="";
  opts.get("htm_file", htmfile);
  string mapfile="";
  opts.get("map_file", mapfile);

  Options infiles; opts.get("args", infiles);

  Rect<double> geom;
  opts.get("geom", geom);
  if (geom.empty()){
    cerr << "Empty geometry! Use -g option.\n";
    exit(1);
  }

  Proj  proj("tmerc");    opts.get("proj", proj);
  Datum datum("pulkovo"); opts.get("datum", datum);

  double scale=0, rscale=0, dpi=0, factor=1;
  opts.get("scale",  scale);
  opts.get("rscale", rscale);
  opts.get("factor", factor);
  opts.get("dpi",    dpi);
  if (rscale!=0) scale=1.0/rscale;


  g_map ref; // unscaled ref
  convs::pt2pt c(datum, proj, opts, Datum("wgs84"), Proj("lonlat"), Options());
  g_line brd;
  brd.push_back(geom.BLC());
  brd.push_back(geom.BRC());
  brd.push_back(geom.TRC());
  brd.push_back(geom.TLC());
  for (g_line::const_iterator p=brd.begin(); p!=brd.end(); p++){
    g_point pg=*p;
    g_point pr=*p;
    c.frw(pg);
    pr.y=(geom.y+geom.w)-(pr.y-geom.y);
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
  if (opts.exists("args")){

    geo_data world;

    //чтение файлов из командной строки:
    Options infiles; opts.get("args", infiles);
    for(Options::const_iterator i=infiles.begin(); i!=infiles.end(); i++) 
      io::in(i->first, world, opts);

    bool draw_borders=false;
    opts.get("draw_borders", draw_borders);
    LayerGeoMap ml(&world, draw_borders);
    g_map orig_ref=ml.get_ref();

    if (k==0) k=1.0/convs::map_mpp(ml.get_ref());
    if (k==0) k=scale/2.54e-2*dpi;

    if (outfile != ""){
      cerr << "writing image: " << Rect<int>(geom*k*factor) << "\n";
      ml.set_ref(ref*k*factor);
      Image<int> im = ml.get_image(Rect<int>(geom*k*factor));
      image_r::save(im, outfile.c_str(), opts);
    }
    // ml.dump_maps("out1.fig");
  }

  if (k==0) k=scale/2.54e-2*dpi;

  if (htmfile != ""){
    /// todo
  }

  if (figfile != ""){
    fig::fig_world W;
    g_map fig_ref= (ref - g_point(ref[0].xr, ref[0].yr)) * k * 2.54 / dpi * factor * fig::cm2fig;
    fig::set_ref(W, fig_ref, Options());
    fig::fig_object o = fig::make_object("2 5 0 1 0 -1 500 -1 -1 0.000 0 0 -1 0 0 *");

    for (g_map::iterator i=fig_ref.begin(); i!=fig_ref.end(); i++){
      o.push_back(Point<int>(int(i->xr), int(i->yr)));
    }
    o.push_back(Point<int>(int(fig_ref[0].xr), int(fig_ref[0].yr)));

    o.image_file = outfile;
    o.comment.push_back("MAP " + outfile);
    W.push_back(o);
    ofstream f(figfile.c_str());
    fig::write(f, W);
  }
  if (mapfile != ""){
    ofstream f(mapfile.c_str());
    oe::write_map_file(f, (ref-g_point(ref[0].xr, ref[0].yr)) * k * factor, Options());
  }
}
