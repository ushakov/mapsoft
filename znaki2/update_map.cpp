// ���������� ����� �� fig ��� mp �����

#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "../geo_io/geofig.h"
#include "../libmp/mp.h"
#include "../geo_io/geo_convs.h"

#include "../libzn/zn.h"
#include "../libzn/zn_key.h"

using namespace std;

void usage(){
    cerr << "usage: update_map [--add] <map> <source> <conf file> <file.mp|file.fig>\n";
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

  // ��������� �� ������ �������?
  bool add=false;
  if ((argc>1) && (strcmp(argv[1],"--add")==0)){
    argv++; argc--;
    add=true;
  }

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
  std::cerr << "Reading old map: " << file << "\n";  
  fig::fig_world MAP;
  if (!fig::read(file.c_str(), MAP)) {cerr << "Bad fig file " << file << "\n"; exit(0);}

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

  std:set<int> ids; // ��� �������� �� ������� ������...

  if (fig_not_mp){ // ������ fig
    std::cerr << "Reading new map: " << infile <<"\n";  
    fig::fig_world FIG;
    if (!fig::read(infile.c_str(), FIG)) {cerr << "Bad fig file " << infile << "\n"; exit(0);}
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
      if (!zconverter.is_map_depth(*i)) {
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
        zn::zn_key k = zn::get_key(*i);
        if (k.id!=0){
          if (ids.find(k.id) != ids.end()) { zn::clear_key(*i);}
          else ids.insert(k.id);
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
      if (!zconverter.is_map_depth(*i)) {
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
    std::cerr << "Getting ref...\n";
    g_map ref = fig::get_ref(MAP);
 
    std::cerr << "Make CNV...\n";
    convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"));

    std::cerr << "Reading new map: " << infile <<"\n";  
    mp::mp_world MP;
    if (!mp::read(infile.c_str(), MP)){cerr << "Bad mp file " << infile << "\n"; exit(0);}

    // mp->fig
    std::cerr << "mp->fig\n";  
    for (mp::mp_world::const_iterator i=MP.begin(); i!=MP.end(); i++){
      int type = zconverter.get_type(*i);
      fig::fig_object fig = zconverter.mp2fig(*i, cnv, type);
      new_objects.insert(std::pair<int, fig::fig_object>(type, fig));
    }
  }
  std::cerr << "ok\n";  


  // ������ ������������ id ��������� ������ �����
  // ��������� ������ ������� � ���
  // ��� �������� �� ��������� ������ ������������ ������ �� �����!
  int maxid=0;
  for (fig::fig_world::const_iterator i=MAP.begin(); i!=MAP.end(); i++){
    if (!zconverter.is_map_depth(*i)) continue;
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

  int l_o_count = 0;
  int l_m_count = 0;
  int l_n_count = 0;

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
      NEW.push_back(i->second);     // ������� ������
      list<fig::fig_object> l1 = zconverter.make_labels(i->second, key.type); // ��������� ����� �������
      add_key(l1, zn::zn_label_key(key));
      NEW.insert(NEW.end(), l1.begin(), l1.end());   
      l_n_count+=l1.size();
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
        zn::clear_key(i->second);
        NEW.push_back(i->second);
        continue;
      }
      zn::zn_key oldkey = zn::get_key(o->second);
      // ���� ������ ������ ����� ������
      if (oldkey.time > key.time){
        cerr << "��������: ������ " << key.id << " ��� �������,\n";
        cerr << "� �� ������ ������ ��������� ��������... :( � ������� 11 ��!\n";
        i->second.depth = 11; con_count++;
        zn::clear_key(i->second);
        NEW.push_back(i->second);
        // ����� ������ �������� ������...
        i->second = o->second;
        key = oldkey;
      }
      // ���� ������ ��������� - ������� ����� � �����
      // ���������� ���, ����� � ����������
      if ((i->first != oldkey.type) || 
          (i->second.text != o->second.text) || 
          (i->second != o->second)){
        key.time.set_current();
//        key.source = source; // �� ���� ������!!!
        mod_count++;
      } else old_count++;

      zn::add_key(i->second, key);  // ������� ����������� ����
      NEW.push_back(i->second);     // ������� ������

      // ������ ��� �������:
      // ���� � ������� ���� ��������, �� ��� ������� - ������� ��
      if ((i->second.comment.size()>0) && 
          (i->second.comment[0] != "") &&
          (labels.count(key.id) == 0)){
        list<fig::fig_object> l1 = zconverter.make_labels(i->second, key.type); // ��������� ����� �������
        add_key(l1, zn::zn_label_key(key));
        NEW.insert(NEW.end(), l1.begin(), l1.end());   
        l_n_count+=l1.size();
        continue;
      } 
      // ������� �� ���� ��� ������ ������� ��� ����� �������
      for (multimap<int, fig::fig_object>::iterator l = labels.find(key.id); 
          (l != labels.end()) && (l->first == key.id); l++){
        // ����� ������� = �������� �������
        std::string text = (i->second.comment.size()>0)? i->second.comment[0]:"";
        if (text != l->second.text){
          l->second.text = text;
          l_m_count++;
        } else l_o_count++;

        if (text !="") NEW.push_back(l->second);
      }

    }
  }
  // ���� ���� - �������� ������ ������� � ������� � ���:
  if (add){
    for (std::map<int, fig::fig_object>::const_iterator 
        o=old_objects.begin(); o!=old_objects.end(); o++){
      NEW.push_back(o->second);
      old_count++;
      for (multimap<int, fig::fig_object>::iterator l = labels.find(o->first);
          (l != labels.end()) && (l->first == o->first); l++){
        NEW.push_back(l->second);
        l_o_count++;
      }
    }
  }


  std::cerr << " --- map objects:\n";
  std::cerr << new_count << " new\n";
  std::cerr << mod_count << " modified\n";
  std::cerr << old_count << " non-modified\n";
  std::cerr << unk_count << " unknown objects (see depth 10)\n";
  std::cerr << con_count << " conflicts (see depth 11)\n";
  std::cerr << " --- labels:\n";
  std::cerr << l_n_count << " new\n";
  std::cerr << l_m_count << " modified\n";
  std::cerr << l_o_count << " non-modified\n";
  std::cerr << " ---\n";
  std::cerr << NEW.size() << " fig objects \n";
  // ���������� MAP
  ofstream out(file.c_str());
  fig::write(out, NEW);

}
