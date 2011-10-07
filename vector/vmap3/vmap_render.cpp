#include "vmap_renderer.h"
#include "options/m_getopt.h"
#include "options/m_time.h"

using namespace std;

void usage(){
  const char * prog = "vmap_render";

  cerr
     << prog << " -- convert vector maps to raster.\n"
     << "  usage: " << prog << " [<options>] <in_file> <out_file>\n"
     << "\n"
     << "  options:\n"
     << "    -m, --map <map file>    -- write OziExplorer map file\n"
     << "    -d, --dpi <dpi>         -- set map dpi (default 300)\n"
     << "    -g  --grid <step>       -- draw step x step cm grid\n"
     << "    -l  --grid_labels       -- draw grid labels\n"
     << "    -N  --draw_name         -- draw map name\n"
     << "    -D  --draw_date         -- draw date stamp\n"
     << "    -T  --draw_text <text>  -- draw text\n"
     << "        --antialiasing <0|1>  -- do antialiasing (default 1)\n"
     << "        --contours <0|1>      -- auto contours (default 1)\n"
     << "        --label_style <int>   -- set label style 0..2 (default 2)\n"
     << "\n"
     << "label styles (0 is fast and 2 is best):\n"
     << " 0 -- don't erase objects under text\n"
     << " 1 -- lighten all objects under text\n"
     << " 2 -- the best erasing [default] \n"
     << "\n"
     << "contours:\n"
     << " 0 -- no auto contours around forests\n"
     << " 1 -- draw contours (needs slightly more memory and time) [default]\n"
     << "\n"
     << "antialiasing:\n"
     << " 0 -- don't do antialiasing (needs 2x time (!))\n"
     << " 1 -- do antialiasing (needs slightly more memory) [default]\n"
  ;
  exit(1);
}


static struct option options[] = {
  {"map",           1, 0, 'm'},
  {"dpi",           1, 0, 'd'},
  {"grid",          1, 0, 'g'},
  {"grid_labels",   0, 0, 'l'},
  {"draw_name",     0, 0, 'N'},
  {"draw_date",     0, 0, 'D'},
  {"draw_text",     1, 0, 'T'},
  {"antialiasing",  1, 0, 0},
  {"contours",      1, 0, 0},
  {"label_style",   1, 0, 0},
  {0,0,0,0}
};

main(int argc, char* argv[]){

  if (argc==1) usage();

  Options O = parse_options(&argc, &argv, options);

  if (argc<2) usage();
  const char * ifile = argv[0];
  const char * ofile = argv[1];

  double dpi=O.get<int>("dpi", 300);

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

  bool use_aa = O.get<bool>("antialiasing", true);

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

  vmap::world W=vmap::read(ifile);
  if (W.size()==0) exit(1);

  // try to detect border from nom_name
  // todo - use option set from vmap_filt.cpp
  dRect nom_range=convs::nom_to_range(W.name);
  if (nom_range.empty())  nom_range=convs::nom_to_range(ifile);
  if (!nom_range.empty()){
    convs::pt2pt nom_cnv(
      Datum("wgs84"), Proj("lonlat"), Options(),
      Datum("pulkovo"), Proj("lonlat"), Options());
    W.brd = nom_cnv.line_bck(rect2line(nom_range));
  }

  g_map ref = vmap::mk_tmerc_ref(W, dpi/2.54, true);

  dRect rng = ref.border.range();
  rng.x = rng.y = 0;
  rng.w+=lm+rm; if (rng.w<0) rng.w=0;
  rng.h+=tm+bm; if (rng.h<0) rng.h=0;
  ref+=dPoint(lm,tm);
  cerr
     << "  scale  = 1:" << int(W.rscale) << "\n"
     << "  dpi    = " << dpi << "\n"
     << "  image = " << int(rng.w) << "x" << int(rng.h)<< "\n";

  VMAPRenderer R(&W, rng.w, rng.h, ref, dpi, use_aa);

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
  string map = O.get<string>("map");
  R.save_image(ofile, (map=="") ? NULL:map.c_str());


  //*******************************
}
