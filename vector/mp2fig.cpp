#include <boost/spirit/core.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include <boost/spirit/actor/insert_at_actor.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>

#include "geofig.h"
#include "mp.h"
#include "../geo_io/geo_convs.h"

using namespace std;
using namespace boost::spirit;

typedef char                    char_t;
typedef file_iterator <char_t>  iterator_t;
typedef scanner<iterator_t>     scanner_t;
typedef rule <scanner_t>        rule_t;


void usage(const char *fname){
  cerr << "usage: " << fname << " <cnvfile> <infile.fig> <outfile> [<not_converted_file>]\n";
  cerr << "or:    " << fname << " <cnvfile> <infile.mp>  <outfile> [<not_converted_file>]\n";
  exit(0);
}

bool testext(const string & nstr, char *ext){
  int pos = nstr.rfind(ext);
  return ((pos>0)&&(pos == nstr.length()-strlen(ext)));
}


main(int argc, char **argv){

  Options opts;
  map<string,string> f2m, m2f;
  string infile, outfile, cnvfile, ncfile;

// разбор командной строки
  if ((argc!=4)&&(argc!=5)) usage(argv[0]);
  cnvfile = argv[1];
  infile  = argv[2];
  outfile = argv[3];
  if (argc==5) ncfile = argv[4];

  bool fig2mp;
  if      (testext(infile, ".mp"))  fig2mp=false;
  else if (testext(infile, ".fig")) fig2mp=true;
  else usage(argv[0]);

// чтение cnv-файла
  string tmp;
  int cn;
  fig::fig_colors colors;

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
  rule_t col = str_p("color:")
    >> *blank_p >> uint_p[assign_a(cn)] 
    >> *blank_p >> "#" >> hex_p[insert_at_a(colors,cn)]
    >> *blank_p >> eol_p;

  if (!parse(first, last, *(comment | empty | m2f_r | f2m_r | col | oo)).full){
    cerr << "can't parse cnv-file!\n";
    exit(0);
  }

// преобразования
  ofstream out(outfile.c_str()), nc(ncfile.c_str());
  if (fig2mp) {
    cerr << "reading fig-file: " << infile << ", ";
    fig::fig_world F = fig::read(infile.c_str()), NC;
    cerr << F.size() << " objects\n";
    NC.colors=colors;

    mp::mp_world   M; 
   
    g_map map = fig::get_map(F);
    convs::map2pt C(map, Datum("wgs84"), Proj("lonlat"), Options());

    for (fig::fig_world::const_iterator i=F.begin(); i!=F.end(); i++){
      bool converted=false;
      for (std::map<string,string>::const_iterator r=f2m.begin(); r!=f2m.end(); r++){
        if (fig::test_object(*i, r->first)){
          mp::mp_object o = mp::make_object(r->second); 

          o.set_vector(C.line_frw(i->get_vector()));

          // если линия замкнута - добавим посл.точку=первой
          if (((i->type==3) && ((i->sub_type==1)||(i->sub_type==3)||(i->sub_type==5)))||
              ((i->type==2) && (i->sub_type>=2))){
            o.X.push_back(o.X[0]);
            o.Y.push_back(o.Y[0]);
          }
          parse (i->comment.c_str(), str_p("# ") >> (+(anychar_p-eol_p))[assign_a(o.Label)] >> !(eol_p >> *anychar_p));
          M.push_back(o);
          converted=true;
        }
      }
      if (!converted) NC.push_back(*i);
    }
    cerr << NC.size() << " objects not converted\n";
    if (out) {mp::write(out, M); out.close();}
    if (nc)  {fig::write(nc, NC); nc.close();}
    exit(0);
  } 
  
  else {
    cerr << "reading mp-file: " << infile << ", ";
    mp::mp_world   M = mp::read(infile.c_str()), NC; 
    cerr << M.size() << " objects\n";
    fig::fig_world F;
    F.colors=colors;

    Datum  datum(opts.get_string("datum", "pulkovo"));
    Proj   proj(opts.get_string("proj", "tmerc"));
    convs::pt2pt cnv(datum,proj,opts, Datum("wgs84"), Proj("lonlat"), Options());

    // определим реальный диапазон координат в нужной нам проекции:
    double minx(1e99), miny(1e99), maxx(-1e99), maxy(-1e99);
    double lon0=0; int ln=0;
    for (mp::mp_world::const_iterator i=M.begin(); i!=M.end(); i++){
      for (int n=0; n < min(i->X.size(),i->Y.size()); n++){
        lon0+=i->Y[n]; ln++;
        g_point p(i->Y[n], i->X[n]); cnv.bck(p);
        if (p.x > maxx) maxx = p.x;
        if (p.y > maxy) maxy = p.y;
        if (p.x < minx) minx = p.x;
        if (p.y < miny) miny = p.y;
      }
    }
    if ((maxx<minx)||(maxy<miny)){
      std::cerr << "mp2fig: empty data\n";
      exit(0);
    }
    lon0 = floor( lon0/ln/6.0 ) * 6 + 3;

    // если диапазон явно указан в конфигурационном файле
    minx=opts.get_double("minx", minx);
    maxx=opts.get_double("maxx", maxx);
    miny=opts.get_double("miny", miny);
    maxy=opts.get_double("maxy", maxy);
    lon0  = opts.get_udouble("lon0", lon0);
    double scale = opts.get_udouble("scale", 1e-5);

    // построим привязку fig-файла
    // добавим в fig-файл точки привязки
    g_map map;
    double W = (maxx-minx)*scale*100*fig::cm2fig;
    double H = (maxy-miny)*scale*100*fig::cm2fig;
    g_refpoint rps[4] = {
      g_refpoint(minx, miny,0,H), 
      g_refpoint(minx, maxy,0,0), 
      g_refpoint(maxx, miny,W,H), 
      g_refpoint(maxx, maxy,W,0)};
    for (int n=0; n<4; n++){
      fig::fig_object o = fig::make_object("2 1 0 4 4 7 1 -1 -1 0.000 0 1 -1 0 0 *");
      o.x.push_back( int(rps[n].xr) );
      o.y.push_back( int(rps[n].yr) );
      ostringstream comm;
      comm << "# REF " << fixed << rps[n].x << " " << rps[n].y << "\n";
      if (datum != Datum("wgs84"))
         comm << "# datum: " << datum.xml_str() << "\n"; 
      if (proj != Proj("lonlat"))
         comm << "# proj: " << proj.xml_str() << "\n"; 
      if ((proj == Proj("tmerc")) && (lon0!=0))
         comm << "# lon0: " << lon0 << "\n"; 
      F.push_back(o);
      
      cnv.frw(rps[n]);
      map.push_back(rps[n]);
      map.border.push_back(g_point(0,0));
    }
    map.map_proj=proj;

    convs::map2pt C(map, Datum("wgs84"), Proj("lonlat"), Options());

    // собственно преобразования
    for (mp::mp_world::const_iterator i=M.begin(); i!=M.end(); i++){
      bool converted=false;
      for (std::map<string,string>::const_iterator r=m2f.begin(); r!=m2f.end(); r++){
        if (mp::test_object(*i, r->first)){
          fig::fig_object o = fig::make_object(r->second);
          o.set_vector(C.line_bck(i->get_vector()));
          // если это сплайн:
          if (o.type==3){
            double f;
            if ((o.sub_type==0)||(o.sub_type==1)) f=1;
            if ((o.sub_type==2)||(o.sub_type==3)) f=-1;
            if ((o.sub_type==4)||(o.sub_type==5)) f=0.4;
            o.f.resize(o.x.size(), f);
            if (o.f.size()>0){ // края
              o.f[0]=0; o.f[o.f.size()-1]=0;
            }
          }
          o.comment="# "+i->Label;
          F.push_back(o);
          converted=true;
        }
      }
      if (!converted) NC.push_back(*i);
    }
    cerr << NC.size() << " objects not converted\n";
    if (out) { fig::write(out, F); out.close();}
    if (nc)  { mp::write(nc, NC); nc.close();}
  }

}
