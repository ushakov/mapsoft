#include "options/m_getopt.h"
#include "geo_io/geofig.h"
#include "geo/geo_refs.h"
#include "geo_io/io.h"

using namespace std;

void usage(){
  const char * prog = "mapsoft_geofig";

  cerr
     << prog << " -- actions with geo-referenced FIG.\n"
     << "  Usage:\n"
     << prog << " create [<options>]  (--out|-o) <output_file>\n"
     << prog << " add [<options>] <file1> ... <fileN>  (--out|-o) <output_file>\n"
     << prog << " del [<options>] (--out|-o) <output_file>\n"
     << "\n"
     << "Options for create action:\n"
     << "  -g --geom <geom>\n"
     << "  -d --datum <datum>\n"
     << "  -p --proj <proj>\n"
     << "  -l --lon0 <lon0>  -- create rectangular map in given projection\n"
     << "                   default datum -- pulkovo, default proj -- tmerc\n"
     << "                   lon 0 can be given through --lon0 or geom prefix\n"
     << "  -w --wgs_geom <geom>     -- \n"
     << "  -b --wgs_brd <line>      -- \n"
     << "  -N --nom <name>          -- \n"
     << "  -G --google <x>,<y>,<z>  -- \n"
     << "  -r --rscale <rscale>     -- reversed scale (10000 for 1:10000 map)\n"
     << "  -m --mag <factor>        -- additional magnification\n"
     << "  -y --swap_y              -- \n"
     << "  -v --verbose             -- be more verbose\n"
     << "\n"
     << "Options for add action:\n"
     << "  -v --verbose             -- be more verbose\n"
     << "\n"
     << "Options for add action:\n"
     << "  -w --wpts                -- remove waypoints\n"
     << "  -t --trks                -- remove tracks\n"
     << "  -m --maps                -- remove maps\n"
     << "  -b --brds                -- remove map borders\n"
     << "  -r --ref                 -- remove fig reference\n"
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
  {"wgs_geom",       1, 0, 'w'},
  {"wgs_brd",        1, 0, 'b'},
  {"nom",            1, 0, 'N'},
  {"google",         1, 0, 'G'},
  {"rscale",         1, 0, 'r'},
  {"mag",            1, 0, 'm'},
  {"swap_y",         0, 0, 'y'},
  {"verbose",        0, 0, 'v'},
  {"out",            1, 0, 'o'},
  {0,0,0,0}
};

static struct option add_options[] = {
  {"verbose",        0, 0, 'v'},
  {"out",            0, 0, 'o'},
  {0,0,0,0}
};

static struct option del_options[] = {
  {"wpts",           0, 0, 'w'},
  {"trks",           0, 0, 't'},
  {"maps",           0, 0, 'm'},
  {"brds",           0, 0, 'b'},
  {"ref",            0, 0, 'r'},
  {"verbose",        0, 0, 'v'},
  {"out",            1, 0, 'o'},
  {0,0,0,0}
};

main(int argc, char **argv){
  try {

    if (argc<3) usage();
    string action = argv[1];
    argv++; argc--;

    /*****************************************/
    if (action == "create"){
      Options O = parse_options(&argc, &argv, create_options, "out");
      if (!O.exists("out")){
        cerr << "error: no output files" << endl;
        exit(1);
      }
      O.put<string>("dpi", "fig");
      fig::fig_world F;
      fig::set_ref(F, mk_ref(O), Options());
      fig::write(O.get<string>("out"), F);
    }
    /*****************************************/
    else if (action == "add"){
      Options O = parse_options(&argc, &argv, add_options, "out");
      Options GO(O);
      geo_data world;
      while (!O.exists("out")) {
        if (argc<1){
          cerr << "error: no output files\n";
          exit(1);
        }
        const char * ifile = argv[0];
        O = parse_options(&argc, &argv, add_options, "out");
        O.insert(GO.begin(), GO.end());
        if (O.get<int>("verbose",0))
          cerr << "reading: " << ifile  << endl;
        io::in(ifile, world, O);
//        geo_data world1;
//        io::in(ifile, world1, O);
//        io::filter(world1);
//        world.add(world1);
      }

      if (argc<1){
        if (GO.get<int>("verbose",0))
        cerr << "error: no output files" << endl;
        exit(1);
      }

      const char * ofile = argv[0];

      if (GO.get<int>("verbose",0))
        cerr << "writing to: " << ofile << endl;

      fig::fig_world F;
      if (!fig::read(ofile, F)) {
        cerr << "error: can't read file: " << ofile << endl;
        exit(1);
      }
      g_map ref = fig::get_ref(F);
      put_wpts(F, ref, world);
      put_trks(F, ref, world);

      exit (!fig::write(ofile, F));
    }
    /*****************************************/
    else if (action == "del"){
      Options O = parse_options(&argc, &argv, del_options, "out");
      if (!O.exists("out")){
        cerr << "error: no output files" << endl;
        exit(1);
      }
      const char * ofile = O.get<string>("out").c_str();
      fig::fig_world F;
      if (!fig::read(ofile, F)) {
        cerr << "error: can't read file: " << ofile << endl;
        exit(1);
      }
      if (O.exists("wpts")) rem_wpts(F);
      if (O.exists("trks")) rem_trks(F);
      if (O.exists("maps")) rem_maps(F);
      if (O.exists("brds")) rem_brds(F);
      if (O.exists("ref"))  rem_ref(F);
      exit (!fig::write(ofile, F));
    }
    /*****************************************/
    else usage();

  } catch (const char *err){
    cerr << "Error: " << err << endl;
    exit(1);
  }
}


