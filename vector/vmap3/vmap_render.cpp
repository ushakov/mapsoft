#include "vmap_renderer.h"
#include "options/m_getopt.h"
#include "options/m_time.h"
#include "geo/geo_refs.h"
#include "loaders/image_r.h"
#include "utils/err.h"

using namespace std;

static struct ext_option options[] = {
  {"verbose",       0,  0, 1, "be more verbose"},
  {"help",          0,'h', 1, "show this message"},
  {"map",           1,'m', 1, "write OziExplorer map file"},
  {"grid",          1,'g', 1, "draw grid with given step [cm]"},
  {"grid_labels",   0,'l', 1, "draw grid labels"},
  {"draw_name",     0,'N', 1, "draw map name"},
  {"draw_date",     0,'D', 1, "draw date stamp"},
  {"draw_text",     1,'T', 1, "draw text\n"},
  {"bgcolor",       1, 0,  1, "backgound color\n"},

  {"antialiasing",  1,  0, 1, "do antialiasing (0|1, default 1)"},
  {"transp_margins",1,  0, 1, "transparent margins (0|1, default 0)"},
  {"contours",      1,  0, 1, "auto contours (0|1, default 1)"},
  {"label_style",   1,  0, 1, "set label style 0..2 (default 2)\n"},

  {"geom",          1,  0, 2, ""},
  {"datum",         1,  0, 2, ""},
  {"proj",          1,  0, 2, ""},
  {"lon0",          1,  0, 2, ""},
  {"wgs_geom",      1,  0, 2, ""},
  {"wgs_brd",       1,  0, 2, ""},
  {"nom",           1,  0, 2, ""},
  {"google",        1,  0, 2, "google tile, \"x,y,z\""},
  {"rscale",        1,  0, 2, "reversed scale (10000 for 1:10000 map)"},
  {"dpi",           1,'d', 2, "resolution, dots per inch"},
  {"mag",           1,  0, 2, "additional magnification"},
  {"swap_y",        0,  0, 2, "\n"},

  {0,0,0,0}
};

void usage(){
  const char * prog = "vmap_render";

  cerr
     << prog << " -- convert vector maps to raster.\n"
     << "Usage: " << prog << " [<options>] <in_file> <out_file>\n"
     << "\n"
     << "Options:\n"
  ;
  print_options(options, 1, cerr);

  cerr 
     << "\n"
     << "Label styles (0 is fast and 2 is best):\n"
     << "  0 -- don't erase objects under text\n"
     << "  1 -- lighten all objects under text\n"
     << "  2 -- the best erasing [default] \n"
     << "\n"
     << "Contours:\n"
     << "  0 -- no auto contours around forests\n"
     << "  1 -- draw contours (needs slightly more memory and time) [default]\n"
     << "\n"
     << "Antialiasing:\n"
     << "  0 -- don't do antialiasing (needs 2x time (!))\n"
     << "  1 -- do antialiasing (needs slightly more memory) [default]\n"
     << "\n"
     << "Options to specify map range and projection:\n"
  ;
  print_options(options, 2, cerr);
  cerr
     << "\n"
     << "Default projection is tmerc, default range is a map border bounding box.\n"
     << "\n"

  ;
  exit(1);
}

main(int argc, char* argv[]){

  if (argc==1) usage();
  Options O = parse_options(&argc, &argv, options, 3);
  if (O.exists("help")) usage();

  if (argc<2) usage();
  const char * ifile = argv[0];
  const char * ofile = argv[1];

  // create map

  vmap::world W=vmap::read(ifile);
  if (W.size()==0) exit(1);

  // set geometry if no --wgs_geom, --wgs_brd, --geom,
  //  --nom, --google option exists
  if (!O.exists("geom") && !O.exists("wgs_geom") &&
      !O.exists("nom") && !O.exists("google") &&
      !O.exists("wgs_brd")){
    O.put("wgs_brd", W.brd);
  }

  if (!O.exists("rscale")) O.put("rscale", W.rscale);

  g_map ref = mk_ref(O);
  ref.comm=W.name;

  // process other options

  double dpi=O.get<double>("dpi", 300);

  // set margins
  int tm=0, bm=0, lm=0, rm=0;
  if (O.get<int>("draw_name", 0) ||
      O.get<int>("draw_date", 0) ||
      (O.get<string>("draw_text") != "")) {
    tm=dpi/3;
    bm=lm=rm=dpi/6;
  }
  int grid_labels = O.get<int>("grid_labels", 0);
  if (grid_labels){
    bm+=dpi/6;
    tm+=dpi/6;
    rm+=dpi/6;
    lm+=dpi/6;
  }

  label_style_t ls;
  switch(O.get<int>("label_style", 2)){
    case 0: ls=LABEL_STYLE0; break;
    case 1: ls=LABEL_STYLE1; break;
    case 2: ls=LABEL_STYLE2; break;
    default:
      cerr << "Error: unknowl label style: "
           << O.get<int>("label_style", 0) << " (must be 0..3)\n";
      exit(1);
  }

  dRect rng = ref.border.range();
  rng.x = rng.y = 0;
  rng.w+=lm+rm; if (rng.w<0) rng.w=0;
  rng.h+=tm+bm; if (rng.h<0) rng.h=0;
  ref+=dPoint(lm,tm);
  cerr
     << "  scale  = 1:" << int(W.rscale) << "\n"
     << "  dpi    = " << dpi << "\n"
     << "  image = " << int(rng.w) << "x" << int(rng.h)<< "\n";


 // modify vmap
  vmap::join_labels(W);
  vmap::move_pics(W);

  iImage img(rng.w, rng.h);


  VMAPRenderer R(&W, img, ref, O);

  R.render_objects(O.get<bool>("contours", true));

  double grid_step = O.get<double>("grid", 0);
  if (grid_step>0) R.render_pulk_grid(grid_step, grid_step, false);

  R.render_labels(ls);

  // draw grid labels after labels
  if ((grid_step>0) && grid_labels)
    R.render_pulk_grid(grid_step, grid_step, true);

  if (O.get<int>("draw_name", 0))
    R.render_text(W.name.c_str(), dPoint(dpi/5,dpi/15), 0, 0, 18, 14, 0, 2);

  if (O.get<int>("draw_date", 0)){
    Time t; t.set_current();
    R.render_text(t.date_str().c_str(), dPoint(dpi/30,dpi), -M_PI/2, 0, 18, 10, 2, 2);
  }

  if (O.get<string>("draw_text") != ""){
    R.render_text(O.get<string>("draw_text").c_str(), dPoint(dpi/5,-dpi/30), 0, 0, 18, 10, 0, 0);
  }

  //*******************************
  image_r::save(img, ofile);

  string map = O.get<string>("map");
  if (map!=""){
    g_map M = ref;
    M.file = ofile;
    if (W.brd.size()>2) M.border=W.brd;
    M.border.push_back(*M.border.begin());
    M.border=generalize(M.border,1,-1); // 1pt accuracy
    M.border.resize(M.border.size()-1);
    try {oe::write_map_file(map.c_str(), M);}
    catch (MapsoftErr e) {cerr << e.str() << endl;}
  }


  //*******************************
}
