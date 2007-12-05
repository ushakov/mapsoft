// ���������� ����� �� fig ��� mp �����

#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "../geo_io/geofig.h"
#include "../geo_io/mp.h"
#include "../geo_io/geo_convs.h"

#include "zn.h"

using namespace std;

void usage(){
    cerr << "usage: update_map_gk <map> <source> <conf file> <file.mp|file.fig> <nc.fig>\n";
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

  if (argc != 6) usage();
  string map_name  = argv[1];
  string source    = argv[2];
  string conf_file = argv[3];
  string infile    = argv[4];
  string ncfile    = argv[5];
  string file = maps_dir+"/"+map_name+".fig";

  // ������� ��������� �����
  std::cerr << "Reading old map...\n";  
  fig::fig_world MAP = fig::read(file.c_str());
  if (MAP.size()==0) {
    cerr << "bad file " << file << "\n";
    exit(0);
  }
  // ��������� ��������
  std::cerr << "Getting old ref...\n";  
  g_map ref = fig::get_ref(MAP);
  convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"), Options());

  zn::zn_conv zconverter(conf_file);

  // backup �������� �����!
  std::cerr << "Backup...\n";  
  int ver_num = 0;
  std::string backup;
  do{ 
    ostringstream fn;
    fn << maps_dir << "/" << map_name << std::setw(6) << std::setfill('0') << ver_num << ".fig";
    backup = fn.str();
    ifstream test(backup.c_str());
    if (!test) break;
    ver_num++;
  } while (true);
  ofstream bu(backup.c_str());
  fig::write(bu, MAP);
  // ... ������� diff?
  

  fig::fig_world FIG; 
  // ��������� ��� ����� �����
  // ���� �� ����������� �� fig - ��������� ����
  // ��� ������� �� fig-�����
  // ���� �� mp - �� ����� � �.�. ������� �� ������� �����,
  // � ������� �����������

  std::cerr << "Reading new map...\n";  
  if (testext(infile, ".fig")){ // ������ fig
    FIG = fig::read(infile.c_str());
    /// ... ������������� ����������!
  } else
  if (testext(infile, ".mp")){ // ������ mp
    mp::mp_world MP = mp::read(infile.c_str());
    // �������� � FIG �� MAP ��� ������� � ��������� 1-44 � 400-999
    // (�����, �������� � �.�.)
    for (fig::fig_world::const_iterator i=MAP.begin(); i!=MAP.end(); i++)
      if ((i->depth <45) || (i->depth >=400)) FIG.push_back(*i);
    // ����������� ������� �� MP � FIG
    // ��� ����� �������� (��� �����) ��������� �������� ���� - ������ � �����
    for (mp::mp_world::const_iterator i=MP.begin(); i!=MP.end(); i++)
      FIG.push_back(zconverter.mp2fig(*i, cnv));
  } else usage();
  // ������ � FIG � ��� ����� ����� � ���������.


  // ������ ������������ id ��������� ������ �����
  // ��������� ������� � ������� � ����
  map<int, fig::fig_object> objects;
  multimap<int, fig::fig_object> labels;

  std::cerr << "Reading old map...\n";  
  int maxid=0;
  for (fig::fig_world::const_iterator i=MAP.begin(); i!=MAP.end(); i++){
    if ((i->depth < 50)||(i->depth>=400)) continue;
    zn::zn_key key = zconverter.get_key(*i);
    if ((key.map != map_name) || (key.id == 0)) continue;
    if (key.label) labels.insert(pair<int, fig::fig_object>(key.id, *i));
    else objects.insert(pair<int, fig::fig_object>(key.id, *i));
    if (key.id > maxid) maxid=key.id;
  }

  //  ������� ������ ����� � ����� �� ��������� ���������� �� FIG
  MAP.clear();
  // � � NC ����� ���������� �������, ������� �� �� ������ �������������...
  fig::fig_world NC; 

  std::cerr << "Merging maps...\n";  
  for (fig::fig_world::iterator i=FIG.begin(); i!=FIG.end(); i++){

    if (i->type == 6){ // ��������� ������
      // �������� ����������� � ��������� ������ (�� ��������� �������� �������!).
      fig::fig_world::iterator j = i; j++;
      if (j!=FIG.end()){
        if (j->comment.size()< i->comment.size()) j->comment.resize(i->comment.size());
        for (int n=0; n<i->comment.size(); n++) j->comment[n] = i->comment[n];
      }
    }

    // ������� � ��������� 1-44 � 400-999 (�����, �������� � �.�.)
    if ((i->depth <45) || (i->depth >=400)) {
      MAP.push_back(*i); 
      continue;
    } 
    // ������� � ��������� 45-49 (��������, ������� ��� _������_ �� �����)
    if ((i->depth >=45) && (i->depth <50)) continue;

    // ��-����� - �������� ��� ���������
    if ((i->type !=2) && (i->type !=3) && (i->type !=4)){
      MAP.push_back(*i); 
      continue;
    }

    // ��������� �������
    zn::zn_key key = zconverter.get_key(*i);

    if ((key.map == map_name) && (key.id !=0)){ // ���� ������ ���� �� ���� �����
      map<int, fig::fig_object>::iterator o = objects.find(key.id);
      if (o==objects.end()){
        cerr << "��������: ������ " << key.id << " ��� ������\n";
        cerr << "�� �������� ���!\n";
        //... ������� ������
        NC.push_back(*i);
        continue;
      }
      zn::zn_key oldkey = zconverter.get_key(o->second);
      if (oldkey.time > key.time){
        cerr << "��������: ������ " << key.id << " ��� �������\n";
        cerr << "�� �������� ���!\n";
        //... ������� ������
        NC.push_back(*i);
        continue;
      }
      // ... ��������� ��� ���������, ����� ��� ����������
      // ������� ���� ���������� � ����� ������!

      key.time.set_current();
      key.sid    = 0;
      key.source = source;

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
        new_labels = zconverter.make_labels(*i);
      }
      MAP.insert(MAP.end(), new_labels.begin(), new_labels.end());  // �������� �������
      continue;
    } 
    // �������� ������� ��� ����� ��� � �������� ������
    maxid++;
    if (key.type == 0) key.type = zconverter.get_type(*i);
    if (key.type == 0) {
      NC.push_back(*i);
      continue;
    }
    key.time.set_current();
    key.id     = maxid;
    key.map    = map_name;
    key.source = source;
    key.sid    = 0;
    zconverter.add_key(*i, key);  // ������� ����
    MAP.push_back(*i);            // ������� ������
    list<fig::fig_object> new_labels = zconverter.make_labels(*i); // ���������� ����� �������
    MAP.insert(MAP.end(), new_labels.begin(), new_labels.end());        // �������� �������
  }

  std::cerr << MAP.size() << " objects converted\n";
  std::cerr << NC.size() << " objects not converted\n";
  for (fig::fig_world::iterator i = MAP.begin(); i!=MAP.end(); i++){
    std::cerr << i->type << " ";
  }
 
  // ���������� MAP
  ofstream out(file.c_str());
  fig::write(out, MAP);
  ofstream outnc(ncfile.c_str());
  fig::write(outnc, NC);

}
