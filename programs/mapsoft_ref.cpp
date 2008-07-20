// привязка fig-файла по геоданным
// ./mapsoft_ref <data> -f <fig-file>
// привязка строится по соответствию треков и точек в fig-файле и в данных

#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>


#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "../geo_io/io.h"
#include "../libfig/fig.h"

using namespace std;

void usage(const char *fname){
  cerr << "Usage: "<< fname << " <in1> ... <inN> -o <fig>\n";
  exit(0);
}

int main(int argc, char *argv[]) {

  Options opts;
  list<string> infiles;
  string outfile = "";

// разбор командной строки
  for (int i=1; i<argc; i++){ 

    if ((strcmp(argv[i], "-h")==0)||
        (strcmp(argv[i], "-help")==0)||
        (strcmp(argv[i], "--help")==0)) usage(argv[0]);

    if (strcmp(argv[i], "-o")==0){
      if (i==argc-1) usage(argv[0]);
      i+=1;
      outfile = argv[i];
      continue;
    }
    infiles.push_back(argv[i]);
  }
  if (outfile == "") usage(argv[0]);

  // чтение геоданных

  geo_data d;
  list<string>::const_iterator i;
  for(i=infiles.begin(); i!=infiles.end(); i++) io::in(*i, d, Options());

  // читаем fig
  fig::fig_world F;
  if (!fig::read(outfile.c_str(), F)) {
    exit(1);
  }

  // теперь все точки и треки заменяем на точки привязки!
  using namespace boost::spirit;
  for (fig::fig_world::iterator i=F.begin(); i!=F.end(); i++){
    string name;
    if ((i->comment.size()>0)&&(parse(i->comment[0].c_str(), str_p("WPT")
          >> +space_p >> (*anychar_p)[assign_a(name)]).full)){
      for (vector<g_waypoint_list>::const_iterator wl = d.wpts.begin(); wl!=d.wpts.end(); wl++){
        for (vector<g_waypoint>::const_iterator w = wl->begin(); w!=wl->end(); w++){
          if (w->name == name){
            ostringstream st;
            st << "REF " << w->x << w->y;
            i->comment.clear();
            i->comment.push_back(st.str());
          }
        }
      }
    }
    if ((i->comment.size()>0)&&(parse(i->comment[0].c_str(), str_p("TRK")
          >> +space_p >> (*anychar_p)[assign_a(name)]).full)){
      for (vector<g_track>::const_iterator tr = d.trks.begin(); tr!=d.trks.end(); tr++){
        if (tr->comm == name){
          for (int n = 0; n < min(tr->size(), i->size()); n++){
            ostringstream st;
            st << "REF " << (*tr)[n].x << " " << (*tr)[n].y;
            fig::fig_object o = *i;
            o.clear();
            o.comment.clear();
            o.comment.push_back(st.str());
            o.push_back(Point<int>((*i)[n].x,(*i)[n].y));
            F.push_back(o);
          }
        }
      }
    }
  }

  ofstream out(outfile.c_str());
  fig::write(out, F);
}

