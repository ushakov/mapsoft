#include <sstream>
#include "geo/geo_convs.h"
#include "geo_io/io.h"
#include "utils/err.h"
#include "utils/err.h"

// command line interface to convs::map2pt conversions

using namespace std;
using namespace convs;

void usage(){
  cerr << "\n"
       << "Command line interface to convs::map2pt conversions.\n"
       << "usage: echo <commands> | convs_map2pt  <map file>  <d> <p> <o>\n"
       << "d,p,o -- datums, projections, projection options\n"
       << "         for destination coordinates.\n"
       << "Commands:\n"
       << "frw <point>\n"
       << "bck <point>\n"
       << "line_frw <line> <acc>\n"
       << "line_bck <line> <acc>\n"
       << "bb_frw <rect>\n"
       << "bb_bck <rect>\n"
       << "\n"
       << "<acc> is an accuracy of line conversions in source projection coordinates\n"
       << "\n"
  ;
  exit(1);
}

int main(int argc, char** argv){
  if (argc != 5) usage();
  geo_data W;
  try {io::in(argv[1],W,Options());}
  catch (MapsoftErr e) {cerr << e.str() << endl;}

  g_map *map = NULL;
  int count = 0;
  for (vector<g_map_list>::iterator i=W.maps.begin();
       i!=W.maps.end(); i++){
    count+=i->size();
    if ((i->size()>0) && (map == NULL)) map = &(*i)[0];
  }

  if (map == NULL){
    cerr << "Can't find any map in " << argv[1] << "\n";
    exit(1);
  }
  if (count>1)
    cerr << W.maps.size() << " maps found. Using first one\n";

  map2pt cnv(
    *map,
    boost::lexical_cast<Datum>(argv[2]),
    boost::lexical_cast<Proj>(argv[3]),
    boost::lexical_cast<Options>(argv[4])
  );

  while (!cin.eof()){
    string str;
    getline(cin, str);
    istringstream sstr(str);

    string s;
    double acc;
    dPoint p;
    dLine  l;
    iRect  r;
    sstr >> s;
    if (s == "frw"){
      sstr >> p;
      cnv.frw(p);
      if (!sstr.eof()) cerr << "Error: frw\n";
      else cout << fixed << p << "\n";
      continue;
    }
    if (s == "bck"){
      sstr >> p;
      cnv.bck(p);
      if (!sstr.eof()) cerr << "Error: bck\n";
      else cout << fixed << p << "\n";
      continue;
    }
    if (s == "line_frw"){
      sstr >> l >> acc;
      if (!sstr.eof()) cerr << "Error: line_frw\n";
      else cout << fixed << cnv.line_frw(l, acc) << "\n";
      continue;
    }
    if (s == "line_bck"){
      sstr >> l >> acc;
      if (!sstr.eof()) cerr << "Error: line_bck\n";
      else cout << fixed << cnv.line_bck(l, acc) << "\n";
      continue;
    }
    if (s == "bb_frw"){
      sstr >> r;
      if (!sstr.eof()) cerr << "Error: bb_frw\n";
      else cout << fixed << cnv.bb_frw(r) << "\n";
      continue;
    }
    if (s == "bb_bck"){
      sstr >> r;
      if (!sstr.eof()) cerr << "Error: bb_bck\n";
      else cout << fixed << cnv.bb_bck(r) << "\n";
      continue;
    }
    if (s != "") cerr << "Error: unknown command\n";
  }
}
