#include "options/m_getopt.h"
#include "geo_io/geofig.h"
#include "geo_io/geo_refs.h"
#include "geo_io/io.h"
#include "utils/err.h"

using namespace std;

#define OPT_CRE 1
#define OPT_ADD 2
#define OPT_DEL 4

static struct ext_option options[] = {
  {"geom",           1, 'g', OPT_CRE, ""},
  {"datum",          1, 'd', OPT_CRE, ""},
  {"proj",           1, 'p', OPT_CRE, ""},
  {"lon0",           1, 'l', OPT_CRE, ""},
  {"wgs_geom",       1, 'w', OPT_CRE, ""},
  {"wgs_brd",        1, 'b', OPT_CRE, ""},
  {"trk_brd",        1, 'b', OPT_CRE, ""},
  {"nom",            1, 'N', OPT_CRE, ""},
  {"google",         1, 'G', OPT_CRE, ""},
  {"rscale",         1, 'r', OPT_CRE, ""},
  {"mag",            1, 'm', OPT_CRE, ""},
  {"swap_y",         0, 'y', OPT_CRE, "\n"},

  {"verbose",        0, 'v', OPT_CRE | OPT_ADD | OPT_DEL, "be more verbose"},
  {"out",            1, 'o', OPT_CRE | OPT_ADD | OPT_DEL, "set output file name"},
  {"help",           0, 'h', OPT_CRE | OPT_ADD | OPT_DEL, "show help message"},
  {"pod",            0,  0, OPT_CRE | OPT_ADD | OPT_DEL, "show help message as pod template"},

  {"wpts",           0, 'w', OPT_DEL, "delete waypoints"},
  {"trks",           0, 't', OPT_DEL, "delete tracks"},
  {"maps",           0, 'm', OPT_DEL, "delete maps"},
  {"brds",           0, 'b', OPT_DEL, "delete map borders"},
  {"ref",            0, 'r', OPT_DEL, "delete fig reference"},
  {0,0,0,0}
};

void usage(bool pod=false){
  string head = pod? "\n=head1 ":"\n";
  const char * prog = "mapsoft_geofig";

  cerr
     << prog << " -- actions with geo-referenced FIG\n"
     << head << "Usage:\n"
     << "\t" << prog << " create [<options>]  (--out|-o) <output_file>\n"
     << "\t" << prog << " add [<options>] <file1> ... <fileN>  (--out|-o) <output_file>\n"
     << "\t" << prog << " del [<options>] (--out|-o) <output_file>\n"
  ;
  cerr << head << "Options for create action:\n";
  print_options(options, OPT_CRE, cerr, pod);
  cerr << head << "Options for add action:\n";
  print_options(options, OPT_ADD, cerr, pod);
  cerr << head << "Options for del action:\n";
  print_options(options, OPT_DEL, cerr, pod);

  exit(1);
}


main(int argc, char **argv){
  try {

    if (argc<3) usage();
    string action = argv[1];
    argv++; argc--;

    /*****************************************/
    if (action == "create"){
      Options O = parse_options(&argc, &argv, options, OPT_CRE);
      if (O.exists("help")) usage();
      if (O.exists("pod")) usage(true);
      if (argc>0){
        cerr << "error: wrong argument: " << argv[0] << endl;
        exit(1);
      }
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
      vector<string> infiles;
      Options O = parse_options_all(&argc, &argv, options, OPT_ADD, infiles);
      if (O.exists("help")) usage();
      if (O.exists("pod")) usage(true);
      if (!O.exists("out")){
        cerr << "no output files.\n";
        exit(1);
      }

      if (O.exists("verbose")) cerr << "Reading data...\n";
      geo_data world;
      for (vector<string>::const_iterator i = infiles.begin(); i!=infiles.end(); i++){
        try {io::in(*i, world, O);}
        catch (MapsoftErr e) {cerr << e.str() << endl;}
      }

      if (O.exists("verbose")){
        cerr << ",  Waypoint lists: " << world.wpts.size()
             << ",  Tracks: " << world.trks.size() << "\n";
      }
      string ofile = O.get<string>("out");
      if (O.exists("verbose")) cerr << "Writing data to " << ofile << "\n";

      fig::fig_world F;
      if (!fig::read(ofile.c_str(), F)) {
        cerr << "error: can't read file: " << ofile << endl;
        exit(1);
      }
      g_map ref = fig::get_ref(F);
      put_wpts(F, ref, world);
      put_trks(F, ref, world);

      exit (!fig::write(ofile.c_str(), F));
    }
    /*****************************************/
    else if (action == "del"){
      Options O = parse_options(&argc, &argv, options, OPT_DEL);
      if (O.exists("help")) usage();
      if (O.exists("pod")) usage(true);
      if (argc>0){
        cerr << "error: wrong argument: " << argv[0] << endl;
        exit(1);
      }
      if (!O.exists("out")){
        cerr << "error: no output files" << endl;
        exit(1);
      }
      string ofile = O.get<string>("out");

      fig::fig_world F;
      if (!fig::read(ofile.c_str(), F)) {
        cerr << "error: can't read file: " << ofile << endl;
        exit(1);
      }
      if (O.exists("wpts")) rem_wpts(F);
      if (O.exists("trks")) rem_trks(F);
      if (O.exists("maps")) rem_maps(F);
      if (O.exists("brds")) rem_brds(F);
      if (O.exists("ref"))  rem_ref(F);
      exit (!fig::write(ofile.c_str(), F));
    }
    /*****************************************/
    else usage();

  } catch (const char *err){
    cerr << "Error: " << err << endl;
    exit(1);
  }
}


