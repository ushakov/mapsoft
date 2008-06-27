
#include <string>
#include <fstream>
#include "../geo_io/geofig.h"
#include "../libmp/mp.h"

#include "../libzn/zn.h"
#include "../libzn/zn_key.h"

using namespace std;

bool testext(const string & nstr, const char *ext){
    int pos = nstr.rfind(ext);
    return ((pos>0)&&(pos == nstr.length()-strlen(ext)));
}
bool testfilter(const string & filter, const string & arg, const zn::zn_key & k){
    return ((filter == "all") ||
            ((filter == "map")  && (arg == k.map)) ||
            ((filter == "nmap") && (arg != k.map)) ||
            ((filter == "src")  && (arg == k.source)) ||
            ((filter == "nsrc") && (arg != k.source)));
}
std::string filter_help(void){
  return  std::string("filters:\n") +
          "  all        -- all map objects\n" +
          "  map  <map> -- objects with keys having map=<map>\n" +
          "  map  ""    -- objects without keys\n" +
          "  nmap <map> -- objects with keys having map!=<map> \n" +
          "  nmap ""    -- objects with keys\n" +
          "  src  <src> -- objects with keys having source=<src> \n" +
          "  nsrc <src> -- objects with keys having source!=<src> \n";
}

void copy_comment(const fig::fig_world::iterator & i, const fig::fig_world::iterator & end){
  if (i->type == 6){ // составной объект
    // копируем первые непустые строки комментария в следующий объект
    // остальное нам не нужно
    fig::fig_world::iterator j=i; j++;

    // пропускаем подписи
    while ((j!=end) && (j->comment.size()>1) && (j->comment[1]=="[skip]")) j++;

    if ((j!=end) && (i->comment.size()>0)){
      if (j->comment.size()< i->comment.size()) j->comment.resize(i->comment.size());
      for (int n=0; n<i->comment.size(); n++) j->comment[n] = i->comment[n];

    }
  }
}



/*****************************************************/
/// Копировать картографические объекты из (mp|fig) в (mp|fig)
// в соответствии с системой знаков
// fig (как входной так и выходной) должен обязательно содержать привязку
// Если выходного файла mp нет - он создается

int copy(int argc, char** argv){

  if (argc < 4){
    cerr << "Copy map objects.\n"
         << "  usage: mapsoft_vmap copy <conf>  <in mp|fig> <out mp|fig> <filter> <filter args>\n"
         << "FIG files must have geo-reference.\n"
         << filter_help();
    return 1;
  }

  string cfile = argv[0];
  string ifile = argv[1];
  string ofile = argv[2];
  string filter = argv[3];
  string arg;
  if ((filter != "all") && 
      (filter != "map") && (filter != "nmap") &&
      (filter != "src") && (filter != "nsrc")){
    cerr << "unknown filter!\n";
    return 1;
  }
  if (filter!="all"){
    if (argc<5){
      cerr << "argument required!\n";
      return 1;
    }
    arg=argv[4];
  }

  std::cerr << "copying from " << ifile << " to " << ofile << ": ";  

  fig::fig_world IF, OF;
  mp::mp_world   IM, OM;

  zn::zn_conv zconverter(cfile);

  // читаем входной файл, преобразуем все в IM
  if (testext(ifile, ".fig")){
    if (!fig::read(ifile.c_str(), IF)) {
      cerr << "bad fig file " << ifile << "\n"; return 1;
    }
    g_map ref = fig::get_ref(IF);
    if (ref.size()<3){
      cerr << "not a GEO-fig\n"; return 1;
    }
    convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"));
    for (fig::fig_world::iterator i=IF.begin(); i!=IF.end(); i++){
      if (i->type==6) copy_comment(i, IF.end());
      if (!zconverter.is_map_depth(*i)) continue;
      IM.push_back(zconverter.fig2mp(*i, cnv));
    }
  }
  else if (testext(ifile, ".mp")){
    if (!mp::read(ifile.c_str(), IM)) {
      cerr << "bad mp file " << ifile << "\n"; return 1;
    }
  }
  else { cerr << "input file is not .fig or .mp\n"; return 1;}

  int obj_cnt=0;

  // читаем выходной файл, дописываем туда новые объекты (фильтруя их), записываем
  if (testext(ofile, ".fig")){
    if (!fig::read(ofile.c_str(), OF)) {
      cerr << "bad fig file " << ofile << "\n"; return 1;
    }
    g_map ref = fig::get_ref(OF);
    if (ref.size()<3){
      cerr << "not a GEO-fig\n"; return 1;
    }
    convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"));
    for (mp::mp_world::const_iterator i=IM.begin(); i!=IM.end(); i++){
      if ((filter=="all") || testfilter(filter, arg, zn::get_key(*i))){
        obj_cnt++;
        OF.push_back(zconverter.mp2fig(*i, cnv));
      }
    }
    ofstream out(ofile.c_str());
    fig::write(out, OF);
  }
  else if (testext(ofile, ".mp")){
    mp::read(ofile.c_str(), OM);
    // если файла нет - OM остается пустым
    for (mp::mp_world::const_iterator i=IM.begin(); i!=IM.end(); i++){
      if ((filter=="all") || testfilter(filter, arg, zn::get_key(*i))){
        obj_cnt++;
        OM.push_back(*i);
      }
    }
    ofstream out(ofile.c_str());
    mp::write(out, OM);
  }
  else { cerr << "output file is not .fig or .mp\n"; return 1; }

  std::cerr << obj_cnt << " map objects copied\n";

  return 0;
}

