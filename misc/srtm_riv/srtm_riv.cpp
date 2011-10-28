#include <iostream>
#include "options/m_getopt.h"

using namespace std;

void usage(){
  const char * prog = "srtm_riv";

  cerr
     << prog << " -- trace rivers from srtm data.\n"
     << "  usage: " << prog << " <options> lat,lon\n"
     << "\n"
     << "  options:\n"
     << "\n"
     << "    --srtm_dir <dir>, -d <dir>\n"
     << "         directory with srtm (.hgt) files (default ~/.srtm_data)\n"
     << "\n"
  ;
}

static struct option options[] = {
  {"srtm_dir",  1, 0, 'd'},
  {0,0,0,0}
};

int main(int argc, char *argv[]) {
  try {
  if (argc==1) usage();
  Options O = parse_options(&argc, &argv, options);

  exit(0);
  } catch (const char *err){
    cerr << "ERROR: " << err << "\n";
    exit(1);
  }
}

