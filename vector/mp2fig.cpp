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
#include <cmath>

#include "../geo_io/geofig.h"
#include "../geo_io/mp.h"
#include "../geo_io/geo_convs.h"

using namespace std;
using namespace boost::spirit;

typedef char                    char_t;
typedef file_iterator <char_t>  iterator_t;
typedef scanner<iterator_t>     scanner_t;
typedef rule <scanner_t>        rule_t;

typedef pair<string,string> mask;


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
  vector<mask> f2m, m2f, f2m_t;
  vector<mask> mp_mkpt;
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

// чтение cnv-файла ///////////////////////////////////////////////////
  string tmp1, tmp2;

  mask tmp;
  int cn;
  fig::fig_colors colors;

  iterator_t first(cnvfile);
  if (!first) { cerr << "can't find file " << cnvfile << '\n'; exit(0);}
  iterator_t last = first.make_end();

  rule_t ch = anychar_p-eol_p;
  rule_t comment = ch_p('#') >> *ch >> eol_p;
  rule_t empty   = *blank_p >> eol_p;
  rule_t m2f_r = str_p("mp2fig:") 
    >> (*(ch-':'))[assign_a(tmp.first)] >> ':' 
    >> (*(ch-':'))[assign_a(tmp.second)] >> ':' 
    >> *ch >> eol_p[push_back_a(m2f,tmp)];
  rule_t f2m_r = str_p("fig2mp:") 
    >> (*(ch-':'))[assign_a(tmp.first)] >> ':' 
    >> (*(ch-':'))[assign_a(tmp.second)] >> ':' 
    >> *ch >> eol_p[push_back_a(f2m,tmp)];
  rule_t mp_mkpt_r = str_p("mp_mkpt:") 
    >> (*(ch-':'))[assign_a(tmp.first)] >> ':' 
    >> (*(ch-':'))[assign_a(tmp.second)] >> ':' 
    >> *ch >> eol_p[push_back_a(mp_mkpt,tmp)];
  rule_t f2m_t_r = str_p("fig2mp_txt:") 
    >> (*(ch-':'))[assign_a(tmp.first)] >> ':' 
    >> (*(ch-':'))[assign_a(tmp.second)] >> ':' 
    >> *ch >> eol_p[push_back_a(f2m_t,tmp)];
  rule_t oo = (+(ch-':'-blank_p))[assign_a(tmp.first)][assign_a(tmp.second,"")]
    >> !(*blank_p >> ':' >> *blank_p 
    >> (*(ch-':'-blank_p))[assign_a(tmp.second)]) 
    >> *blank_p >> eol_p[insert_at_a(opts, tmp.first, tmp.second)];
  rule_t col = str_p("color:")
    >> *blank_p >> uint_p[assign_a(cn)] 
    >> *blank_p >> "#" >> hex_p[insert_at_a(colors,cn)]
    >> *blank_p >> eol_p;

  if (!parse(first, last, *(comment | empty | mp_mkpt_r | m2f_r | f2m_r | f2m_t_r | col | oo)).full){
    cerr << "can't parse cnv-file!\n";
    exit(0);
  }

