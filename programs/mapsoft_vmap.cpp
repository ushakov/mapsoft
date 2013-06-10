#include <string>
#include <cstring>
#include "options/m_getopt.h"
#include "vmap/vmap.h"
#include "fig/fig.h"

using namespace std;

#define OPT_INP  1  // input-only options
#define OPT_FLT  2  // filter options (used both as input and output)
#define OPT_OUT  4  // output-only options
#define OPT_STP  8  // special option -o/--out

#define MASK_INP  (OPT_INP | OPT_FLT | OPT_STP) // mask to select input options
#define MASK_OUT  (OPT_OUT | OPT_FLT | OPT_STP) // mask to select output options

static struct ext_option options[] = {
  {"out",                   0,'o', OPT_STP, ""},
  {"help",                  0,'h', OPT_INP, "show this message"},
  {"verbose",               0,'v', OPT_INP, "be verbose"},

  {"skip_labels",           0,  0, OPT_FLT, "don't read labels"},
  {"read_labels",           0,  0, OPT_FLT, "do read labels\n(used to override global --skip_labels)"},
  {"split_labels",          0,  0, OPT_FLT, "split labels from object (be careful when using --split_labels in write options: fig, mp, and ocad does not support writing of splitted labels yet)\n"},

  {"set_source",            1,  0, OPT_FLT, "set source parameter"},
  {"set_source_from_name",  0,  0, OPT_FLT, "set source parameter from map name"},
  {"set_source_from_fname", 0,  0, OPT_FLT, "set source parameter from file name"},
  {"select_source",         1,  0, OPT_FLT, "copy only objects with given source"},
  {"skip_source",           1,  0, OPT_FLT, "copy all objects but ones with given source (select_source and skip_source options are processed before set_source_*)\n"},

  {"select_type",           1,  0, OPT_FLT, "copy only objects with given type (int value)"},
  {"skip_type",             1,  0, OPT_FLT, "copy all objects but ones with given type"},
  {"skip_all",              0,  0, OPT_FLT, "don't read any objects, only labels\n"},

  {"range_datum",           1,  0, OPT_FLT, "set datum for range setting (wgs84, pulkovo, ...)"},
  {"range_proj",            1,  0, OPT_FLT, "set proj for range setting (lonlat, tmerc, ...)"},
  {"range_lon0",            1,  0, OPT_FLT, "set lon0 for tmerc proj in range settings"},
  {"range",                 1,  0, OPT_FLT, "set range geometry (prefix in x coord overrides lon0)"},
  {"range_nom",             1,  0, OPT_FLT, "set nomenclature range\n(overrides range, range_datum, range_proj)"},
  {"range_action",          1,  0, OPT_FLT, "select actions to do with range\n(crop, select, skip, crop_spl, help)\n"},

  {"set_brd_from_range",    0,  0, OPT_FLT, "set map border from range/range_nom options (border from last input only goes to the output)"},
  {"set_brd",               1,  0, OPT_FLT, "set map border from wgs84 points"},
  {"remove_tails",          1,  0, OPT_FLT, "remove object tails (lines which is close to border and to other object of the same type) (range must be set)\n"},

  {"name",                  1,'n', OPT_FLT, "set map name"},
  {"mp_id",                 1,'i', OPT_FLT, "set mp id"},
  {"rscale",                1,'m', OPT_FLT, "set reversed scale (50000 for 1:50000 map)"},
  {"style",                 1,'s', OPT_FLT, "set map style"},

  {"append",                0,'a', OPT_OUT, "don't remove map from output file"},
  {"keep_border",           0,  0, OPT_OUT, "(fig only) use border from the old file"},
  {"keep_labels",           0,  0, OPT_OUT, "(fig only) use labels from the old file\n"},

  {"remove_dups",           1,  0, OPT_OUT, "remove repeated points with given accuracy\n(before adding labels)"},
  {"remove_empty",          0,  0, OPT_OUT, "remove empty objects and lines (remove_tails and range_action do remove_empty) (before adding labels)"},
  {"join_objects",          1,  0, OPT_OUT, "join objects (before adding labels)"},
  {"join_labels",           1,  0, OPT_OUT, "join labels with objects"},
  {"add_labels",            1,  0, OPT_OUT, "create new labels"},
  {"move_pics",             1,  0, OPT_OUT, "move pics"},
  {"legend",                1,  0, OPT_OUT, "<style>, write list of all object types"},

  {0,0,0,0}
};


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
  ;

  cerr << "\nFilter Options (can be used as input and output options):\n";
  print_options(options, OPT_FLT, cerr);
  cerr << "\nInput Options:\n";
  print_options(options, OPT_INP, cerr);
  cerr << "\nOutput Options:\n";
  print_options(options, OPT_OUT, cerr);
  exit(1);
}



main(int argc, char **argv){
  try {

  if (argc==1) usage();
  Options O = parse_options(&argc, &argv, options, MASK_INP, "out");
  if (O.exists("help")) usage();

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
    O = parse_options(&argc, &argv, options, MASK_INP, "out");
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
  O = parse_options(&argc, &argv, options, MASK_OUT);

  /***************** write file ****************/

  if (O.exists("legend"))
    V = vmap::make_legend(O.get("legend", string()));

  if (O.exists("set_source_from_fname"))
    O.put<string>("set_source", ofile);

  // OPTION remove_dups
  // OPTION remove_empty
  // OPTION join_objects
  double remove_dups_acc=O.get("remove_dups", 0.0);
  if (remove_dups_acc>0) remove_dups(V, remove_dups_acc);

  double join_objects_acc=O.get("join_objects", 0.0);
  if (join_objects_acc>0) join_objects(V, join_objects_acc);

  // remove empty objects
  if (O.get<int>("remove_empty", 0)) remove_empty(V);

  // find labels for each object
  if (O.get<int>("join_labels", 1)) join_labels(V);

  // create new labels
  if (O.get<int>("create_labels", 1)) create_labels(V);

  // move and rotate pics
  if (O.get<int>("move_pics", 1)) move_pics(V);

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


