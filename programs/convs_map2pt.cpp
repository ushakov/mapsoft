#include "../core/libgeo/geo_convs.h"
#include "../core/libgeo_io/io.h"

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
       << "safe_frw <point>\n"
       << "safe_bck <point>\n"
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
  io::in(argv[1],W,Options());
  if (W.maps.size()<1){
    cerr << "Can't find any map in " << argv[1] << "\n";
    exit(1);
  }
  if (W.maps.size()>1)
    cerr << W.maps.size() << " maps found. Using first one\n";

  map2pt cnv(
    W.maps[0],
    boost::lexical_cast<Datum>(argv[2]),
    boost::lexical_cast<Proj>(argv[3]),
    boost::lexical_cast<Options>(argv[4])
  );

  while (!cin.eof()){
    string s;
    double acc;
    dPoint p;
    dLine  l;
    iRect  r;
    cin >> s;
    if (s == "frw"){
      cin >> p;
      cnv.frw(p);
      cout << fixed << p << "\n";
      continue;
    }
    if (s == "bck"){
      cin >> p;
      cnv.bck(p);
      cout << fixed << p << "\n";
      continue;
    }
    if (s == "frw_safe"){
      cin >> p;
      cnv.frw_safe(p);
      cout << fixed << p << "\n";
      continue;
    }
    if (s == "bck_safe"){
      cin >> p;
      cnv.bck(p);
      cout << fixed << p << "\n";
      continue;
    }
    if (s == "line_frw"){
      cin >> l >> acc;
      cout << fixed << cnv.line_frw(l, acc) << "\n";
      continue;
    }
    if (s == "line_bck"){
      cin >> l >> acc;
      cout << fixed << cnv.line_bck(l, acc) << "\n";
      continue;
    }
    if (s == "bb_frw"){
      cin >> r;
      cout << fixed << cnv.bb_frw(r) << "\n";
      continue;
    }
    if (s == "bb_bck"){
      cin >> r;
      cout << fixed << cnv.bb_bck(r) << "\n";
      continue;
    }
  }
}