// ���������� ����� �� fig ��� mp �����

#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "../geo_io/geofig.h"
#include "../geo_io/mp.h"
#include "../geo_io/geo_convs.h"

#include "zn.h"
#include "zn_key.h"

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

/*
    ��� ��� ���������:

 �����:           fig  mp
�������.�������   +    +
�������           +    -
������ �������    +    -
 ������:
�������.�������   +    +
�������           -    +
������ �������    -    +
*/

  
  fig::fig_world NEW; // ����� ����� - ���� ����� ������ ������ �������
  std::multimap<int, fig::fig_object> new_objects; // �� ����
  std::map<int, fig::fig_object> old_objects;      // �� id
  std::multimap<int, fig::fig_object> labels;      // �� id �������


  int unk_count=0;

  std::cerr << "Reading new map...\n";  
  if (fig_not_mp){ // ������ fig
    fig::fig_world FIG = fig::read(infile.c_str());
    for (fig::fig_world::iterator i=FIG.begin(); i!=FIG.end(); i++){

      if (i->type == 6){ // ��������� ������
        // �������� ����������� � ��������� ������ (�� ��������� �������� �������!).
        // ��������� ��� �� �����
        fig::fig_world::iterator j = i; j++;
        if (j!=FIG.end()){
          if (j->comment.size()< i->comment.size()) j->comment.resize(i->comment.size());
          for (int n=0; n<i->comment.size(); n++) j->comment[n] = i->comment[n];
        }
        continue;
      }
      if (i->type == -6) continue;
      if ((i->comment.size()>1) && (i->comment[1]=="[skip]")) continue;

      // ������������������ �������
      if (!zn::is_map_depth(*i)) {
        if (i->comment.size()>1){ 
          zn::zn_label_key k = zn::get_label_key(*i);
          if ((k.id!=0) && (k.map==map_name)){// �������
            labels.insert(std::pair<int, fig::fig_object>(k.id, *i)); 
            continue;
          }
        }
        NEW.push_back(*i); 
        continue;
      } 
      else {
        int type = zconverter.get_type(*i);
        if (type==0) {
          cerr << "can't determin object type! Making it's depth=10!!!\n";
          i->depth=10; unk_count++;
          NEW.push_back(*i);
          continue;
        }
        new_objects.insert(std::pair<int, fig::fig_object>(type, *i));
      }
    }
  } 
  else { // ������ mp
    // ������ �� ������ ����� ������������������ ������� � �������
    //��������� �������� ����� �� ���������, �� �� ������ ��. � ��� ������������ ��������...
    for (fig::fig_world::iterator i=MAP.begin(); i!=MAP.end(); i++){
      if (i->type == 6){ // ��������� ������
        // �������� ����������� � ��������� ������ (�� ��������� �������� �������!).
        // ��������� ��� �� �����
        fig::fig_world::iterator j = i; j++;
        if (j!=MAP.end()){
          if (j->comment.size()< i->comment.size()) j->comment.resize(i->comment.size());
          for (int n=0; n<i->comment.size(); n++) j->comment[n] = i->comment[n];
        }
        continue;
      }
      if (i->type == -6) continue;
      if ((i->comment.size()>1) && (i->comment[1]=="[skip]")) continue;

      // ������������������ �������
      if (!zn::is_map_depth(*i)) {
        if (i->comment.size()>1){ 
          zn::zn_label_key k = zn::get_label_key(*i);
          if ((k.id!=0) && (k.map==map_name)){// �������
            labels.insert(std::pair<int, fig::fig_object>(k.id, *i)); 
            continue;
          }
        }
        NEW.push_back(*i); 
        continue;
      } 
    }

    // ��������� �������� �� ������ �����:
    g_map ref = fig::get_ref(MAP);
    convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"), Options());

    mp::mp_world MP = mp::read(infile.c_str());

    // mp->fig
    for (mp::mp_world::const_iterator i=MP.begin(); i!=MP.end(); i++){
      int type = zconverter.get_type(*i);
      fig::fig_object fig = zconverter.mp2fig(*i, cnv, type);
      // ����������� ������� ���� ������ ���� �� ������� 10
      // �� �� ������ ������ ������������...
      if (type==0) {
        cerr << "can't determin object type! Making it's depth=10!!!\n";
        fig.depth=10; unk_count++;
        NEW.push_back(fig);
        continue;
      }
      new_objects.insert(std::pair<int, fig::fig_object>(type, fig));
    }
  }


  // ������ ������������ id ��������� ������ �����
  // ��������� ������ ������� � ���
  // ��� �������� �� ��������� ������ ������������ ������ �� �����!
  int maxid=0;
  for (fig::fig_world::const_iterator i=MAP.begin(); i!=MAP.end(); i++){
    if (!zn::is_map_depth(*i)) continue;
    zn::zn_key key = zn::get_key(*i);
    if ((key.map != map_name) || (key.id == 0)){
      std::cerr << "strange object in map storage! skipping...\n";
      continue;
    }
    old_objects.insert(pair<int, fig::fig_object>(key.id, *i));
    if (key.id > maxid) maxid=key.id;
  }




  // ������ ��������� ����� ���������������� �������, ���������� �� �� ������� � �.�...
  // ������� �� �������...

  int new_count = 0;
  int mod_count = 0;
  int old_count = 0;
  int con_count = 0;

  std::cerr << "Merging maps...\n";  
  for (std::multimap<int, fig::fig_object>::iterator i = new_objects.begin(); i!=new_objects.end(); i++){

    zn::zn_key key = zn::get_key(i->second);

    // ��� ������: ���� � ������� ��� �����, ���� ����. ������ ������ �����:
    if ((key.map != map_name) || (key.id ==0)){ 
      maxid++; new_count++;
      key.type   = i->first; // ��� ������������ �� ���� �������
      key.time.set_current();
      key.id     = maxid;
      key.map    = map_name;
      key.source = source;
      key.sid    = 0;
      zn::add_key(i->second, key);  // ������� ����
      MAP.push_back(i->second);     // ������� ������
      list<fig::fig_object> l1 = zconverter.make_labels(i->second); // ��������� ����� �������
      MAP.insert(MAP.end(), l1.begin(), l1.end());   
    }
    else { // � ������� ���� ����. ��� ��� �������
      key.type = i->first; // �����, ���� �� ������. ��� ������ ������� - �� ��� ����.
      // ���� ������� ������� � ����� id ���, ������ ��� �������
      // ����� ������ ����� �� ��������������
      map<int, fig::fig_object>::iterator o = old_objects.find(key.id);
      if (o==old_objects.end()){
        cerr << "��������: �� ������� ������ " << key.id << " ��� ������,\n";
        cerr << "� �� ��� ����� ���� �����������... :( � ������� 11 ���!\n";
        i->second.depth = 11; con_count++;
        MAP.push_back(i->second);
        continue;
      }
      zn::zn_key oldkey = zn::get_key(o->second);
      // ���� ������ ������ ����� ������
      if (oldkey.time > key.time){
        cerr << "��������: ������ " << key.id << " ��� �������,\n";
        cerr << "� �� ������ ������ ��������� ��������... :( � ������� 11 ��!\n";
        i->second.depth = 11; con_count++;
        MAP.push_back(i->second);
        // ����� ������ �������� ������...
        i->second = o->second;
        key = oldkey;
      }
      // ���� ������ ��������� - ������� ����� � ����� � ��������.
      // ���������� ���, ����� � ����������
      if ((i->first != oldkey.type) || 
          (i->second.text != o->second.text) || 
          (i->second != o->second)){
        key.time.set_current();
        key.source = source;
        mod_count++;
      } else old_count++;

      zn::add_key(i->second, key);  // ������� ����������� ����
      MAP.push_back(i->second);     // ������� ������

      // ������ ��� �������:
      // ���� � ������� ���� ��������, �� ��� ������� - ������� ��
      if ((i->second.comment.size()>0) && 
          (i->second.comment[0] != "") &&
          (labels.count(key.id) == 0)){
        list<fig::fig_object> l1 = zconverter.make_labels(i->second); // ��������� ����� �������
        MAP.insert(MAP.end(), l1.begin(), l1.end());   
        continue;
      } 
      // ������� �� ���� ��� ������ ������� ��� ����� �������
      for (multimap<int, fig::fig_object>::iterator l = labels.find(key.id); 
          (l != labels.end()) && (l->first == key.id); l++){
        // ����� ������� = �������� �������
        if (i->second.comment.size()>0) l->second.text = i->second.comment[0];
        else l->second.text = "";

        if (l->second.text !="") MAP.push_back(l->second);
      }

    }
  }

  std::cerr << new_count << " new objects\n";
  std::cerr << mod_count << " modified objects\n";
  std::cerr << old_count << " non-modified objects\n";
  std::cerr << unk_count << " unknown objects (see depth 10)\n";
  std::cerr << con_count << " conflicts (see depth 11)\n";
  std::cerr << MAP.size() << " objects \n";
  // ���������� MAP
  ofstream out(file.c_str());
  fig::write(out, MAP);

}
