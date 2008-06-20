
#include <string>
#include <fstream>
#include "../geo_io/geofig.h"
#include "../libmp/mp.h"

#include "../libzn/zn.h"

using namespace std;

/*****************************************************/
// Добавить картографические объекты из mp в привязанный fig
// в соответствии с системой знаков
int mp2fig(int argc, char** argv){

  if (argc != 3){
    cerr << "usage: zn mp2fig <mp> <conf file> <fig>\n";
    exit(0);
  }

  string mp_file   = argv[0];
  string conf_file = argv[1];
  string fig_file  = argv[2];

  std::cerr << "mp2fig:\n";  

  // читаем fig
  std::cerr << "  reading fig: " << fig_file <<"\n";  
  fig::fig_world F;
  if (!fig::read(fig_file.c_str(), F)) {
    cerr << "Bad fig file " << fig_file << "\n"; return 1;
  }

  // привязка fig-файла
  g_map ref = fig::get_ref(F);
  if (ref.size()<3){
    cerr << "Not a GEO-fig\n"; exit(0);
  }
  convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"));


  // читаем mp
  std::cerr << "  reading mp: " << mp_file <<"\n";  
  mp::mp_world M;
  if (!mp::read(mp_file.c_str(), M)){
    cerr << "Bad mp file " << mp_file << "\n"; return 1;
  }

  zn::zn_conv zconverter(conf_file);

  // mp->fig
  std::cerr << "  converting mp->fig\n";  
  for (mp::mp_world::const_iterator i=M.begin(); i!=M.end(); i++)
    F.push_back(zconverter.mp2fig(*i, cnv));

  // записываем fig
  std::cerr << "  writing fig: " << fig_file <<"\n";  
  ofstream out(fig_file.c_str());
  fig::write(out, F);
  return 0;
}

/*****************************************************/
// Добавить картографические объекты из fig в mp
// в соответствии с системой знаков
// Если файла mp нет - создать
int fig2mp(int argc, char** argv){

  if (argc != 3){
    cerr << "usage: zn fig2mp <fig> <conf file> <mp>\n";
    exit(0);
  }

  string fig_file  = argv[0];
  string conf_file = argv[1];
  string mp_file   = argv[2];

  std::cerr << "fig2mp:\n";  

  // читаем fig
  std::cerr << "  reading fig: " << fig_file <<"\n";  
  fig::fig_world F;
  if (!fig::read(fig_file.c_str(), F)) {
    cerr << "Bad fig file " << fig_file << "\n"; return 1;
  }

  // привязка fig-файла
  g_map ref = fig::get_ref(F);
  if (ref.size()<3){
    cerr << "Not a GEO-fig\n"; exit(0);
  }
  convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"));

  // читаем mp
  std::cerr << "  reading mp: " << mp_file <<"\n";  
  mp::mp_world M;
  mp::read(mp_file.c_str(), M);

  zn::zn_conv zconverter(conf_file);

  std::cerr << "  converting fig->mp\n";  
  for (fig::fig_world::const_iterator i=F.begin(); i!=F.end(); i++){
    if (!zn::is_map_depth(*i)) continue;
    M.push_back(zconverter.fig2mp(*i, cnv));
  }

  // записываем mp
  std::cerr << "  writing mp: " << mp_file <<"\n";  
  ofstream out(mp_file.c_str());
  mp::write(out, M);
  return 0;
}

/*****************************************************/
// Удалить картографические объекты из fig
int fig_clear(int argc, char** argv){

  if (argc != 2){
    cerr << "usage: zn fig_clear <fig> <conf file>\n";
    exit(0);
  }

  string fig_file  = argv[0];
  string conf_file = argv[1];

  std::cerr << "fig_clear:\n";  

  // читаем fig
  std::cerr << "  reading fig: " << fig_file <<"\n";  
  fig::fig_world F;
  if (!fig::read(fig_file.c_str(), F)) {
    cerr << "Bad fig file " << fig_file << "\n"; return 1;
  }

  zn::zn_conv zconverter(conf_file);

  std::cerr << "  removing map objects\n";  
  fig::fig_world::iterator i=F.begin(); 
  while (i!=F.end()){
    if (zn::is_map_depth(*i)) i=F.erase(i);
    else i++;
  }

  // записываем fig
  std::cerr << "  writing fig: " << fig_file << "\n";  
  ofstream out(fig_file.c_str());
  fig::write(out, F);
  return 0;
}

/*****************************************************/
// Удалить картографические объекты из mp
// Если mp не было - он создается
int mp_clear(int argc, char** argv){

  if (argc != 1){
    cerr << "usage: zn mp_clear <mp>\n";
    exit(0);
  }

  string mp_file  = argv[0];

  std::cerr << "mp_clear:\n";  

  // читаем mp
  std::cerr << "  reading mp: " << mp_file <<"\n";  
  mp::mp_world M;
  mp::read(mp_file.c_str(), M);

  std::cerr << "  removing map objects\n";  
  M.clear();

  std::cerr << "  writing mp: " << mp_file <<"\n";  
  ofstream out(mp_file.c_str());
  mp::write(out, M);
  return 0;
}


/*****************************************************/
int main(int argc, char** argv){
  if (argc < 2){
    cerr << "usage: zn <command> <args>\n";
    cerr << "commands: mp2fig, fig2mp, fig_clear, mp_clear\n";
    exit(0);
  }
  if (strcmp(argv[1], "mp2fig")==0)    exit(mp2fig(argc-2, argv+2));
  if (strcmp(argv[1], "fig2mp")==0)    exit(fig2mp(argc-2, argv+2));
  if (strcmp(argv[1], "fig_clear")==0) exit(fig_clear(argc-2, argv+2));
  if (strcmp(argv[1], "mp_clear")==0)  exit(mp_clear(argc-2, argv+2));

  cerr << "unknown command: " << argv[1] << "\n";
  exit(1);
}
