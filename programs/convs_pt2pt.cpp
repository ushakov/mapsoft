#include <sstream>
#include "geo/geo_convs.h"

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
       << "line_frw <line> <acc>\n"
       << "line_bck <line> <acc>\n"
       << "bb_frw <rect> <acc>\n"
       << "bb_bck <rect> <acc>\n"
       << "\n"
       << "<acc> is an accuracy of line conversions in source projection coordinates\n"
       << "\n"
  ;
}

int main(int argc, char** argv){
  if (argc != 7){
    usage();
    return 1;
  }
  pt2pt cnv(
    boost::lexical_cast<Datum>(argv[1]),
    boost::lexical_cast<Proj>(argv[2]),
    boost::lexical_cast<Options>(argv[3]),
    boost::lexical_cast<Datum>(argv[4]),
    boost::lexical_cast<Proj>(argv[5]),
    boost::lexical_cast<Options>(argv[6])
  );
  while (!cin.eof()){
    string str;
    getline(cin, str);
    istringstream sstr(str);

    string s;
    double acc;
    dPoint p;
    dLine l;
    dRect r;
    sstr >> s;
    if (s == "frw"){
      sstr >> p;
      cnv.frw(p);
      if (!sstr.eof()) cerr << "Error: frw\n";
      else             cout << fixed << p << "\n";
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
      sstr >> r >> acc;
      if (!sstr.eof()) cerr << "Error: bb_frw\n";
      else cout << fixed << cnv.bb_frw(r, acc) << "\n";
      continue;
    }
    if (s == "bb_bck"){
      sstr >> r >> acc;
      if (!sstr.eof()) cerr << "Error: bb_bck\n";
      else cout << fixed << cnv.bb_bck(r, acc) << "\n";
      continue;
    }
    if (s != "") cerr << "Error: unknown command\n";
  }
  return 0;
}