/*****************************************************/
/// Удалить картографические объекты (fig|mp)
int remove(int argc, char** argv){

  if (argc < 3){
    cerr << "Remove map objects from mp or fig.\n"
         << "  usage: mapsoft_vmap remove <conf> <fig|mp> <filter> <filter arg>\n"
         << "FIG file must have geo-reference.\n"
         << filter_help();
    return 1;
  }

  string cfile  = argv[0];
  string file   = argv[1];
  string filter = argv[2];
  string arg;
  if ((filter != "all") && 
      (filter != "map") && (filter != "nmap") &&
      (filter != "src") && (filter != "nsrc")){
    cerr << "unknown filter!\n";
    return 1;
  }
  if (filter!="all"){
    if (argc<4){
      cerr << "argument required!\n";
      return 1;
    }
    arg=argv[3];
  }

  std::cerr << "removing map objects from " << file <<": ";  

  zn::zn_conv zconverter(cfile);

  int obj_cnt=0;
  
  if (testext(file, ".fig")){
    fig::fig_world F;
    if (!fig::read(file.c_str(), F)) {
      cerr << "bad fig file\n"; return 1;
    }

    fig::fig_world::iterator i=F.begin(); 
    while (i!=F.end()){
      if (zconverter.is_map_depth(*i) &&
          ((filter=="all") || testfilter(filter, arg, zn::get_key(*i)))) 
             {i=F.erase(i); obj_cnt++;}
      else i++;
    }

    ofstream out(file.c_str());
    fig::write(out, F);
  }

  else if (testext(file, ".mp")){
    mp::mp_world M;
    if (!mp::read(file.c_str(), M)) {
      cerr << "bad mb file\n"; return 1;
    }

    mp::mp_world::iterator i=M.begin(); 
    while (i!=M.end()){
      if ((filter=="all") || testfilter(filter, arg, zn::get_key(*i))) 
        {i=M.erase(i); obj_cnt++;}
      else i++;
    }

    ofstream out(file.c_str());
    mp::write(out, M);
  }
  else { cerr << "file is not .fig or .mp\n"; return 1; }

  std::cerr << obj_cnt << " map objects removed\n";

  return 0;
}


/*****************************************************/
///  Обновить подписи (fig)
// Рассматриваются только объекты с ключами, относящиеся к указанной карте.
// - если к объекту есть подписи - оставить их
// - если нет - создать
// - если у объекта поменялось название - поменять подпись
// - если исчез объект, к которому привязана подпись - удалить подпись

