#include <string>
#include <cstring>
#include <getopt.h>
#include "vmap.h"
#include "libfig/fig.h"

using namespace std;

bool testext(const string & nstr, const char *ext){
    int pos = nstr.rfind(ext);
    return ((pos>0)&&(pos == nstr.length()-strlen(ext)));
}

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
     << "    --set_source <string value>    -- set source parameter\n"
     << "    --set_source_from_name         -- set source parameter from map name\n"
     << "    --set_source_from_fname        -- set source parameter from file name\n"
     << "    --select_source <string value> -- copy only objects with given source\n"
     << "    --skip_source <string value>   -- copy all objects but ones with given source\n"
     << "    (select_source and skip_source options are processed before set_source_*)\n"
     << "\n"
     << "    --select_type <int value>      -- copy only objects with given type\n"
     << "    --skip_typee <int value>       -- copy all objects but ones with given type\n"
     << "    --skip_all                     -- don't read any objects, only labels\n"
     << "\n"
     << "    --skip_range <geometry>        -- skip objects touching range\n"
     << "    --select_range <geometry>      -- select objects touching range\n"
     << "    --crop_range <geometry>        -- crop objects to the range\n"
     << "    --skip_nom <string>            -- \n"
     << "    --select_nom <string>          -- \n"
     << "    --crop_nom <string>            -- \n"
     << "    --range_datum <string>         -- set datum for range settings (wgs84, pulkovo)\n"
     << "    --range_proj <string>          -- set proj for range settings (lonlat, tmerc)\n"
     << "    --range_lon0 <double>          -- set lon0tmerc proj in range settings\n"
     << "    (lon0 can be overriden by prefix in range x coord)\n"
     << "\n"
     << "    -v, --verbose                  -- be verbose (works only in global options)\n"

    // OPTION skip_range
    // OPTION select_range
    // OPTION crop_range
    // OPTION skip_nom
    // OPTION select_nom
    // OPTION crop_nom
    // OPTION range_datum lonlat
    // OPTION range_proj  wgs84
    // OPTION range_lon0  0

     << "\n"
     << "  output options:\n"
     << "\n"
     << "    -a, --append                   -- don't remove map from output file\n"
     << "    -n, --name <string value>      -- set map name\n"
     << "    -i, --mp_id <int value>        -- set mp id\n"
     << "    -m, --rscale <double value>    -- set map reversed scale (50000 for 1:50000 map)\n"
     << "    -s, --style <string value>     -- set map style\n"
     << "\n"
     << "    --skip_labels                  -- don't write labels\n"
     << "\n"
     << "    --set_source <string value>    -- set source parameter\n"
     << "    --set_source_from_name         -- set source parameter from map name\n"
     << "    --set_source_from_fname        -- set source parameter from file name\n"
     << "    --select_source <string value> -- copy only objects with given source\n"
     << "    --skip_source <string value>   -- copy all objects but ones with given source\n"
     << "    (select_source and skip_source options are processed before set_source_*)\n"
     << "\n"
     << "    --select_type <int value>      -- copy only objects with given type\n"
     << "    --skip_typee <int value>       -- copy all objects but ones with given type\n"
     << "    --skip_all                     -- don't read any objects, only labels\n"
  ;
  exit(1);
}

