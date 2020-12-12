#include <list>
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib> // for atoi

#include "2d/line_utils.h"
#include "vmap/zn.h"
#include "vmap.h"

#define CUR_VER 3.2

namespace vmap {

const int point_scale = 1000000;

using namespace std;

/***************************************/

void
point_read_cnv(dPoint &p){
  p.x = p.x/point_scale;
  p.y = p.y/point_scale;
}
void
point_write_cnv(dPoint &p){
  p.x = round(p.x*point_scale);
  p.y = round(p.y*point_scale);
}

// read key-value pair separated by tab char
int get_kv(const string &s, string &key, string &val){
  if (s=="") return 1;
  int tab=s.find('\t', 0);
  if (tab==-1){
    cerr << "skipping bad line: " << s << "\n";
    return 1;
  }
  int st=0;
  while (s[st]==' ') st++;
  key=s.substr(st, tab-st);
  val=s.substr(tab+1, -1);
  return 0;
}

// Read label parameters: align, horizontality or angle, font size
// v<3.1:  <align:0|1|2> <hor:0|1> <ang>
// v>=3.1: L|C|R H|<ang>
// v>=3.2: L|C|R H|<ang> S<fsize>

void get_l_pars(istream & IN, lpos & l, double ver){
  if (ver<3.1){
    IN >> l.dir >> l.hor >> l.ang;
  }
  else { // version 3.1...
    char c;
    IN >> c;
    switch (c){
      case 0: case 'l': case 'L': l.dir=0; break;
      case 1: case 'c': case 'C': l.dir=1; break;
      case 2: case 'r': case 'R': l.dir=2; break;
    }
    string s;
    IN >> s;
    if (s.size() && (s[0]=='H' || s[0]=='h')){
      l.hor=1;
      l.ang=0.0;
    }
    else{
      l.hor=0;
      l.ang=atof(s.c_str());
    }
    IN >> s; // version 3.2
    if (s.size() && s[0]=='S')  l.fsize = atoi(s.c_str()+1);
    else l.fsize = 0;
  }
}

lpos get_lpos(const string & s, double ver){
  lpos ret;
  istringstream IN1(s);
  IN1 >> ret.pos;
  get_l_pars(IN1, ret, ver);
  point_read_cnv(ret.pos);
  return ret;
}

lpos_full get_lbuf(const string & s, double ver){
  lpos_full ret;
  istringstream IN1(s);
  IN1 >> ret.pos;
  get_l_pars(IN1, ret, ver);
  IN1 >> ret.ref;
  getline(IN1, ret.text);
  point_read_cnv(ret.pos);
  point_read_cnv(ret.ref);
  return ret;
}

dLine
read_points(istream & IN, string & s){
  dLine ret;
  string key,val;
  if (get_kv(s, key, val)!=0){
    cerr << "wrong call of read_points()!\n";
    return ret;
  }
  s=val;
  do {
    dPoint p;
    istringstream IN1(s);
    while (IN1.good()){
      IN1 >> p;
      point_read_cnv(p);
      ret.push_back(p);
    }
    getline(IN, s);
  } while (s[0]=='\t');

  return ret;
}


object
read_object(istream & IN, string & s, double ver){
  object ret;
  string key,val;
  bool read_ahead=false;

  if ((get_kv(s, key, val)!=0) || (key!="OBJECT")){
    cerr << "wrong call of read_object()!\n";
    return ret;
  }

  istringstream IN1(val);
  IN1 >> setbase(16) >> ret.type >> ws;
  getline(IN1,ret.text);

  while (!IN.eof() || read_ahead){
    if (!read_ahead) getline(IN, s);
    else read_ahead=false;
    if (get_kv(s, key, val)!=0) continue;

    if (ver<3.1 && key=="TEXT"){  // backward comp
      ret.text=val;
      continue;
    }
    if (key=="DIR"){
      ret.dir=atoi(val.c_str());
      continue;
    }
    if (key=="OPT"){
      string k,v;
      if (get_kv(val, k, v)!=0){
        cerr << "bad options in: " << s << "\n";
        continue;
      }
      ret.opts.put(k,v);
      continue;
    }
    if (key=="COMM"){
      ret.comm.push_back(val);
      continue;
    }
    if (key=="LABEL"){
      ret.labels.push_back(get_lpos(val, ver));
      continue;
    }
    if (key=="DATA"){
      ret.push_back(read_points(IN, s));
      read_ahead=true;
      continue;
    }
    break; // end of object
  }
  return ret;
}

// read vmap native format
world
read(istream & IN){
  world ret;
  string s, key, val;
  bool read_ahead=false;

  double ver;
  IN >> s >> ver;
  if (s!="VMAP"){
    cerr << "error: not a VMAP file\n";
    return ret;
  }
  if (ver>CUR_VER){
    cerr << "error: Too new VMAP format. Update mapsoft package.\n";
    return ret;
  }
  if (ver<CUR_VER){
    cerr << "note: reading old VMAP format version: "
         << fixed << setprecision(1) << ver << " < " << CUR_VER << "\n";
  }

  while (!IN.eof() || read_ahead){
    if (!read_ahead) getline(IN, s);
    else read_ahead=false;
    if (get_kv(s, key, val)!=0) continue;

    if (key=="NAME"){
      ret.name=val;
      continue;
    }
    if (key=="RSCALE"){
      ret.rscale=atof(val.c_str());
      continue;
    }
    if (key=="STYLE"){
      ret.style=val;
      continue;
    }
    if (key=="MP_ID"){
      ret.mp_id=atoi(val.c_str());
      continue;
    }
    if (key=="BRD"){
      ret.brd = read_points(IN, s);
      read_ahead=true;
      continue;
    }
    if (key=="LBUF"){
      ret.lbuf.push_back(get_lbuf(val, ver));
      continue;
    }
    if (key=="OBJECT"){
      ret.push_back(read_object(IN, s, ver));
      read_ahead=true;
      continue;
    }
    cerr << "skipping bad line: " << s << "\n";
  }
  return ret;
}

/***************************************/


void print_line(ostream & OUT, const dLine & L){
  int n=0;
  for (dLine::const_iterator i=L.begin(); i!=L.end(); i++){
    if ((n>0)&&(n%4==0)) OUT << "\n\t";
    else if (n!=0) OUT << " ";
    OUT << iPoint(*i);
    n++;
  }
}
// write label position
void print_lpos(ostream & OUT, const lpos & L){
  // coordinates
  OUT << iPoint(L.pos) << " ";
  // alignment (left,right,center)
  switch (L.dir){
    case 0: OUT << 'L'; break;
    case 1: OUT << 'C'; break;
    case 2: OUT << 'R'; break;
  }
  // angle (or H for horizontal labels)
  if (L.hor) OUT << " H";
  else  OUT << " " << setprecision(2) << round(L.ang*100)/100;

  // font size correction
  if (L.fsize) OUT << " S" << L.fsize;
}


// write vmap to ostream
// put vmap to mp
int
write(ostream & OUT, const world & W){

  world WS (W);

  // Sort problem: we write rounded values, so order can change!
  // Rounding values before sort:

  for (list<lpos_full>::iterator l=WS.lbuf.begin(); l!=WS.lbuf.end(); l++){
    point_write_cnv(l->pos);
    point_write_cnv(l->ref);
  }
  for (list<object>::iterator o=WS.begin(); o!=WS.end(); o++){
    for (dMultiLine::iterator l=o->begin(); l!=o->end(); l++){
      for (dLine::iterator p=l->begin(); p!=l->end(); p++){
        point_write_cnv(*p);
      }
    }
    for (list<lpos>::iterator l=o->labels.begin(); l!=o->labels.end(); l++){
      point_write_cnv(l->pos);
    }
  }
  for (dLine::iterator p=WS.brd.begin(); p!=WS.brd.end(); p++){
    point_write_cnv(*p);
  }

  WS.sort();
  WS.lbuf.sort();

  OUT << "VMAP " << fixed << setprecision(1) << CUR_VER << "\n";
  if (WS.name!="") OUT << "NAME\t" << WS.name << "\n";
  OUT << "RSCALE\t" << int(WS.rscale) << "\n";
  OUT << "STYLE\t" << WS.style << "\n";
  OUT << "MP_ID\t" << WS.mp_id << "\n";
  if (WS.brd.size()>0){
    OUT << "BRD\t";  print_line(OUT, WS.brd); OUT << "\n";
  }

  // lbuf
  for (list<lpos_full>::const_iterator l=WS.lbuf.begin(); l!=WS.lbuf.end(); l++){
    OUT << "LBUF\t"; print_lpos(OUT, *l);
    OUT << " " << iPoint(l->ref);
    if (l->text.size()) OUT << " " << l->text;
    OUT << "\n";
  }

  for (list<object>::iterator o=WS.begin(); o!=WS.end(); o++){
    OUT << "OBJECT\t" << "0x" << setbase(16) << o->type << setbase(10);
    if (o->text != "") OUT << " " << o->text;
    OUT << "\n";

    if (o->dir != 0) 
      OUT << "  DIR\t" << o->dir << "\n";
    for (Options::const_iterator i=o->opts.begin(); i!=o->opts.end(); i++){
      OUT << "  OPT\t" << i->first << "\t" << i->second << "\n"; // protect \n!
    }
    for (vector<string>::const_iterator i=o->comm.begin(); i!=o->comm.end(); i++){
      OUT << "  COMM\t" << *i << "\n";
    }
    o->labels.sort();
    for (list<lpos>::const_iterator l=o->labels.begin(); l!=o->labels.end(); l++){
      OUT << "  LABEL\t";  print_lpos(OUT, *l); OUT << "\n";
    }
    for (dMultiLine::const_iterator i=o->begin(); i!=o->end(); i++){
      OUT << "  DATA\t"; print_line(OUT, *i); OUT << "\n";
    }
  }

  return 0;
}

} // namespace
