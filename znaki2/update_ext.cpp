// ���������� ����� �� �������� ��������� (fig ��� mp)

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
    cerr << "usage: update_ext <map1> <map2> <conf file> <file.mp|file.fig>\n";
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
  g_map old_ref = get_ref(MAP);

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
�� ����� ����� ��� ��������� ������ ������� � ������!
�� ������ �� 
*/

  
  fig::fig_world NEW; // ����� ����� - ���� ����� ������ ������ �������

  std::map<int, fig::fig_object> new_objects; // �� id � source
  std::map<int, fig::fig_object> old_objects; // �� id � source
  std::multimap<int, fig::fig_object> labels; // �� id � map



  std::cerr << "Reading new map: ";  
  if (fig_not_mp){ // ������ fig
    fig::fig_world FIG = fig::read(infile.c_str());
    g_map new_ref = fig::get_ref(FIG);
    convs::map2map cnv(old_ref, new_ref);

    for (fig::fig_world::iterator i=FIG.begin(); i!=FIG.end(); i++){
      if ((i->type != 2) && (i->type != 3) && (i->type != 4)) continue;
      if ((i->comment.size()>1) && (i->comment[1]=="[skip]")) continue;
      if (!zn::is_map_depth(*i)) continue;
      zn::zn_key k = zn::get_key(*i);
      if ((k.type==0) || (k.map!=source) || (k.id==0)) continue;
      fig::fig_object o = *i;
      o.set_points(cnv.line_bck(*i));
      new_objects.insert(std::pair<int, fig::fig_object>(k.id, o));
    }
  } 
  else { // ������ mp
    mp::mp_world MP = mp::read(infile.c_str());
    convs::map2pt cnv(old_ref, Datum("wgs84"), Proj("lonlat"), Options());

    for (mp::mp_world::const_iterator i=MP.begin(); i!=MP.end(); i++){
      zn::zn_key k = zn::get_key(*i);
      if ((k.type==0) || (k.map!=source) || (k.id==0)) continue;
      fig::fig_object o = zconverter.mp2fig(*i, cnv, k.type);
      new_objects.insert(std::pair<int, fig::fig_object>(k.id, o));
    }
  }
  std::cerr << new_objects.size() << " objects\n";


  // ������ ������������ id ��������� ������ �����
  // ��������� ������ ������� � ���������� source � ���,
  // ��������� �������� ���
  int maxid=0;
  std::set<int> old_obj_ids;
  for (fig::fig_world::const_iterator i=MAP.begin(); i!=MAP.end(); i++){
    if (!zn::is_map_depth(*i)) continue;
    zn::zn_key k = zn::get_key(*i);
    if ((k.map == map_name) && (k.id!=0) && (k.source == source) && (k.sid != 0)){
      old_objects.insert(std::pair<int, fig::fig_object>(k.sid, *i));
      old_obj_ids.insert(k.id);
      continue;
    }
    NEW.push_back(*i);
  }

  // ������� � ������������ ��� �������� - � ���� ���
  // ��������� ������� � ������ ������� - ��� ��������� � ����� �����.
  for (fig::fig_world::const_iterator i=MAP.begin(); i!=MAP.end(); i++){
    if (zn::is_map_depth(*i)) continue;
    zn::zn_label_key k = zn::get_label_key(*i);
    if ((k.map == map_name) && (k.id!=0) && old_obj_ids.count(k.id)!=0){
      labels.insert(std::pair<int, fig::fig_object>(k.id, *i));
      continue;
    }
    NEW.push_back(*i);
  }




  // ������ ��������� ����� ���������������� �������, ���������� �� �� ������� � �.�...
  // ������� �� �������...

  int new_count = 0;
  int mod_count = 0;
  int old_count = 0;

  int l_o_count = 0;
  int l_m_count = 0;
  int l_n_count = 0;

  std::cerr << "Merging maps...\n";  
  for (std::map<int, fig::fig_object>::iterator i = new_objects.begin(); i!=new_objects.end(); i++){

    zn::zn_key key = zn::get_key(i->second);
    // ���� ������ ���� ����������, �� �������� �� ������ ������
    if ((key.map != source) || (key.id ==0)) continue;

    key.sid       = key.id;
    key.source    = key.map;
    key.map       = map_name;

    map<int, fig::fig_object>::iterator o = old_objects.find(i->first);

    if (o==old_objects.end()){  // ��� - ����� ������
      maxid++;
      key.id = maxid;
      key.time.set_current();
      new_count++;
    }
    else {
      zn::zn_key old_key = zn::get_key(o->second);
      key.id = old_key.id;
     
      // ���� ������ ��������� - ������� ����� � �����.
      // ���������� ���, ����� � ����������
      if ((key.type != old_key.type) || 
          (i->second.text != o->second.text) || 
          (i->second != o->second)){
        key.time.set_current();
        mod_count++;
      } else old_count++;
    }  

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

  std::cerr << " --- map objects:\n";
  std::cerr << new_count << " new\n";
  std::cerr << mod_count << " modified\n";
  std::cerr << old_count << " non-modified\n";
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
