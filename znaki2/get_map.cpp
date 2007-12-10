// ��������� fig-����� ��� ��������������

#include <string>
#include <fstream>
#include "zn.h"
#include "zn_key.h"

using namespace std;

/*
������ ����� �� ��������� � ������� fig ��� mp.
��� ��������� ���������������� �������� ���������������
� ����������� �� �� ����� (� ��������� ������� 
��� �������� �� �����!)
*/

void usage(){
    cerr << "usage: get_map <map> <conf_file> <out.fig|out.mp>\n";
    exit(0);
}

const string maps_dir  = "./maps";

// �������� ����������
bool testext(const string & nstr, const char *ext){
  int pos = nstr.rfind(ext);
  return ((pos>0)&&(pos == nstr.length()-strlen(ext)));
}

main(int argc, char** argv){

  if (argc != 4) usage();
  string map_name  = argv[1];
  string conf_file = argv[2];
  string out_file = argv[3];

  string file = maps_dir+"/"+map_name+".fig";

  // ������� ��������� �����
  fig::fig_world MAP = fig::read(file.c_str());
  if (MAP.size()==0) {
    cerr << "bad file " << file << "\n";
    exit(0);
  }

  zn::zn_conv zconverter(conf_file);
  
  if (testext(out_file, ".fig")){ // ����� fig
    fig::fig_world F;
    for (fig::fig_world::iterator i=MAP.begin(); i!=MAP.end(); i++){
      if ((i->depth >=50) && (i->depth <400)){
        zn::zn_key k = zn::get_key(*i);
        zconverter.fig_update(*i, k.type);
        list<fig::fig_object> l1 = zconverter.make_pic(*i, k.type);
        F.insert(F.begin(), l1.begin(), l1.end());
      }
      else F.push_back(*i);
    }
    ofstream out(out_file.c_str());
    fig::write(out, F);
  } 

  else if (testext(out_file, ".mp")){ // ������ mp
    // ��������� ��������
    g_map ref = fig::get_ref(MAP);
    convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"), Options());

    mp::mp_world M;
    for (fig::fig_world::const_iterator i=MAP.begin(); i!=MAP.end(); i++){
      if ((i->depth >=50) && (i->depth <400)){
        zn::zn_key k = zn::get_key(*i);
        M.push_back(zconverter.fig2mp(*i, cnv, k.type));
      }
    }
    ofstream out(out_file.c_str());
    mp::write(out, M);
  }
}
