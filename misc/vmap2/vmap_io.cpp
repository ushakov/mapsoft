#include <sstream>
#include <fstream>
#include <iomanip>

#include "vmap.h"

using namespace std;
namespace vmap {


void dump_object(vmap & M, string & id, string & ot,
            list<string> & comm, Options & opts,
            dLine & line, dMultiLine & mline, lpos_t & lpos){

  if ((id != "") && (ot != "")){

    if (line.size()>0) mline.push_back(line);
    if (ot == "RMAP"){
      rmap_t o;
      o.comm.insert(o.comm.end(), comm.begin(), comm.end());
      o.opts=opts;
      M.rmaps[id]=o;
    }
    else if (ot == "MOBJ"){
      mobj_t o;
      o.insert(o.end(), mline.begin(), mline.end());
      o.comm.insert(o.comm.end(), comm.begin(), comm.end());
      o.opts=opts;
      M.mobjs[id]=o;
    }
    else if (ot == "LPOS"){
      lpos.comm.insert(lpos.comm.end(), comm.begin(), comm.end());
      lpos.opts=opts;
      M.lpos[id]=lpos;
    }
  }
  lpos=lpos_t();
  mline.clear();
  line.clear();
  comm.clear();
  opts.clear();
  ot="";
  id="";
}


istream & operator>> (istream & s, vmap & M){
  char mode;

  string ot, id, key, val;
  list<string> comm;
  Options      opts;
  dLine        line;
  dMultiLine  mline;
  lpos_t       lpos;

  string istr;

  int ln=0;
  while (s.get(mode)){
    dPoint p;

    if (mode=='\n'){
      if ((line.size()>0) && (ot=="MOBJ")){
        mline.push_back(line);
        line.clear();
      }
      continue;
    }

    if (s.peek()!=EOF){
      if (!getline(s, istr)) break;
    }
    istringstream s1(istr);
    ln++;

    char m=mode;
    if ( ((m>='a') && (m<='z')) ||
         ((m>='A') && (m<='Z')) ) m='A'; // options starts with [a-zA-Z]

    switch (m){
      case '=': // start of object
        dump_object(M, id, ot, comm, opts, line, mline, lpos);
        s1 >> ot >> id >> ws;
        if (!s1) goto err;
        if ((ot!="RMAP") && (ot!="MOBJ") && (ot!="LPOS")){
          cerr << "line " << ln << ": skipping unknown object \"" << ot << "\"\n";
          ot="";
          continue;
        }
        if (id==""){
          cerr << "line " << ln << ": skipping object with empty id\n";
          continue;
        }
        break;
      case '#': // comment
        comm.push_back(istr);
        break;
      case ' ': // data
        if (ot == "MOBJ"){
          s1 >> p.x >> p.y >> ws;
          if (!s1 || !s1.eof()) goto err;
          line.push_back(p);
        }
        else if (ot == "LPOS"){
          lp_t lp;
          string oid;
          s1 >> oid >> lp.x >> lp.y >> lp.align >> lp.angle >> ws;
          if (!s1 || !s1.eof()) goto err;
          lpos.insert(pair<string, lp_t>(oid,lp));
        }
        break;
      case 'A': // key-value pair
        s1 >> key >> ws;
        getline(s1, val);
        if (!s1) goto err;
        key.insert(key.begin(),mode);
        opts[key]=val;
        break;
      default:
        err:
        cerr << "line " << ln << ": skipping bad line: "
             << (char)mode << istr << "\n";
        continue;
    }
  }
  dump_object(M, id, ot, comm, opts, line, mline, lpos);
  return s;
}

void print_comm (ostream & s, const vector<string> & comm){
  for (vector<string>::const_iterator i=comm.begin(); i!=comm.end(); i++)
    if (i->size()>0) s << '#' << *i << '\n';
}
void print_opts (ostream & s, const Options & opts){
  for (Options::const_iterator i=opts.begin(); i!=opts.end(); i++)
    s << i->first << '\t' << i->second << '\n';
}
void print_line (ostream & s, const dLine & line){
  if (line.size()>0) s << "\n";
  for (dLine::const_iterator i=line.begin(); i!=line.end(); i++)
    s << ' ' << setprecision(6) << fixed << i->x << ' ' << i->y << '\n';
}
void print_mline (ostream & s, const dMultiLine & mline){
  for (dMultiLine::const_iterator l=mline.begin(); l!=mline.end(); l++)
    print_line(s, *l);
}
void print_lpos (ostream & s, const lpos_t & lpos){
    for (lpos_t::const_iterator i=lpos.begin(); i!=lpos.end(); i++)
      s << " " << i->first << '\t'
        << setprecision(6) << fixed << i->second.x << ' ' << i->second.y
        << ' ' << i->second.align << ' ' << setprecision(2) << i->second.angle <<  '\n';
}


istream & operator<< (ostream & s, const vmap & m){
  for (map<string, rmap_t>::const_iterator o=m.rmaps.begin(); o!=m.rmaps.end(); o++){
    s << "= RMAP " << o->first << "\n";
    print_comm(s, o->second.comm);
    print_opts(s, o->second.opts);
    s << '\n';
  }
  for (map<string, lpos_t>::const_iterator o=m.lpos.begin(); o!=m.lpos.end(); o++){
    s << "= LPOS " << o->first << "\n";
    print_comm(s, o->second.comm);
    print_opts(s, o->second.opts);
    print_lpos(s, o->second);
    s << '\n';
  }
  for (map<string, mobj_t>::const_iterator o=m.mobjs.begin(); o!=m.mobjs.end(); o++){
    s << "= MOBJ " << o->first << "\n";
    print_comm(s, o->second.comm);
    print_opts(s, o->second.opts);
    print_mline(s, o->second);
    s << '\n';
  }
}

int vmap_read(const std::string & file, vmap & M){
  ifstream F(file.c_str());
  if (!F) {
    cerr << "Error: can't open vmap file \"" << file << "\"\n";
    return -1;
  }
  F >> M;
  if (!F.eof()) {
    cerr << "Error: can't read vmap file \"" << file << "\"\n";
    return -1;
  }
  return 0;
}

int vmap_write(const std::string & file, const vmap & M){
  ofstream F(file.c_str());
  if (!F) {
    cerr << "Error: can't open vmap file \"" << file << "\"\n";
    return -1;
  }
  F << M;
  if (!F) {
    cerr << "Error: can't write to vmap file \"" << file << "\"\n";
    return -1;
  }
  return 0;
}

} // namespace