// преобразования //////////////////////////////////
  ofstream out(outfile.c_str()), nc(ncfile.c_str());
  if (fig2mp) {

    cerr << "reading fig-file: " << infile << ", ";
    fig::fig_world F = fig::read(infile.c_str()), NC;
    cerr << F.size() << " objects\n";
    NC.colors=colors;

    if (opts.get_bool("white_fill_conv")){
      for (fig::fig_world::iterator i=F.begin(); i!=F.end(); i++)
        if ((i->fill_color!=7)&&(i->area_fill==40)){i->fill_color=7; i->area_fill=20;}
    }

    if (opts.get_bool("spl2poly")){
      for (fig::fig_world::iterator i=F.begin(); i!=F.end(); i++){
        if (i->type==3){i->type=2; i->sub_type=1+2*(i->sub_type%2);}
      }
    }
    bool cfc=false; 
    if (opts.get_bool("comm_from_comp")) cfc=true;

    double txt_dist = 2; // максимальное расстояние (см), на котором ловится текст.
    opts.get_udouble("txt_dist");
    txt_dist *= fig::cm2fig;

    mp::mp_world   M; 
   
    g_map map = fig::get_map(F);
    convs::map2pt C(map, Datum("wgs84"), Proj("lonlat"), Options());

    // ловля подписей!
    // для каждого текста ищем объекты подходящего вида
    // выбираем тот, у которого первая строчка комментария совпадает
    // с текстом, а если такого нет - ближайший
    // В комментарий к объекту вставляем текст и указание на его положение...

    // расстояние от объекта до текста здесь считается довольно глупо:
    // как расстояние от начальной точки текста до ближайшей точки объекта

    // нам нужно два прохода: 
    // сперва пройдемся по всех текстовым объектам и посмотрим,
    // нет ли подходящего объекта для них с правильным комментарием...
    // потом уже определяем неподписанные объекты

    for (fig::fig_world::iterator i=F.begin(); i!=F.end(); i++){
      if (i->type!=4) continue;
      if ((i->comment.size()>0)&&(i->comment[0]=="[skip]")) continue;

      for (vector<mask>::const_iterator m=f2m_t.begin(); m!=f2m_t.end(); m++){
        if (!fig::test_object(*i, m->first)) continue;

        fig::fig_world::iterator o1=F.end();
        double mindist = 1e99;
        for (fig::fig_world::iterator o=F.begin(); o!=F.end(); o++){
          if (!fig::test_object(*o, m->second)) continue;
          if ((o->comment.size()==0) || (i->text != o->comment[0])) continue;
          double dist = 1e99;
          for (int n=0; n<min(o->x.size(),o->y.size());n++){
            double d=sqrt( (i->x[0] - o->x[n])*(i->x[0] - o->x[n]) + 
                           (i->y[0] - o->y[n])*(i->y[0] - o->y[n]) );
            if (dist>d) dist=d;
          }
          if (mindist>dist) {mindist=dist; o1=o;}
        }
        if (mindist > txt_dist) continue;

        if (o1==F.end()) continue;
        cerr << "txt_capt-1: " << i->text << " mindist:" << mindist <<  "\n";
        o1->comment.clear();
        o1->comment.push_back(i->text);
        o1->comment.push_back("[txt: ]");
        i->comment.clear();
        i->comment.push_back("[skip]");
        break;
      }
    }


    for (fig::fig_world::iterator i=F.begin(); i!=F.end(); i++){
      if (i->type!=4) continue;
      if ((i->comment.size()>0)&&(i->comment[0]=="[skip]")) continue;

      for (vector<mask>::const_iterator m=f2m_t.begin(); m!=f2m_t.end(); m++){
        if (!fig::test_object(*i, m->first)) continue;
        double mindist = 1e99;
        fig::fig_world::iterator o1=F.end();

        for (fig::fig_world::iterator o=F.begin(); o!=F.end(); o++){
          if (!fig::test_object(*o, m->second)) continue;
          double dist = 1e99;
          for (int n=0; n<min(o->x.size(),o->y.size());n++){
            double d=sqrt( (i->x[0] - o->x[n])*(i->x[0] - o->x[n]) + 
                           (i->y[0] - o->y[n])*(i->y[0] - o->y[n]) );
            if (dist>d) dist=d;
          }
          if (mindist>dist) {mindist=dist; o1=o;}
        }

	if (mindist > txt_dist) continue;
        if (o1==F.end()) continue;
        cerr << "txt_capt-2: " << i->text << " mindist:" << mindist <<  "\n";
        o1->comment.clear();
        o1->comment.push_back(i->text);
        o1->comment.push_back("[txt: ]");
        i->comment.clear();
        i->comment.push_back("[skip]");
        break;
      }
    }

    ////////////////////////////////////////////////////////////////////

    int depth=0, comm_depth=0;
    vector<string> comp_comm;
    // преобразования объектов
    for (fig::fig_world::iterator i=F.begin(); i!=F.end(); i++){

      bool converted=false;
      for (int n=0; n< i->comment.size(); n++) if (i->comment[n]=="[skip]") converted=true;
      if (converted) continue;

      if (i->type==6) depth++;

      if (cfc && (i->type==6) && (i->comment.size()!=0)){ comp_comm=i->comment; comm_depth=depth;}
      if ((i->type==-6)&&(comm_depth==depth)) comp_comm.clear();

      if (i->type==-6) depth--;
      if ((i->type==6)||(i->type==-6)) continue;
 
      if (cfc && (i->comment.size()==0)) i->comment=comp_comm;

      for (vector<mask>::const_iterator r=f2m.begin(); r!=f2m.end(); r++){

        if (!fig::test_object(*i, r->first)) continue;
        mp::mp_object o = mp::make_object(r->second); 
        o = C.line_frw(i->get_vector());

        // если линия замкнута - добавим посл.точку=первой
        if (((i->type==3) && ((i->sub_type==1)||(i->sub_type==3)||(i->sub_type==5)))||
            ((i->type==2) && (i->sub_type>=2) && (o.size()>0))){
          o.push_back(o[0]);
        }
        // Если объект - текст, то text->Label, comment->comment
        // Иначе, первую строчку comment->Label, остальные в comment
        if (i->type==4){
          o.Label=i->text;
          o.Comment=i->comment;
        }
        else {
          if (i->comment.size()>0){
            o.Label=i->comment[0];
            for (int n=1; n< i->comment.size(); n++) o.Comment.push_back(i->comment[n]);
          }
        }
        // если есть стрелка вперед -- установить DirIndicator=1
        // если есть стрелка назад -- установить  DirIndicator=2
        // если 0 или 2 стрелки - DirIndicator=0
        if ((i->forward_arrow==1)&&(i->backward_arrow==0)) o.DirIndicator=1;
        if ((i->forward_arrow==0)&&(i->backward_arrow==1)) o.DirIndicator=2;


        M.push_back(o);
        converted=true;

      }
      if (!converted) NC.push_back(*i);
    }
    cerr << NC.size() << " objects not converted\n";

    for (mp::mp_world::iterator i=M.begin(); i!=M.end(); i++){
      for (vector<mask>::const_iterator r=mp_mkpt.begin(); r!=mp_mkpt.end(); r++){
        if (!mp::test_object(*i, r->first)) continue;
        if (i->Label == "") continue;
        mp::mp_object o = mp::make_object(r->second); 
        o.push_back(i->center());
        o.Label = i->Label;
        o.Comment.push_back("[skip]");
        M.push_back(o);
      }
    }
    

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
      for (int n=0; n < i->size(); n++){
        lon0+=(*i)[n].x; ln++;
        g_point p = (*i)[n]; cnv.bck(p);
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
    lon0  = opts.get_double("lon0", lon0);
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
      comm << "REF " << fixed << rps[n].x << " " << rps[n].y;
      o.comment.push_back(comm.str()); comm.str("");
      if (datum != Datum("wgs84"))
         comm << "datum: " << datum.xml_str(); 
      o.comment.push_back(comm.str()); comm.str("");
      if (proj != Proj("lonlat"))
         comm << "proj: " << proj.xml_str(); 
      o.comment.push_back(comm.str()); comm.str("");
      if ((proj == Proj("tmerc")) && (lon0!=0))
         comm << "lon0: " << lon0; 
      o.comment.push_back(comm.str()); comm.str("");
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
      for (int n=0; n< i->Comment.size(); n++) if (i->Comment[n]=="[skip]") converted=true;
      if (converted) continue;

      for (vector<mask>::const_iterator r=m2f.begin(); r!=m2f.end(); r++){
        if (mp::test_object(*i, r->first)){
          fig::fig_object o = fig::make_object(r->second);
          o.set_vector(C.line_bck(*i));

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
          // полигон не может содержать менее чем 3 точки
          if ((o.type==2)&&(o.sub_type>1)&&(o.x.size()<3)) o.sub_type=1;
          if ((o.type==3)&&(o.sub_type%2==1)&&(o.x.size()<3)) o.sub_type--;


          // Если объект - текст, то Label->text, comment->comment
          // Иначе, Label -> первую строчку comment, comment->comment
          if (o.type==4){
            o.text = i->Label;
            o.comment = i->Comment;
          }
          else {
            if (i->Label !="") o.comment.push_back(i->Label);
            for (int n=0; n<i->Comment.size(); n++) o.comment.push_back(i->Comment[n]);
          }
          // если DirIndicator==1 -- стрелка вперед,
          // если DirIndicator==2 -- стрелка назад
          if (i->DirIndicator==1) {o.forward_arrow=1; o.backward_arrow==0; o.farrow_width=30; o.farrow_height=30;}
          if (i->DirIndicator==2) {o.forward_arrow=0; o.backward_arrow==1; o.barrow_width=30; o.barrow_height=30;}

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