Options parse_in_options(int argc, char **argv){
  Options O;
  int c;
  while(1){
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    static struct option long_options[] = {
      {"set_source",            1, 0, 0},
      {"set_source_from_name",  0, 0, 0},
      {"set_source_from_fname", 0, 0, 0},
      {"select_source",         1, 0, 0},
      {"skip_source",           1, 0, 0},
      {"select_type",           1, 0, 0},
      {"skip_type",             1, 0, 0},
      {"skip_all",              0, 0, 0},

      {"skip_range",            1, 0, 0},
      {"select_range",          1, 0, 0},
      {"crop_range",            1, 0, 0},
      {"skip_nom",              1, 0, 0},
      {"select_nom",            1, 0, 0},
      {"crop_nom",              1, 0, 0},
      {"range_datum",           1, 0, 0},
      {"range_proj",            1, 0, 0},
      {"range_lon0",            1, 0, 0},

      {"out",         0, 0 , 'o'},
      {"verbose",     0, 0 , 'v'},
      {0,0,0,0}
    };
    c = getopt_long(argc, argv, "+ov", long_options, &option_index); // note "+" in option list
    if (c == -1) break;

    if (O.exists("out")){
      cerr << "error: wrong position of --out special option\n";
      exit(1);
    }

    if (c!=0){ // short option -- we must manually set option_index
      int i = 0;
      while (long_options[i].name){
        if (long_options[i].val == c) option_index = i;
        i++;
      }
      if (!long_options[option_index].name){
        std::cerr << "error: bad option\n";
        exit(1);
      }
    }
    O.put<string>(long_options[option_index].name,
      long_options[option_index].has_arg? optarg:"1");
  }
  return O;
}

Options parse_out_options(int argc, char **argv){
  Options O;
  int c;
  while(1){
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    static struct option long_options[] = {
      {"skip_labels",  0, 0 , 0},

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
      {"verbose",     0, 0 , 'v'},
      {0,0,0,0}
    };
    c = getopt_long(argc, argv, "+an:i:m:s:v", long_options, &option_index); // note "+" in option list
    if (c == -1) break;
    if (c!=0){ // short option -- we must manually set option_index
      int i = 0;
      while (long_options[i].name){
        if (long_options[i].val == c) option_index = i;
        i++;
      }
      if (!long_options[option_index].name){
        std::cerr << "error: bad option\n";
        exit(1);
      }
    }
    O.put<string>(long_options[option_index].name,
      long_options[option_index].has_arg? optarg:"1");
  }
  return O;
}


main(int argc, char **argv){

  Options O;
  Options GO = parse_in_options(argc, argv); // read global options
  argc-=optind;
  argv+=optind;
  optind=0;

  if (GO.exists("out")){
      std::cerr << "error: no input files\n";
      exit(1);
  }
  vmap::world V;

  do {
    if (argc<1){
      std::cerr << "error: no output files\n";
      exit(1);
    }
    const char * ifile = argv[0];

    // parse options for this file and append global options
    O = parse_in_options(argc, argv);
    argc-=optind;
    argv+=optind;
    optind=0;
    O.insert(GO.begin(), GO.end());

    if (O.exists("set_source_from_fname"))
      O.put<string>("set_source", ifile);

    if (GO.get<int>("verbose",0)){
      cerr << "reading " << ifile << "\n";
      cerr << "  with options " << O << "\n";
    }
    if (!V.get(ifile, O)) exit(1);
  }
  while (!O.exists("out"));


  /***************** output ****************/

  if (argc<1){
    std::cerr << "no output files\n";
    exit(1);
  }
  const char * ofile = argv[0];

  // parse output options
  O = parse_out_options(argc, argv);
  argc-=optind;
  argv+=optind;
  optind=0;

  /***************** modify vmap ****************/

  if (O.exists("name")) V.name = O.get<string>("name");

  if (O.exists("mp_id")){
    int mp_id = O.get<int>("mp_id", 0);
    if (mp_id<= 0){
      cerr << "error: bad mp_id value: " << O.get<string>("mp_id") << "\n";
      exit(1);
    }
    V.mp_id  = mp_id;
  }

  if (O.exists("rscale")){
    double rscale = O.get<double>("rscale", 0.0);
    if (rscale <= 0){
      cerr << "error: bad rscale value: " << O.get<string>("rscale") << "\n";
      exit(1);
    }
    V.rscale = rscale;
  }

  if (O.exists("style")){
    string style = O.get<string>("style");
    if (style == ""){
      cerr << "error: empty style value\n";
      exit(1);
    }
    V.style  = style;
  }

  /***************** write file ****************/

  if (O.exists("set_source_from_fname"))
    O.put<string>("set_source", ofile);

  if (GO.get<int>("verbose",0)){
    std::cerr << "writing to " << ofile << "\n";
      cerr << "  with options " << O << "\n";
  }
  if (!V.put(ofile, O)) exit(1);

  exit(0);
}