int labels(int argc, char** argv){
  if (argc != 3){
    cerr << "Update labels in fig.\n"
         << "  usage: mapsoft_vmap labels <conf> <map name> <fig>\n";
    return 1;
  }

  string cfile    = argv[0];
  string map_name = argv[1];
  string file     = argv[2];

  std::cerr << "updating labels in " << file <<": ";  
  if (!testext(file, ".fig")){ std::cerr << "file is not a .fig\n"; return 1;}

  fig::fig_world F;
  if (!fig::read(file.c_str(), F)) {
    cerr << "bad fig file\n"; return 1;
  }

  zn::zn_conv zconverter(cfile);

  //первый проход: удаляем подписи из файла, переносим их в multimap
  std::multimap<int, fig::fig_object> labels;      // по id объекта
  fig::fig_world::iterator i=F.begin();
  while (i!=F.end()){
    if (i->comment.size()>1){
      zn::zn_label_key k = zn::get_label_key(*i);
      if ((k.id!=0) && (k.map==map_name)){// подписи
        labels.insert(std::pair<int, fig::fig_object>(k.id, *i));
        i=F.erase(i);
        continue;
      }
    }
    i++;
  }

  // второй проход: для каждого объекта из карты берем подпись из multimap 
  // или создаем заново

  list<fig::fig_object> NEW;
  int l_n_count=0;
  int l_o_count=0;
  int l_m_count=0;

  for (i=F.begin(); i!=F.end(); i++){
    if (i->type==6) copy_comment(i, F.end());

    if (!zconverter.is_map_depth(*i)) continue;

    zn::zn_key key = zn::get_key(*i);
    if ((key.map != map_name) || (key.id ==0)) continue;

    // если у объекта есть название, но нет подписи - сделаем ее
    if ((i->comment.size()>0) &&
        (i->comment[0] != "") &&
        (labels.count(key.id) == 0)){
      list<fig::fig_object> l1 = zconverter.make_labels(*i, key.type); // изготовим новые подписи
      add_key(l1, zn::zn_label_key(key));
      NEW.insert(NEW.end(), l1.begin(), l1.end());
      l_n_count+=l1.size();
      continue;
    }
    // вытащим из хэша все старые подписи для этого объекта
    for (multimap<int, fig::fig_object>::iterator l = labels.find(key.id);
        (l != labels.end()) && (l->first == key.id); l++){
      // текст подписи = название объекта
      std::string text = (i->comment.size()>0)? i->comment[0]:"";
      if (text != l->second.text){
        l->second.text = text;
        l_m_count++;
      } else l_o_count++;

      if (text !="") NEW.push_back(l->second);
    }
    
  }
  F.insert(F.end(), NEW.begin(), NEW.end());

  std::cerr << l_n_count << " new, "
            << l_m_count << " modified, "
            << l_o_count << " non-modified\n";

  ofstream out(file.c_str());
  fig::write(out, F);
  return 0;
}

