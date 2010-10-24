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

namespace vmap {

const int point_scale = 1000000;

using namespace std;

/***************************************/

void
point_read_cnv(dPoint &p){
  p.x = (p.x+0.5)/point_scale; // avoid getting different value from floor()
  p.y = (p.y+0.5)/point_scale;
}
void
point_write_cnv(dPoint &p){
  p.x = floor(p.x*point_scale);
  p.y = floor(p.y*point_scale);
}


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

lpos get_lpos(const string & s){
  lpos ret;
  istringstream IN1(s);
  IN1 >> ret.pos >> ret.dir >> ret.hor >> ret.ang;
  point_read_cnv(ret.pos);
  return ret;
}

lpos_full get_lbuf(const string & s){
  lpos_full ret;
  istringstream IN1(s);
  IN1 >> ret.pos >> ret.dir >> ret.hor >> ret.ang
      >> ret.ref;
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
read_object(istream & IN, string & s){
  object ret;
  string key,val;
  bool read_ahead=false;

  if ((get_kv(s, key, val)!=0) || (key!="OBJECT")){
    cerr << "wrong call of read_object()!\n";
    return ret;
  }

  istringstream IN1(val);
  IN1 >> setbase(16) >> ret.type;

  while (!IN.eof() || read_ahead){
    if (!read_ahead) getline(IN, s);
    else read_ahead=false;
    if (get_kv(s, key, val)!=0) continue;

    if (key=="TEXT"){
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
      ret.labels.push_back(get_lpos(val));
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

  getline(IN, s);
  if (s!="VMAP 3.0"){
    cerr << "error: not a VMAP 3.0 file\n";
    return ret;
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
      ret.lbuf.push_back(get_lbuf(val));
      continue;
    }
    if (key=="OBJECT"){
      ret.push_back(read_object(IN, s));
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
void print_lpos(ostream & OUT, const lpos & L){
  OUT << iPoint(L.pos) << " " << L.dir << " " << L.hor << " " << L.ang;
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

  OUT << "VMAP 3.0\n";
  if (WS.name!=""){
    OUT << "NAME\t" << WS.name << "\n";
  }
  OUT << "RSCALE\t" << int(WS.rscale) << "\n";
  OUT << "STYLE\t" << WS.style << "\n";
  OUT << "MP_ID\t" << WS.mp_id << "\n";
  if (WS.brd.size()>0){
    OUT << "BRD\t";  print_line(OUT, WS.brd); OUT << "\n";
  }

  // lbuf
  for (list<lpos_full>::const_iterator l=WS.lbuf.begin(); l!=WS.lbuf.end(); l++){
    OUT << "LBUF\t"; print_lpos(OUT, *l);
    OUT << " " << iPoint(l->ref) << " " << l->text << "\n";
  }

  for (list<object>::iterator o=WS.begin(); o!=WS.end(); o++){
    OUT << "OBJECT\t" << "0x" << setbase(16) << o->type << setbase(10) << "\n";
    if (o->text != "")
      OUT << "  TEXT\t" << o->text << "\n";
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
