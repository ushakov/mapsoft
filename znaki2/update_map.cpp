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

  bool fig_not_mp;
  if      (testext(infile, ".fig")) fig_not_mp=true;
  else if (testext(infile, ".mp"))  fig_not_mp=false;
  else usage();

  // ������ ������ �����
  std::cerr << "Reading old map...\n";  
  fig::fig_world MAP = fig::read(file.c_str());

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

  zn::zn_conv zconverter(conf_file);


  // ����� �����
  fig::fig_world FIG; 
  // ���� �� ����������� �� fig - ��������� ����
  // ��� ������� �� fig-�����
  // ���� �� mp - �� ���������������� ������� ������� �� mp, 
  // � ��� ��������� - �� ������ �����!
  std::cerr << "Reading new map...\n";  
  if (fig_not_mp){ // ������ fig
    FIG = fig::read(infile.c_str());
  } 
  else { // ������ mp
    mp::mp_world MP = mp::read(infile.c_str());
    // �������� � FIG �� MAP ��� ������������������ �������
    for (fig::fig_world::const_iterator i=MAP.begin(); i!=MAP.end(); i++)
      if (!zn::is_map_depth(*i)) FIG.push_back(*i);

    // ��������� �������� �� ������ �����:
    g_map ref = fig::get_ref(MAP);
    convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"), Options());

    // ����������� ������� �� MP � FIG
    // ��� ����� �������� (��� �����) ��������� �������� ���� - ������ � �����
    for (mp::mp_world::const_iterator i=MP.begin(); i!=MP.end(); i++)
      FIG.push_back(zconverter.mp2fig(*i, cnv));
    
  }
  // ������ � FIG � ��� ����� ����� � ���������.


  // ������ ������������ id ��������� ������ �����
  // ��������� ������ ������� � ���
  map<int, fig::fig_object> objects;
  int maxid=0;
  for (fig::fig_world::const_iterator i=MAP.begin(); i!=MAP.end(); i++){
    if (!zn::is_map_depth(*i)) continue;
    zn::zn_key key = zn::get_key(*i);
    if ((key.map != map_name) || (key.id == 0)) continue;
    objects.insert(pair<int, fig::fig_object>(key.id, *i));
    if (key.id > maxid) maxid=key.id;
  }

  // ��������� ����� ������� � ���
  multimap<int, fig::fig_object> labels;
  for (fig::fig_world::const_iterator i=FIG.begin(); i!=FIG.end(); i++){
    if (zn::is_map_depth(*i)) continue;
    zn::zn_label_key key = zn::get_label_key(*i);
    if ((key.map != map_name) || (key.id == 0)) continue;
    labels.insert(pair<int, fig::fig_object>(key.id, *i));
  }


  //  ������� ������ ����� � ����� �� ��������� ���������� �� FIG
  // � � NC ����� ���������� �������, ������� �� �� ������ �������������...
  MAP.clear();
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
      continue;
    }
    if (i->type == -6) continue;

    // ������������������ �������
    if (!zn::is_map_depth(*i)) {
      if (i->comment.size()>1){ 
         if (i->comment[1]=="[skip]") continue;
         zn::zn_label_key k = zn::get_label_key(*i);
         if ((k.id!=0) && (k.map==map_name)) continue; // ������� ��� �� �����
      }
      MAP.push_back(*i); 
      continue;
    } 

    // ���������������� �������
    zn::zn_key key = zn::get_key(*i);

    if ((key.map == map_name) && (key.id !=0)){ // � ������� ���� ���� �� ���� �����

      zn::zn_key oldkey;
      map<int, fig::fig_object>::iterator o = objects.find(key.id);
      if (o==objects.end()){
        cerr << "��������: ������ " << key.id << " ��� ������\n";
        //... ������� ������
//        NC.push_back(*i);
//        continue;
        key.time.set_current();
        key.sid    = 0;
        key.source = source;
        key.id     = maxid;
        key.map    = map_name;
      } else {
        oldkey = zn::get_key(o->second);
        if (oldkey.time > key.time){
          cerr << "��������: ������ " << key.id << " ��� �������\n";
          //... ������� ������
//          NC.push_back(*i);
//          continue; 
          key.sid    = 0;
          key.source = source;
          key.time.set_current();
        } else if ( *i != o->second){
          key.time.set_current();
          key.sid    = 0;
          key.source = source;
        } else key = oldkey;
      }
      // ... ��������� �� ��� ���������, ����� ��� ����������
      // ������� ���� ���������� � ����� ������!

      zn::add_key(*i, key);  // ������� ����������� ����
      MAP.push_back(*i);            // ������� ������ 

      // ������ ��� �������:
      // ������� �� ���� ������� ��� ����� �������
      for (multimap<int, fig::fig_object>::const_iterator l = labels.find(key.id); 
          (l != labels.end()) && (l->first == key.id); l++){
        // ����� ���� �� ��� ���������� ��������, ����� ������ �������,
        // �������� ������� ���������� � �.�.
        MAP.push_back(l->second);
      }

      continue; 
    } 
    // �������� ������� ��� ����� ��� � �������� ������
    maxid++;
    if (key.type == 0) key.type = zconverter.get_type(*i);
    if (key.type == 0) {
      cerr <<  "������ ������������ ����\n";
      NC.push_back(*i);
      continue;
    }
    key.time.set_current();
    key.id     = maxid;
    key.map    = map_name;
    key.source = source;
    key.sid    = 0;
    zn::add_key(*i, key);  // ������� ����
    MAP.push_back(*i);            // ������� ������
    list<fig::fig_object> new_labels = zconverter.make_labels(*i); // ���������� ����� �������
    MAP.insert(MAP.end(), new_labels.begin(), new_labels.end());   
  }

  std::cerr << MAP.size() << " objects converted\n";
  std::cerr << NC.size() << " objects not converted\n";
 
  // ���������� MAP
  ofstream out(file.c_str());
  fig::write(out, MAP);
  ofstream outnc(ncfile.c_str());
  fig::write(outnc, NC);

}