/*****************************************************/
/// Обновить ключи (fig|mp)
// - для объектов со своим ключом - если надо, меняется тип объекта в зависимости от внешнего вида (только в fig)
// - для объектов с чужим ключом - создается новый ключ, старая карта и номер сдвигается в source, старое время сохраняется
// - для объектов без ключа - создается новый ключ, source устанавливается из параметра source
int keys(int argc, char** argv){

  if (argc != 4){
    cerr << "Update keys.\n"
         << "  usage: mapsoft_vmap keys <conf> <map name> <source> <fig|mp>\n";
    return 1;
  }

  string cfile    = argv[0];
  string map_name = argv[1];
  string source   = argv[2];
  string file     = argv[3];

  std::cerr << "updating keys in " << file <<": ";  

  int obj_n_cnt=0;
  int obj_i_cnt=0;
  int obj_o_cnt=0;

  zn::zn_conv zconverter(cfile);

  if (testext(file, ".fig")){
    fig::fig_world F;
    if (!fig::read(file.c_str(), F)) {
      cerr << "bad fig file\n"; return 1;
    }

    // первый проход: найдем максимальный id
    int maxid=0;
    for (fig::fig_world::iterator i=F.begin(); i!=F.end(); i++){
      if (!zconverter.is_map_depth(*i)) continue;
      zn::zn_key key = zn::get_key(*i);
      if ((key.map != map_name) || (key.id == 0)) continue;
      if (key.id > maxid) maxid=key.id;
    }
 
    // второй проход
    for (fig::fig_world::iterator i=F.begin(); i!=F.end(); i++){
      if (!zconverter.is_map_depth(*i)) continue;
      zn::zn_key key = zn::get_key(*i);
      key.type   = zconverter.get_type(*i); // тип объекта - по внешнему виду

      if ((key.map == map_name) && (key.id !=0)){ // свой ключ
        obj_o_cnt++;
        zn::add_key(*i, key);  // добавим ключ
        continue;
      }
      if ((key.map != map_name) && (key.id !=0)){ // чужой ключ
        maxid++; obj_i_cnt++;
        key.sid    = key.id; // (id,map) -> (sid,source)
        key.source = key.map;
        key.id     = maxid;
        key.map    = map_name;
        zn::add_key(*i, key);  // добавим ключ
        continue;
      }
      // новый ключ
      maxid++; obj_n_cnt++;
      key.time.set_current();
      key.id     = maxid;
      key.map    = map_name;
      key.source = source;
      key.sid    = 0;
      zn::add_key(*i, key);  // добавим ключ
    }

    ofstream out(file.c_str());
    fig::write(out, F);
  }

  else if (testext(file, ".mp")){
    mp::mp_world M;
    if (!mp::read(file.c_str(), M)) {
      cerr << "Bad mp file\n"; return 1;
    }

    // первый проход: найдем максимальный id
    int maxid=0;
    for (mp::mp_world::iterator i=M.begin(); i!=M.end(); i++){
      zn::zn_key key = zn::get_key(*i);
      if ((key.map != map_name) || (key.id == 0)) continue;
      if (key.id > maxid) maxid=key.id;
    }
 
    // второй проход
    for (mp::mp_world::iterator i=M.begin(); i!=M.end(); i++){
      zn::zn_key key = zn::get_key(*i);
      key.type   = zconverter.get_type(*i); // тип объекта - по типу mp

      if ((key.map == map_name) && (key.id !=0)){ // свой ключ
        obj_o_cnt++;
        zn::add_key(*i, key);  // добавим ключ
        continue;
      }
      if ((key.map != map_name) && (key.id !=0)){ // чужой ключ
        maxid++; obj_i_cnt++;
        key.sid    = key.id; // (id,map) -> (sid,source)
        key.source = key.map;
        key.id     = maxid;
        key.map    = map_name;
        zn::add_key(*i, key);  // добавим ключ
        continue;
      }
      // новый ключ
      maxid++; obj_n_cnt++;
      key.time.set_current();
      key.id     = maxid;
      key.map    = map_name;
      key.source = source;
      key.sid    = 0;
      zn::add_key(*i, key);  // добавим ключ
    }

    ofstream out(file.c_str());
    mp::write(out, M);
  }
  else { cerr << "file is not .fig or .mp\n"; return 1; }

  std::cerr << obj_n_cnt << " new, " 
            << obj_i_cnt << " imports, " 
            << obj_o_cnt << " old\n";
  return 0;
}

/*****************************************************/
/// Обновить картинки (fig)
//  - раскрываются все старые составные объекты
//  - удаляются все старые картинки (объекты с комментариями [skip])
//  - новые картинки создаются
//  картинки добавляются ко всем объектам (вне зависимости от ключа)
//  объект и его картинка оборачиваются в compound, комментарий объекта
//  переносится в комментарий compound'a

