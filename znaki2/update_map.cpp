// ���������� ����� �� fig ��� mp �����

#include <string>
#include "../geo_io/geofig.h"
#include "../geo_io/mp.h"
#include "../geo_io/geo_convs.h"

#include "zn.h"

using namespace std;

void usage(){
    cerr << "usage: update_map_gk <map> <source> <conf file> <file.mp|file.fig>\n";
    exit(0);
}


const string maps_dir  = "./maps";

// �������� ����������
bool testext(const string & nstr, const char *ext){
  int pos = nstr.rfind(ext);
  return ((pos>0)&&(pos == nstr.length()-strlen(ext)));
}

//
main(int argc, char** argv){

  if (argc != 5) usage();
  string map_name  = argv[1];
  string source    = argv[2];
  string conf_file = argv[3];
  string infile    = argv[4];
  string file = maps_dir+"/"+map_name+".fig";

  // ������� ��������� �����
  fig::fig_world MAP = fig::read(file.c_str());
  if (MAP.size()==0) {
    cerr << "bad file " << file << "\n";
    exit(0);
  }
  // ��������� ��������
  g_map ref = fig::get_ref(MAP);

  zn::zn_conv zconverter(conf_file);

// ����� ���� ������� backup �������� �����!!!!!


  fig::fig_world FIG; // ��������� ��� ����� �����
  
  if (testext(infile, ".fig")){ // ������ fig
    FIG = fig::read(infile.c_str());
  } else
  if (testext(infile, ".mp")){ // ������ mp
    mp::mp_world MP = mp::read(infile.c_str());
    // �������� � FIG �� MAP ��� ������� � ��������� 1-29 � 200-999
    // (�����, �������� � �.�.)
    for (fig::fig_world::const_iterator i=MAP.begin(); i!=MAP.end(); i++)
      if ((i->depth <30) || (i->depth >=200)) FIG.push_back(*i);
    // ����������� ������� �� MP � FIG
    for (mp::mp_world::const_iterator i=MP.begin(); i!=MP.end(); i++)
      FIG.push_back(zconverter.mp2fig(*i, ref));
  } else usage();
  // ������ � FIG � ��� ����� ����� � ���������.


  // ������ ������������ id ��������� ������ �����
  // ��������� ������� � ������� � ����
  map<int, fig::fig_object> objects;
  multimap<int, fig::fig_object> labels;

  int maxid=0;
  for (fig::fig_world::const_iterator i=MAP.begin(); i!=MAP.end(); i++){
    zn::zn_key key = zconverter.get_key(*i);
    if ((key.map_name != map_name) || (key.id == 0)) continue;
    if (key.label) labels.insert(pair<int, fig::fig_object>(key.id, *i));
    else objects.insert(pair<int, fig::fig_object>(key.id, *i));
    if (key.id > maxid) maxid=key.id;
  }

  //  ������� ������ ����� � ����� �� ��������� ���������� �� FIG
  MAP.clear();

  for (fig::fig_world::const_iterator i=FIG.begin(); i!=FIG.end(); i++){
    // ������� � ��������� 1-29 � 200-999 (�����, �������� � �.�.)
    if ((i->depth <30) || (i->depth >=200)) {
      MAP.push_back(*i); 
      continue;
    }
    // ��������� �������
    zn::zn_key key = zconverter.get_key(*i);
    if ((key.map_name == map_name) && (key.id !=0)){ // ���� ������ ���� �� ���� �����
      map<int, fig::fig_object>::iterator o = objects.find(key.id);
      if (o==objects.end()){
        cerr << "��������: ������ " << key.id << " ��� ������\n";
        cerr << "�� �������� ���!\n";
        //... ������� ������
        continue;
      }
      zn::zn_key oldkey = zconverter.get_key(o->second);
      if (oldkey.time > key.time){
        cerr << "��������: ������ " << key.id << " ��� �������\n";
        cerr << "�� �������� ���!\n";
        //... ������� ������
        continue;
      }
      // ... ��������� ��� ���������, ����� ��� ������� 
      // ������� ���� ���������� � ����� ������!
      key.time.set_current();
      key.source_id  = 0;
      key.source     = source;

      zconverter.add_key(*i, key);  // ������� ����
      MAP.push_back(*i);            // ������� ������ 

      // ������ ��� �������:
      list<fig::fig_object> new_labels;
      Point<int> shift;
      if (i->isshifted(o->second, shift)){
        // ���� ������ � ����� ������� ����������� ������ ��������� -
        // ���������� � new_labels ������ �������, ������� ��
        for (multimap<int, fig::fig_object>::const_iterator l = labels.find(key.id); 
              (l != labels.end()) && (l->first == key.id); l++){
          new_labels.push_back(l->second + shift);
        }
      } else { 
        // ����� - ��������� ������� ������
        new_labels = zconverter.make_labels(*i, key);
      }
      MAP.insert(MAP.end(), new_labels.begin(), new_labels.end());  // �������� �������

    } 
    else { // ������ ��� �����
      maxid++;
      key = zconverter.make_key(*i);            // ������� ����� ����
      key.time.set_current();
      key.id        = maxid;
      key.map_name  = map_name;
      key.source    = source;
      key.source_id = 0;
      zconverter.add_key(*i, key);  // ������� ����
      MAP.push_back(*i);            // ������� ������
      list<fig::fig_object> new_labels = zconverter.make_labels(*i, key); // ���������� ����� �������
      MAP.insert(MAP.end(), new_labels.begin(), new_labels.end());        // �������� �������
    }
  }

  // ���������� MAP
  fig::write(MAP, file.c_str());

}
