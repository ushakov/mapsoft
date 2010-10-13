#include <string>
#include <cstring>
#include "options/m_getopt.h"
#include "vmap/vmap.h"
#include "fig/fig.h"

using namespace std;

void usage(){
  const char * prog = "vmap_copy";

  cerr
     << prog << " -- convert vector maps in mp or fig formats.\n"
     << "  usage: " << prog << " [<global_input_options>]\\\n"
     << "         <input_file1> [<input_options1>] ... \\\n"
     << "         (--out|-o) <output_file> [<output_options>]\n"
     << "\n"
     << "  input options:\n"
     << "\n"
     << "    --skip_labels               -- don't read labels\n"
     << "    --read_labels               -- do read labels\n"
     << "\n"
     << "    --set_source <string>       -- set source parameter\n"
     << "    --set_source_from_name      -- set source parameter from map name\n"
     << "    --set_source_from_fname     -- set source parameter from file name\n"
     << "    --select_source <string>    -- copy only objects with given source\n"
     << "    --skip_source <string>      -- copy all objects but ones with given source\n"
     << "    (select_source and skip_source options are processed before set_source_*)\n"
     << "\n"
     << "    --select_type <int value>   -- copy only objects with given type\n"
     << "    --skip_type <int value>     -- copy all objects but ones with given type\n"
     << "    --skip_all                  -- don't read any objects, only labels\n"
     << "\n"
     << "    --range_datum <string>      -- set datum for range setting (wgs84, pulkovo, ...)\n"
     << "    --range_proj <string>       -- set proj for range setting (lonlat, tmerc, ...)\n"
     << "    --range_lon0 <double>       -- set lon0 for tmerc proj in range settings\n"
     << "    --range <geometry>          -- set range\n"
     << "                                   (prefix in x coord can override lon0)\n"
     << "    --range_nom <name>          -- set nomenclature range\n"
     << "                                   (overrides range, range_datum, range_proj)\n"
     << "    --range_action <action>     -- select actions to do with range (crop, skip, select)\n"
     << "\n"
     << "    -v, --verbose               -- be verbose\n"
     << "\n"
     << "  output options:\n"
     << "\n"
     << "    -a, --append                -- don't remove map from output file\n"
     << "    -n, --name <string>         -- set map name\n"
     << "    -i, --mp_id <int>           -- set mp id\n"
     << "    -m, --rscale <double>       -- set reversed scale (50000 for 1:50000 map)\n"
     << "    -s, --style <string>        -- set map style\n"
     << "\n"
     << "    --skip_labels               -- don't write labels\n"
     << "\n"
     << "    --set_source <string>       -- set source parameter\n"
     << "    --set_source_from_name      -- set source parameter from map name\n"
     << "    --set_source_from_fname     -- set source parameter from file name\n"
     << "    --select_source <string>    -- copy only objects with given source\n"
     << "    --skip_source <string>      -- copy all objects but ones with given source\n"
     << "    (select_source and skip_source options are processed before set_source_*)\n"
     << "\n"
     << "    --select_type <int>         -- copy only objects with given type\n"
     << "    --skip_typee <int>          -- copy all objects but ones with given type\n"
     << "    --skip_all                  -- don't read any objects, only labels\n"
  ;
  exit(1);
}

static struct option in_options[] = {
  {"skip_labels",           0, 0, 0},
  {"read_labels",           0, 0, 0},

  {"set_source",            1, 0, 0},
  {"set_source_from_name",  0, 0, 0},
  {"set_source_from_fname", 0, 0, 0},
  {"select_source",         1, 0, 0},
  {"skip_source",           1, 0, 0},
  {"select_type",           1, 0, 0},
  {"skip_type",             1, 0, 0},
  {"skip_all",              0, 0, 0},

  {"range_datum",           1, 0, 0},
  {"range_proj",            1, 0, 0},
  {"range_lon0",            1, 0, 0},
  {"range",                 1, 0, 0},
  {"range_nom",             1, 0, 0},
  {"range_action",          1, 0, 0},

  {"out",         0, 0 , 'o'},
  {"verbose",     0, 0 , 'v'},
  {0,0,0,0}
};

static struct option out_options[] = {
  {"skip_labels",           0, 0, 0},

  {"set_source",            1, 0 , 0},
  {"set_source_from_name",  0, 0 , 0},
  {"set_source_from_fname", 0, 0 , 0},
  {"select_source",         1, 0 , 0},
  {"skip_source",           1, 0 , 0},
  {"select_type",           1, 0 , 0},
  {"skip_type",             1, 0 , 0},
  {"skip_all",              0, 0 , 0},

  {"append",      0, 0 , 'a'},
  {"name",        1, 0 , 'n'},
  {"mp_id",       1, 0 , 'i'},
  {"rscale",      1, 0 , 'm'},
  {"style",       1, 0 , 's'},
  {0,0,0,0}
};


main(int argc, char **argv){

  if (argc==1) usage();

  Options O = parse_options(argc, argv, in_options, "out");
  Options GO(O); // global options
  argc-=optind;
  argv+=optind;
  optind=0;

  vmap::world V;

  while (!O.exists("out")) {
    if (argc<1){
      if (O.get<int>("verbose",0))
        cout << "no output files\n";
      exit(0);
    }
    const char * ifile = argv[0];

    // parse options for this file and append global options
    O = parse_options(argc, argv, in_options, "out");
    argc-=optind;
    argv+=optind;
    optind=0;
    O.insert(GO.begin(), GO.end());

    if (O.exists("set_source_from_fname"))
      O.put<string>("set_source", ifile);

    if (O.get<int>("verbose",0))
      cout << "reading: " << ifile  << "\n";

    vmap::world V1 = vmap::read(ifile);
    filter(V1, O);
    V.add(V1);
  }

  /***************** output ****************/

  if (argc<1){
    if (O.get<int>("verbose",0))
      cout << "no output files\n";
  }
  const char * ofile = argv[0];

  // parse output options
  O = parse_options(argc, argv, out_options);
  argc-=optind;
  argv+=optind;
  optind=0;

  /***************** write file ****************/

  if (O.exists("set_source_from_fname"))
    O.put<string>("set_source", ofile);

  if (GO.get<int>("verbose",0))
    cout << "writing to: " << ofile << "\n";

  // find labels for each object
  add_labels(V);
  // create new labels
  new_labels(V);

  filter(V, O);

  if (!vmap::write(ofile, V, O)) exit(1);

  exit(0);
}


