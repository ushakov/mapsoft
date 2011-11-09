#include "options/m_getopt.h"
#include "geo_io/geofig.h"
#include "geo/geo_refs.h"

using namespace std;

void usage(){
  const char * prog = "mapsoft_geofig";

  cerr
     << prog << " -- actions with geo-referenced FIG.\n"
     << "  usage: " << prog << " create [<options>]  (--out|-o) <output_file>\n"
     << "\n"
     << "  options for create action:\n"
     << "  -g --geom <geom>\n"
     << "  -d --datum <datum>\n"
     << "  -p --proj <proj>\n"
     << "  -l --lon0 <lon0>  -- create rectangular map in given projection\n"
     << "                   default datum -- pulkovo, default proj -- tmerc\n"
     << "                   lon 0 can be given through --lon0 or geom prefix\n"
     << "  -N --nom <name>          -- \n"
     << "  -G --google <x>,<y>,<z>  -- \n"
     << "  -r --rscale <rscale>     -- projection units / m\n"
     << "  -m --mag <factor>        -- additional magnification\n"
     << "  -y --swap_y              -- \n"
     << "  -v --verbose             -- be more verbose\n"
     << "\n"
  ;
  exit(1);
}

static struct option create_options[] = {
  {"geom",           1, 0, 'g'},
  {"datum",          1, 0, 'd'},
  {"proj",           1, 0, 'p'},
  {"lon0",           1, 0, 'l'},
  {"nom",            1, 0, 'N'},
  {"google",         1, 0, 'G'},
  {"rscale",         1, 0, 'r'},
  {"mag",            1, 0, 'm'},
  {"swap_y",         0, 0, 'y'},
  {"verbose",        0, 0, 'v'},
  {"out",            1, 0, 'o'},
  {0,0,0,0}
};

main(int argc, char **argv){
  try {

    if (argc<3) usage();
    string action = argv[1];
    argv++; argc--;

    if (action == "create"){
      Options O = parse_options(&argc, &argv, create_options, "out");
      if (!O.exists("out")){
        cerr << "error: no output files\n";
        exit(1);
      }
      O.put<string>("dpi", "fig");
      fig::fig_world F;
      fig::set_ref(F, mk_ref(O), Options());
      fig::write(O.get<string>("out"), F);
    }
    else usage();

  } catch (const char *err){
    cerr << "ERROR: " << err << "\n";
    exit(1);
  }
}


