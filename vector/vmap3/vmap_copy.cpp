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
     << "  usage: " << prog << " [<options>] <in_files> ... <out_file>\n"
     << "  optins:\n"
     << "    -n, --name <string value>      -- change map name\n"
     << "    -i, --mp_id <int value>        -- change mp id\n"
     << "    -m, --rscale <double value>    -- change reversed scale (50000 for 1:50000 map)\n"
     << "    -s, --style <string value>     -- change map style\n"
     << "    --skip_labels                  -- don't write labels\n"
     << "    --set_source <string value>    -- set source parameter\n"
     << "    --set_source_from_name         -- set source parameter from map name\n"
     << "    --set_source_from_fname        -- set source parameter from file name\n"
     << "    --select_source <string value> -- copy only objects with given source\n"
     << "    --skip_source <string value>   -- copy all objects but ones with given source\n"
     << "    (select_source and skip_source options are processed before set_source_*)\n"
  ;
  exit(1);
}

main(int argc, char **argv){

  /***************** parse command line options ****************/
  Options O;
  int c;
  while(1){
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    static struct option long_options[] = {
      {"skip_labels", 0, 0 , 0},
      {"set_source",            1, 0 , 0},
      {"set_source_from_name",  0, 0 , 0},
      {"set_source_from_fname", 0, 0 , 0},
      {"select_source",         1, 0 , 0},
      {"skip_source",           1, 0 , 0},
      {"append",      0, 0 , 'a'},
      {"name",        1, 0 , 'n'},
      {"mp_id",       1, 0 , 'i'},
      {"rscale",      1, 0 , 'm'},
      {"style",       1, 0 , 's'},
      {0,0,0,0}
    };
    c = getopt_long(argc, argv, "an:i:m:s:", long_options, &option_index);
    if (c == -1) break;
    switch (c) {
      case 'a': O.put<string>("append", "1");     break;
      case 'n': O.put<string>("name",   optarg);  break;
      case 'i': O.put<string>("mp_id",  optarg);  break;
      case 'm': O.put<string>("rscale", optarg);  break;
      case 's': O.put<string>("style",  optarg);  break;
      case 0:
        if (strcmp(long_options[option_index].name, "skip_labels")==0)
          O.put<string>("skip_labels", "1");
        if (strcmp(long_options[option_index].name, "set_source")==0)
          O.put<string>("set_source", optarg);
        if (strcmp(long_options[option_index].name, "set_source_from_name")==0)
          O.put<string>("set_source_from_name", "1");
        if (strcmp(long_options[option_index].name, "set_source_from_fname")==0)
          O.put<string>("set_source_from_fname", "1");
        if (strcmp(long_options[option_index].name, "select_source")==0)
          O.put<string>("select_source", optarg);
        if (strcmp(long_options[option_index].name, "skip_source")==0)
          O.put<string>("skip_source", optarg);
        break;
      default:
        usage();
    }
  }

  /***************** read files ****************/

  argc-=optind;
  argv+=optind;
  if (argc<2) usage();

  vmap::world V;
  for (int i=0; i<argc-1; i++){
    if (O.exists("set_source_from_fname"))
      O.put<string>("set_source", argv[i]);
    if (!V.get(argv[i], O)) exit(1);
  }

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

  string out = O.get<string>("out");
  if (!V.put(argv[argc-1], O)) exit(1);

  exit(0);
}


