#include <string>
#include <cstring>
#include "options/m_getopt.h"
#include "vmap/vmap.h"
#include "fig/fig.h"

using namespace std;

void usage(){
  const char * prog = "mapsoft_vmap";

  cerr
     << prog << " -- convert vector maps in mp, fig, vmap or ocad formats.\n"
     << "  usage: " << prog << " [<global_input_options>]\\\n"
     << "         <input_file_1> [<input_options_1>] ... \\\n"
     << "         (--out|-o) <output_file> [<output_options>]\n"
     << "\n"
     << "Every input file is read, filtered according to input options\n"
     << "and appended to previous ones. After all files have beed read labels\n"
     << "are attached to map objects. After that output filtering is applied\n"
     << "and map is wrote to the output file.\n"
     << "\n"
     << "  input options:\n"
     << "    -v, --verbose               -- be verbose\n"
     << "    <any filter options>\n"
     << "\n"
     << "  output options:\n"
     << "\n"
     << "    -a, --append                -- don't remove map from output file\n"
     << "        --keep border           -- (fig only) use border from the old file\n"
     << "        --keep labels           -- (fig only) use labels from the old file\n"
     << "    <any filter options>\n"
     << "\n"
     << "  filter options:\n"
     << "\n"
     << "    --skip_labels               -- don't read labels\n"
     << "    --read_labels               -- do read labels\n"
     << "      (--read_labels options is intendent to override global --skip_labels option)\n"
     << "      (if you skip labels in input options - they will be created before output)\n"
     << "    --split_labels              -- split labels from object\n"
     << "      (be careful when using --split_labels in write options: fig, mp, and ocad\n"
     << "       does not support writing of splitted labels yet)\n"
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
     << "    --range_action <action>     -- select actions to do with range\n"
     << "                                   (crop, select, skip, crop_spl, help)\n"
     << "    --set_brd_from_range        -- set map border from range/range_nom options\n"
     << "                        (border from last input only goes to the output)\n"
     << "    --set_brd <line>            -- set map border from wgs84 points\n"
     << "    --remove_tails <dist>       -- remove object tails (lines which is close to\n"
     << "                                   border and to other object of the same type)\n"
     << "                                   (range must be set)\n"
     << "\n"
     << " these options applyed before adding labels:\n"
     << "    --remove_dups <accuracy>    -- remove repeated points\n"
     << "    --remove_empty              -- remove empty objects and lines\n"
     << "                                   (remove_tails and range_action do remove_empty)\n"
     << "    --join_objects <accuracy>   -- join objects\n"
     << "\n"
     << "    -n, --name <string>         -- set map name\n"
     << "    -i, --mp_id <int>           -- set mp id\n"
     << "    -m, --rscale <double>       -- set reversed scale (50000 for 1:50000 map)\n"
     << "    -s, --style <string>        -- set map style\n"
     << "                        (values from last input only goes to the output)\n"
     << "\n"
  ;
  exit(1);
}

static struct option in_options[] = {
  {"skip_labels",           0, 0, 0},
  {"read_labels",           0, 0, 0},
  {"split_labels",          0, 0 , 0},

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
  {"set_brd_from_range",    0, 0, 0},
  {"set_brd",               1, 0, 0},
  {"remove_tails",          1, 0, 0},

  {"name",        1, 0 , 'n'},
  {"mp_id",       1, 0 , 'i'},
  {"rscale",      1, 0 , 'm'},
  {"style",       1, 0 , 's'},

  {"out",         0, 0 , 'o'},
  {"verbose",     0, 0 , 'v'},
  {0,0,0,0}
};

static struct option out_options[] = {
  {"skip_labels",           0, 0, 0},
  {"read_labels",           0, 0, 0},
  {"split_labels",          0, 0 , 0},

  {"set_source",            1, 0 , 0},
  {"set_source_from_name",  0, 0 , 0},
  {"set_source_from_fname", 0, 0 , 0},
  {"select_source",         1, 0 , 0},
  {"skip_source",           1, 0 , 0},
  {"select_type",           1, 0 , 0},
  {"skip_type",             1, 0 , 0},
  {"skip_all",              0, 0 , 0},

  {"range_datum",           1, 0, 0},
  {"range_proj",            1, 0, 0},
  {"range_lon0",            1, 0, 0},
  {"range",                 1, 0, 0},
  {"range_nom",             1, 0, 0},
  {"range_action",          1, 0, 0},
  {"set_brd_from_range",    0, 0, 0},
  {"set_brd",               1, 0, 0},
  {"remove_tails",          1, 0, 0},
  {"remove_dups",           1, 0, 0},
  {"remove_empty",          0, 0, 0},
  {"join_objects",          1, 0, 0},

  {"name",        1, 0 , 'n'},
  {"mp_id",       1, 0 , 'i'},
  {"rscale",      1, 0 , 'm'},
  {"style",       1, 0 , 's'},

  {"append",      0, 0 , 'a'},
  {"keep_border", 0, 0 , 0},
  {"keep_labels", 0, 0 , 0},
  {0,0,0,0}
};


main(int argc, char **argv){
  try {

  if (argc==1) usage();

  Options O = parse_options(&argc, &argv, in_options, "out");
  Options GO(O); // global options

  vmap::world V;

  while (!O.exists("out")) {
    if (argc<1){
      if (O.get<int>("verbose",0))
        cout << "no output files\n";
      exit(0);
    }
    const char * ifile = argv[0];

    // parse options for this file and append global options
    O = parse_options(&argc, &argv, in_options, "out");
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

  const char * ofile = NULL;
  if (argc<1){
    if (O.get<int>("verbose",0))
      cout << "no output files\n";
  }
  else ofile = argv[0];

  // parse output options
  O = parse_options(&argc, &argv, out_options);

  /***************** write file ****************/

  if (O.exists("set_source_from_fname"))
    O.put<string>("set_source", ofile);

  // OPTION remove_dups
  // OPTION remove_empty
  // OPTION join_objects
  double remove_dups_acc=O.get("remove_dups", 0.0);
  if (remove_dups_acc>0) remove_dups(V, remove_dups_acc);

  double join_objects_acc=O.get("join_objects", 0.0);
  if (join_objects_acc>0) join_objects(V, join_objects_acc);

  int do_remove_empty=O.get("remove_empty", 0);
  if (do_remove_empty) remove_empty(V);

  // find labels for each object
  join_labels(V);
  // create new labels
  create_labels(V);
  // move and rotate pics
  move_pics(V);

  filter(V, O);

  if (ofile){
    if (GO.get<int>("verbose",0))
      cout << "writing to: " << ofile << "\n";
    if (!vmap::write(ofile, V, O)) exit(1);
  }

  exit(0);
  } catch (const char *err){
    cerr << "ERROR: " << err << "\n";
    exit(1);
  }
}

