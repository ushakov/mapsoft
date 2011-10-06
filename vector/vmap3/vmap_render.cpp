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
  {0,0,0,0}
};

main(int argc, char* argv[]){

  if (argc==1) usage();

  Options O = parse_options(&argc, &argv, options);

  if (argc<2) usage();
  const char * ifile = argv[0];
  const char * ofile = argv[1];

  int dpi=O.get<int>("dpi", 300);

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

  VMAPRenderer R(ifile, dpi, lm, tm, rm, bm);

  R.render_objects();

  double grid_step = O.get<double>("grid", 0);
  if (grid_step>0) R.render_pulk_grid(grid_step, grid_step, false);

  R.render_labels();

  // draw grid labels after labels
  if ((grid_step>0) && grid_labels) 
    R.render_pulk_grid(grid_step, grid_step, true);

  if (O.get<int>("draw_name", 0))
    R.render_text(R.W.name.c_str(), dPoint(dpi/5,dpi/15), 0, 0, 18, 14, 0, 2);

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