int pics(int argc, char** argv){

  if (argc != 2){
    cerr << "Remove compounds and old pics, add new pics (fig).\n"
         << "  usage: mapsoft_vmap pics <conf> <fig>\n";
    return 1;
  }

  string cfile    = argv[0];
  string file     = argv[1];

  // первый проход: удаляем составные объекты и объекты с комментарием [skip] во второй строке
  // или создаем заново
  std::cerr << "removing compounds and pics in" << file <<": ";  
  if (!testext(file, ".fig")){ std::cerr << "file is not a .fig\n"; return 1;}

  fig::fig_world F;
  if (!fig::read(file.c_str(), F)) {
    cerr << "bad fig file\n"; return 1;
  }


  int obj_r_cnt=0;
  fig::fig_world::iterator i=F.begin();
  while (i!=F.end()){
    if (i->type==6) copy_comment(i, F.end());
    if ((i->type==6) || (i->type==-6) ||
        ((i->comment.size()>1) && (i->comment[1] == "[skip]"))){
      i=F.erase(i); obj_r_cnt++;
      continue;
    }
    i++;
  }
  cerr << obj_r_cnt << " objects removed\n";

  // второй проход: для каждого объекта создаем картинку, если это надо
  std::cerr << "creating new pictures in " << file <<": ";  

  list<fig::fig_object> NEW;
  int l_o_count=0;

  zn::zn_conv zconverter(cfile);

  i=F.begin();
  while (i!=F.end()){
    if (zconverter.is_map_depth(*i)){
      list<fig::fig_object> l1 = zconverter.make_pic(*i, zconverter.get_type(*i));
      if (l1.size()!=1) l_o_count++;
      NEW.insert(NEW.end(), l1.begin(), l1.end());
      i=F.erase(i);
      continue;
    }
    i++;
  }
  F.insert(F.end(), NEW.begin(), NEW.end());
  std::cerr << l_o_count << " pics added\n";

  ofstream out(file.c_str());
  fig::write(out, F);
  return 0;
}

/*****************************************************/
/// Удалить сетку и т.п.оформление (fig). Остаются: привязка, подписи, объекты
int remove_grids(int argc, char** argv){

  if (argc < 2){
    cerr << "Remove non-map objects from fig. Keep reference, labels, map-objects\n"
         << "  usage: mapsoft_vmap remove_grids <conf> <fig>\n";
    return 1;
  }

  string cfile  = argv[0];
  string file   = argv[1];

  std::cerr << "removing non-map objects from " << file <<": ";  

  zn::zn_conv zconverter(cfile);

  int obj_cnt=0;
  
  if (!testext(file, ".fig")){ std::cerr << "file is not a .fig\n"; return 1;}

  fig::fig_world F;
  if (!fig::read(file.c_str(), F)) {
    cerr << "bad fig file\n"; return 1;
  }

  fig::fig_world::iterator i=F.begin(); 
  while (i!=F.end()){
    if (!zconverter.is_map_depth(*i) &&
        (zn::get_label_key(*i).map=="") &&
        ((i->comment.size()==0) || (0!=strncmp(i->comment[0].c_str(), "REF",3))))
           {i=F.erase(i); obj_cnt++;}
    else i++;
  }

  ofstream out(file.c_str());
  fig::write(out, F);

  std::cerr << obj_cnt << " fig objects removed\n";

  return 0;
}

/*****************************************************/
/// Копировать все подписи (объекты, имеющие ключ подписи) из (fig) в (fig).
int copy_labels(int argc, char** argv){

  if (argc < 3){
    cerr << "Copy labels from fig to fig.\n"
         << "  usage: mapsoft_vmap copy_labels <conf> <infig> <outfig>\n";
    return 1;
  }

  string cfile  = argv[0];
  string ifile   = argv[1];
  string ofile   = argv[2];

  std::cerr << "copying labels from " << ifile <<" to " << ofile << ": ";  

  zn::zn_conv zconverter(cfile);

  int obj_cnt=0;
  
  if (!testext(ifile, ".fig")){ std::cerr << ifile << " is not a .fig\n"; return 1;}
  if (!testext(ofile, ".fig")){ std::cerr << ofile << " is not a .fig\n"; return 1;}

  fig::fig_world IF;
  if (!fig::read(ifile.c_str(), IF)) {
    cerr << "bad fig file " << ifile << "\n"; return 1;
  }
  g_map iref = fig::get_ref(IF);
  if (iref.size()<3){
    cerr << "not a GEO-fig: "<< ifile << "\n"; return 1;
  }

  fig::fig_world OF;
  if (!fig::read(ofile.c_str(), OF)) {
    cerr << "bad fig file " << ofile << "\n"; return 1;
  }
  g_map oref = fig::get_ref(OF);
  if (oref.size()<3){
    cerr << "not a GEO-fig: "<< ofile << "\n"; return 1;
  }
  convs::map2map cnv(iref, oref);

  for (fig::fig_world::iterator i=IF.begin(); i!=IF.end(); i++){
    if (zn::get_label_key(*i).map=="")  continue;
    obj_cnt++;
    fig::fig_object j=*i; j.clear();
    j.set_points(cnv.line_frw(*i));
    OF.push_back(j);
  }

  ofstream out(ofile.c_str());
  fig::write(out, OF);

  std::cerr << obj_cnt << " labels copied\n";

  return 0;
}

