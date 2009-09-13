#include "../core/libgeo/geo_convs.h"

// command line interface to convs::pt2pt conversions

using namespace std;
using namespace convs;

void usage(){
  cerr << "\n"
       << "Command line interface to convs::pt2pt conversions.\n"
       << "usage: echo <commands> | convs_pt2pt  <d1> <p1> <o1>  <d2> <p2> <o2>\n"
       << "d1,p1,o1, d2,p2,o2 -- datums, projections, projection options\n"
       << "                      for source and destination coordinates.\n"
       << "Commands:\n"
       << "frw <point>\n"
       << "bck <point>\n"
       << "safe_frw <point>\n"
       << "safe_bck <point>\n"
       << "line_frw <line> <acc>\n"
       << "line_bck <line> <acc>\n"
       << "bb_frw <rect> <acc>\n"
       << "bb_bck <rect> <acc>\n"
       << "\n"
       << "<acc> is an accuracy of line conversions in source projection coordinates\n"
       << "\n"
  ;
  exit(1);
}

int main(int argc, char** argv){
  if (argc != 7) usage();
  pt2pt cnv(
    boost::lexical_cast<Datum>(argv[1]),
    boost::lexical_cast<Proj>(argv[2]),
    boost::lexical_cast<Options>(argv[3]),
    boost::lexical_cast<Datum>(argv[4]),
    boost::lexical_cast<Proj>(argv[5]),
    boost::lexical_cast<Options>(argv[6])
  );
  while (!cin.eof()){
    string s;
    double acc;
    g_point p;
    g_line l;
    g_rect r;
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
      cin >> r >> acc;
      cout << fixed << cnv.bb_frw(r, acc) << "\n";
      continue;
    }
    if (s == "bb_bck"){
      cin >> r >> acc;
      cout << fixed << cnv.bb_bck(r, acc) << "\n";
      continue;
    }
  }
}
