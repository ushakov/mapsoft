#include <boost/spirit/core.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include <boost/spirit/actor/insert_at_actor.hpp>

#include <iostream>
#include <fstream>
#include <map>
#include <string>

#include "geofig.h"
#include "mp.h"

using namespace std;
using namespace boost::spirit;

typedef char                    char_t;
typedef file_iterator <char_t>  iterator_t;
typedef scanner<iterator_t>     scanner_t;
typedef rule <scanner_t>        rule_t;


void usage(const char *fname){
  cerr << "usage: " << fname << " <cnvfile> <infile> <outfile>\n";
  exit(0);
}

main(int argc, char **argv){

  Options opts;
  map<string,string> f2m, m2f;
  string infile, outfile, cnvfile;

// разбор командной строки
  if (argc!=4) usage(argv[0]);
  cnvfile = argv[1];
  infile  = argv[2];
  outfile = argv[3];

  bool fig2mp = (strcmp(argv[0], "fig2mp")==0);

// чтение cnv-файла
  string tmp;

  iterator_t first(cnvfile);
  if (!first) { cerr << "can't find file " << cnvfile << '\n'; exit(0);}
  iterator_t last = first.make_end();

  rule_t ch = anychar_p-eol_p;
  rule_t comment = ch_p('#') >> *ch >> eol_p;
  rule_t empty   = *blank_p >> eol_p;
  rule_t m2f_r = str_p("mp2fig:") 
    >> (*(ch-':'))[assign_a(tmp)] >> ':' 
    >> (*(ch-':'))[insert_at_a(m2f,tmp)] >> ':' 
    >> *ch >> eol_p;
  rule_t f2m_r = str_p("fig2mp:") 
    >> (*(ch-':'))[assign_a(tmp)] >> ':' 
    >> (*(ch-':'))[insert_at_a(f2m,tmp)] >> ':' 
    >> *ch >> eol_p;
  rule_t oo = (+(ch-':'-blank_p))[assign_a(tmp)] 
    >> *blank_p >> ':' >> *blank_p 
    >> (*(ch-':'-blank_p))[insert_at_a(opts, tmp)] 
    >> *blank_p >> eol_p;

  if (!parse(first, last, *(comment | empty | m2f_r | f2m_r | oo)).full){
    cerr << "can't parse cnv-file!\n";
    exit(0);
  }

// преобразования
  ofstream out(outfile.c_str());
  if (fig2mp) {
    fig::fig_world F = fig::read(infile.c_str());
    mp::mp_world   M; 
    g_map map = fig::get_map(F);

    for (fig::fig_world::const_iterator i=F.begin(); i!=F.end(); i++){
      for (std::map<string,string>::const_iterator r=f2m.begin(); r!=f2m.end(); r++){
        if (fig::test_object(*i, r->first)){
          mp::mp_object o = mp::make_object(r->second);
          M.push_back(o);
        }
      }
    }
    mp::write(out, M);
  } 

  else {
    mp::mp_world   M = mp::read(infile.c_str());; 
    fig::fig_world F;

    for (mp::mp_world::const_iterator i=M.begin(); i!=M.end(); i++){
      for (std::map<string,string>::const_iterator r=m2f.begin(); r!=m2f.end(); r++){
        if (mp::test_object(*i, r->first)){
          fig::fig_object o = fig::make_object(r->second);
          F.push_back(o);
        }
      }
    }
    fig::write(out, F);
  }

}