/*****************************************************/
/// Показать карты, объекты из которых присутствуют в (fig|mp)
int show_maps(int argc, char** argv){

  if (argc != 2){
    cerr << "Show maps.\n"
         << "  usage: mapsoft_vmap maps <conf> <fig|mp>\n";
    return 1;
  }

  map<string, int> cnt;

  string cfile    = argv[0];
  string file     = argv[1];

  zn::zn_conv zconverter(cfile);

  if (testext(file, ".fig")){
    fig::fig_world F;
    if (!fig::read(file.c_str(), F)) {
      cerr << "bad fig file\n"; return 1;
    }

    int maxid=0;
    for (fig::fig_world::iterator i=F.begin(); i!=F.end(); i++){
      if (!zconverter.is_map_depth(*i)) continue;
      zn::zn_key key = zn::get_key(*i);
      if (key.map == "") continue;
      if (cnt.count(key.map)==0) cnt[key.map]=0;
      else cnt[key.map]++;
    }
  }
  else if (testext(file, ".mp")){
    mp::mp_world M;
    if (!mp::read(file.c_str(), M)) {
      cerr << "Bad mp file\n"; return 1;
    }

    int maxid=0;
    for (mp::mp_world::iterator i=M.begin(); i!=M.end(); i++){
      zn::zn_key key = zn::get_key(*i);
      if (key.map == "") continue;
      if (cnt.count(key.map)==0) cnt[key.map]=1;
      else cnt[key.map]++;
    }
  }
  else { cerr << "file is not .fig or .mp\n"; return 1; }

  for (map<string,int>::const_iterator i=cnt.begin(); i!=cnt.end(); i++){
    cout << i->first << "\t" << i->second << "\n";
  }
  return 0;
}


/*****************************************************/
int main(int argc, char** argv){
  if (argc < 2){
    cerr << "usage: mapsoft_vmap <command> <args>\n"
         << "commands: \n"
         << "  - copy   -- copy map objects from (fig|mp) to (fig|mp)\n"
         << "  - remove -- remove map objects from (fig|mp) map\n"
         << "  - labels -- update labels in fig map\n"
         << "  - pics   -- update pics in fig map\n"
         << "  - keys   -- update keys in (fig|mp) map\n"
         << "  - remove_grids  -- remove all but reference lables and map objects in fig\n"
         << "  - copy_labels   -- copy all labels from (fig) to (fig)\n"
         << "  - show_maps     -- show map_names in object keys in (fig|mp)\n"
;
    exit(0);
  }
  if (strcmp(argv[1], "copy")==0)           exit(copy(argc-2, argv+2));
  if (strcmp(argv[1], "remove")==0)         exit(remove(argc-2, argv+2));
  if (strcmp(argv[1], "labels")==0)         exit(labels(argc-2, argv+2));
  if (strcmp(argv[1], "keys")==0)           exit(keys(argc-2, argv+2));
  if (strcmp(argv[1], "pics")==0)           exit(pics(argc-2, argv+2));
  if (strcmp(argv[1], "remove_grids")==0)   exit(remove_grids(argc-2, argv+2));
  if (strcmp(argv[1], "copy_labels")==0)    exit(copy_labels(argc-2, argv+2));
  if (strcmp(argv[1], "show_maps")==0)      exit(show_maps(argc-2, argv+2));

  cerr << "unknown command: " << argv[1] << "\n";
  exit(1);
}
