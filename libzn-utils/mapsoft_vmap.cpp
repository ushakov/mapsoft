#include <string>
#include <fstream>
#include "../libgeo_io/geofig.h"
#include "../libmp/mp.h"

#include "../libzn/zn.h"
#include "../libzn/zn_key.h"

#include "../lib2d/line_rectcrop.h"
#include "../libgeo_io/io.h"

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
string filter_help(void){
  return  string("filters:\n") +
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
    cerr << "ERR: unknown filter!\n";
    return 1;
  }
  if (filter!="all"){
    if (argc<5){
      cerr << "ERR: argument required!\n";
      return 1;
    }
    arg=argv[4];
  }

  cerr << "copying from " << ifile << " to " << ofile << ": ";

  fig::fig_world IF, OF;
  mp::mp_world   IM, OM;

  zn::zn_conv zconverter(cfile);

  // читаем входной файл, преобразуем все в IM
  if (testext(ifile, ".fig")){
    if (!fig::read(ifile.c_str(), IF)) {
      cerr << "ERR: bad fig file " << ifile << "\n"; return 1;
    }
    g_map ref = fig::get_ref(IF);
    if (ref.size()<3){
      cerr << "ERR: not a GEO-fig\n"; return 1;
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
      cerr << "ERR: bad mp file " << ifile << "\n"; return 1;
    }
  }
  else { cerr << "ERR: input file is not .fig or .mp\n"; return 1;}

  int obj_cnt=0;

  // читаем выходной файл, дописываем туда новые объекты (фильтруя их), записываем
  if (testext(ofile, ".fig")){
    if (!fig::read(ofile.c_str(), OF)) {
      cerr << "ERR: bad fig file " << ofile << "\n"; return 1;
    }
    g_map ref = fig::get_ref(OF);
    if (ref.size()<3){
      cerr << "ERR: not a GEO-fig\n"; return 1;
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
  else { cerr << "ERR: output file is not .fig or .mp\n"; return 1; }

  cerr << obj_cnt << " map objects copied\n";

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
    cerr << "ERR: unknown filter!\n";
    return 1;
  }
  if (filter!="all"){
    if (argc<4){
      cerr << "ERR: argument required!\n";
      return 1;
    }
    arg=argv[3];
  }

  cerr << "removing map objects from " << file <<": ";

  zn::zn_conv zconverter(cfile);

  int obj_cnt=0;

  if (testext(file, ".fig")){
    fig::fig_world F;
    if (!fig::read(file.c_str(), F)) {
      cerr << "ERR: bad fig file\n"; return 1;
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
      cerr << "ERR: bad mp file\n"; return 1;
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
  else { cerr << "ERR: file is not .fig or .mp\n"; return 1; }

  cerr << obj_cnt << " map objects removed\n";

  return 0;
}

/*****************************************************/
/// Удалить ключи картографических объектов (fig|mp)
int remove_keys(int argc, char** argv){

  if (argc < 3){
    cerr << "Remove keys of map objects from mp or fig.\n"
         << "  usage: mapsoft_vmap remove <conf> <fig|mp> <filter> <filter arg>\n"
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
    cerr << "ERR: unknown filter!\n";
    return 1;
  }
  if (filter!="all"){
    if (argc<4){
      cerr << "ERR: argument required!\n";
      return 1;
    }
    arg=argv[3];
  }

  cerr << "removing keys from " << file <<": ";

  zn::zn_conv zconverter(cfile);

  int obj_cnt=0;

  if (testext(file, ".fig")){
    fig::fig_world F;
    if (!fig::read(file.c_str(), F)) {
      cerr << "ERR: bad fig file\n"; return 1;
    }

    for (fig::fig_world::iterator i=F.begin(); i!=F.end(); i++){
      if (i->type==6) copy_comment(i, F.end());
      if (zconverter.is_map_depth(*i) &&
          testfilter(filter, arg, zn::get_key(*i)))
             {zn::clear_key(*i); obj_cnt++;}
    }

    ofstream out(file.c_str());
    fig::write(out, F);
  }

  else if (testext(file, ".mp")){
    mp::mp_world M;
    if (!mp::read(file.c_str(), M)) {
      cerr << "ERR: bad mp file\n"; return 1;
    }

    for (mp::mp_world::iterator i=M.begin(); i!=M.end(); i++){
      if (testfilter(filter, arg, zn::get_key(*i)))
         {zn::clear_key(*i); obj_cnt++;}
    }

    ofstream out(file.c_str());
    mp::write(out, M);
  }
  else { cerr << "ERR: file is not .fig or .mp\n"; return 1; }

  cerr << obj_cnt << " keys removed\n";

  return 0;
}


/*****************************************************/
///  Обновление подписей (fig)
// - если к объекту есть подписи - оставим их
// - если нет - создадим
// - если у объекта поменялось название - подпись меняется
// - если исчез объект, к которому привязана подпись - подпись удаляется
/*****************************************************/
/// Обновление ключей (fig|mp)
// - для объектов с ключом - если надо, меняется тип объекта в зависимости от внешнего вида
//   (м.б вообще не хранить тип в ключе?)
// - для объектов без ключа - создается новый ключ, source устанавливается из параметра source
// - если найдется несколько объектов с одинаковым ключом в одной карте - для всех создаются новые ключи
/*****************************************************/
/// Обновление картинок (fig)
//  - раскрываются все составные объекты
//  - удаляются все старые картинки (объекты с комментариями [skip])
//  - fig-файл сортируется
//  - новые картинки создаются
//  картинки добавляются ко всем объектам (вне зависимости от ключа)
//  объект и его картинка оборачиваются в compound, комментарий объекта
//  переносится в комментарий compound'a

/*****************************************************/
/// update fig map
/// (new function to replace keys+labels+pics)
int update_fig(int argc, char** argv){

  if (argc != 4){
    cerr << "Update fig.\n"
         << "  usage: mapsoft_vmap update_fig <conf> <map name> <editor> <fig>\n";
    return 1;
  }
  string cfile    = argv[0];
  string map_name = argv[1];
  string editor   = argv[2];
  string file     = argv[3];

  // read file
  cerr << "updating FIG file " << file <<"\n";
  fig::fig_world F;
  if (!fig::read(file.c_str(), F)) {
    cerr << "ERR: bad fig file\n"; return 1;
  }

  map<string, multimap<zn::id_type, fig::fig_object> > labels;
  map<string, multiset<zn::id_type> > dup_counter;

  zn::zn_conv zconverter(cfile);

  // First pass
  fig::fig_world::iterator i=F.begin();
  while (i!=F.end()){
    // * remove compounds, copy comments from them
    // * remove objects with "[skip]" comment
    if (i->type==6) copy_comment(i, F.end());
    if ((i->type==6) || (i->type==-6) ||
        ((i->comment.size()>1) && (i->comment[1] == "[skip]"))){
      i=F.erase(i);
      continue;
    }
    // * move labels to labels multimap
    zn::zn_label_key k = zn::get_label_key(*i);
    if (k.id!=0){// подписи
      labels[k.map].insert(pair<zn::id_type, fig::fig_object>(k.id,*i));
      i=F.erase(i);
      continue;
    }
    // * count objects
    if (zconverter.is_map_depth(*i)){
      zn::zn_key k = zn::get_key(*i);
      if (k.id!=0) dup_counter[k.map].insert(k.id);
    }
    i++;
  }

  int obj_n_cnt=0;
  int obj_o_cnt=0;
  // Second pass. Clear duplicated keys, create new keys, update keys
  for (i=F.begin(); i!=F.end(); i++){
    if (!zconverter.is_map_depth(*i)) continue;
    zn::zn_key key = zn::get_key(*i);
    if (dup_counter[key.map].count(key.id)>1){
      std::cerr << "Warning: clear duplicated keys " << key.id << "@" << key.map << "\n";
      key.id=0;
    }
    if (key.id == 0){ // new key
      key=zn::make_key(map_name, editor);
      obj_n_cnt++;
    }
    else {  // old key
      obj_o_cnt++;
    }

    key.type = zconverter.get_type(*i); // тип объекта - по внешнему виду
    if (key.type==0){
      std::cerr << "Warning: unknown fig object. Put to depth=10\n";
      i->depth=10;
    }
    zn::add_key(*i, key);  // add key
  }

  // Third pass. Update labels
  int l_n_count=0;
  int l_o_count=0;
  int l_m_count=0;
  list<fig::fig_object> NEW;
  for (i=F.begin(); i!=F.end(); i++){
    if (!zconverter.is_map_depth(*i)) continue;
    zn::zn_key key = zn::get_key(*i);

    // FIXME: remove this code if all is OK!
    if (key.sid!=0){
      key.id=key.sid; key.map=key.source;
      std::cerr << "Warning: key.sid!=0. This is impossible!\n";
    }

    // unknown objects
    if (key.id ==0){
      std::cerr << "Warning: key.id!=0!\n";
      continue;
    }

    // если у объекта есть название, но нет подписи - сделаем ее
    if ((i->comment.size()>0) &&
        (i->comment[0] != "") &&
        (labels[key.map].count(key.id) == 0)){
      list<fig::fig_object> l1 = zconverter.make_labels(*i, key.type); // изготовим новые подписи
      add_key(l1, zn::zn_label_key(key));
      NEW.insert(NEW.end(), l1.begin(), l1.end());
      l_n_count+=l1.size();
      continue;
    }
    // вытащим из хэша все старые подписи для этого объекта
    for (multimap<zn::id_type, fig::fig_object>::iterator l = labels[key.map].find(key.id);
        (l != labels[key.map].end()) && (l->first == key.id); l++){
      // текст подписи = название объекта
      string text = (i->comment.size()>0)? i->comment[0]:"";
      if (text != l->second.text){
        l->second.text = text;
        l_m_count++;
      } else l_o_count++;
      zconverter.label_update(l->second, key.type);
      if (text !="") NEW.push_back(l->second);
    }
  }
  F.insert(F.end(), NEW.begin(), NEW.end());

  // sort fig before creating compounds
  F.sort();

  // Fourth pass. Create pics
  NEW.clear();
  int p_count=0;

  i=F.begin();
  while (i!=F.end()){
    if (zconverter.is_map_depth(*i)){
      list<fig::fig_object> l1 = zconverter.make_pic(*i, zconverter.get_type(*i));
      if (l1.size()>1){ p_count++;
        i=F.erase(i);
        F.insert(i, l1.begin(), l1.end());
        continue;
      }
    }
    i++;
  }

  cerr << " * Map objects: "
       << obj_n_cnt << " new, "
       << obj_o_cnt << " old\n";
  cerr << " * Labels:      "
       << l_n_count << " new, "
       << l_m_count << " modified, "
       << l_o_count << " non-modified\n";
  cerr << " * Pics:        "
       << l_o_count << " pics added\n";

  ofstream out(file.c_str());
  fig::write(out, F);
  return 0;
}

/*****************************************************/
/// update mp map
/// (new function to replace keys+labels+pics)
int update_mp(int argc, char** argv){

  if (argc != 4){
    cerr << "Update mp.\n"
         << "  usage: mapsoft_vmap update_mp <conf> <map name> <editor> <mp>\n";
    return 1;
  }
  string cfile    = argv[0];
  string map_name = argv[1];
  string editor   = argv[2];
  string file     = argv[3];

  zn::zn_conv zconverter(cfile);

  // read file
  cerr << "updating MP file " << file <<"\n";
  mp::mp_world M;
  if (!mp::read(file.c_str(), M)) {
    cerr << "ERR: bad mp file\n"; return 1;
  }

  map<string, multiset<zn::id_type> > dup_counter;
  // First pass. Count objects
  mp::mp_world::iterator i=M.begin();
  while (i!=M.end()){
    zn::zn_key k = zn::get_key(*i);
    if (k.id!=0) dup_counter[k.map].insert(k.id);
    i++;
  }

  int obj_n_cnt=0;
  int obj_o_cnt=0;
  // Second pass. Clear duplicated keys, create new keys, update keys
  for (i=M.begin(); i!=M.end(); i++){
    zn::zn_key key = zn::get_key(*i);
    if (dup_counter[key.map].count(key.id)>1){
      std::cerr << "Warning: clear duplicated keys " << key.id << "@" << key.map << "\n";
      key.id=0;
    }
    if (key.id == 0){ // new key
      key=zn::make_key(map_name, editor);
      obj_n_cnt++;
    }
    else {  // old key
      obj_o_cnt++;
    }
    key.type = zconverter.get_type(*i); // тип объекта - по внешнему виду
    if (key.type==0) std::cerr << "Warning: unknown mp object\n";
    zn::add_key(*i, key);  // add key
  }

  // sort mp
  M.sort();

  cerr << " * Map objects: "
       << obj_n_cnt << " new, "
       << obj_o_cnt << " old\n";

  ofstream out(file.c_str());
  mp::write(out, M);
  return 0;
}




/*****************************************************/
/// Обрезать картографические объекты (fig|mp)
int crop(int argc, char** argv){

  if ((argc != 5) && (argc!=3)){
    cerr << "Update keys.\n"
         << "  usage: mapsoft_vmap crop <conf> <proj> <datum> <geom> <fig|mp>\n"
         << "         mapsoft_vmap crop <conf> <nom> <fig|mp>\n";
    return 1;
  }

  string cfile    = argv[0];
  string proj;
  string datum;
  string file;
  Rect<double> cutter;
  if (argc==5){
    proj     = argv[1];
    datum    = argv[2];
    cutter=boost::lexical_cast<Rect<double> >(argv[3]);
    file     = argv[4];
  } else {
    proj     = "lonlat";
    datum    = "pulkovo";
    cutter=filters::nom_range(argv[1]);
    file     = argv[2];
  }

  cerr << "cropping " << file <<": ";

  int obj_n_cnt=0;
  int obj_c_cnt=0;
  int obj_d_cnt=0;

  zn::zn_conv zconverter(cfile);

  if (testext(file, ".fig")){
    fig::fig_world F;
    if (!fig::read(file.c_str(), F)) {
      cerr << "ERR: bad fig file\n"; return 1;
    }
    g_map ref = fig::get_ref(F);
    if (ref.size()<3){
      cerr << "ERR: not a GEO-fig\n"; return 1;
    }
    convs::map2pt cnv(ref, Datum(datum), Proj(proj));

    fig::fig_world::iterator i=F.begin();
    while (i!=F.end()){
      if (!zconverter.is_map_depth(*i)) { i++; continue;}
      Line<double> l = cnv.line_frw(*i);
      bool closed= i->is_closed() || (i->area_fill != -1);
      if (rect_crop(cutter, l, closed)) obj_c_cnt++; else obj_n_cnt++;
      i->set_points(cnv.line_bck(l));
      if (i->size()==0) {i=F.erase(i); obj_d_cnt++; obj_c_cnt--;} else i++;
    }
    ofstream out(file.c_str());
    fig::write(out, F);
  }

  else if (testext(file, ".mp")){
    mp::mp_world M;
    if (!mp::read(file.c_str(), M)) {
      cerr << "ERR: bad mp file\n"; return 1;
    }
    convs::pt2pt cnv(Datum("wgs84"), Proj("lonlat"), Options(),
                     Datum(datum), Proj(proj), Options());

    mp::mp_world::iterator i=M.begin();
    while (i!=M.end()){
      Line<double> l = cnv.line_frw(*i, 1e-7);
      bool closed= (i->Class == "POLYGON");
      if (rect_crop(cutter, l, closed)) obj_c_cnt++; else obj_n_cnt++;
      i->set_points(cnv.line_bck(l, 1e-7));
      if (i->size()==0) {i=M.erase(i); obj_d_cnt++; obj_c_cnt--;} else i++;
    }
    ofstream out(file.c_str());
    mp::write(out, M);
  }
  else { cerr << "ERR: file is not .fig or .mp\n"; return 1; }

  cerr << obj_n_cnt << " non-modified, "
       << obj_c_cnt << " cropped, "
       << obj_d_cnt << " deleted\n";
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

  cerr << "removing non-map objects from " << file <<": ";

  zn::zn_conv zconverter(cfile);

  int obj_cnt=0;

  if (!testext(file, ".fig")){ cerr << "ERR: file is not a .fig\n"; return 1;}

  fig::fig_world F;
  if (!fig::read(file.c_str(), F)) {
    cerr << "ERR: bad fig file\n"; return 1;
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

  cerr << obj_cnt << " fig objects removed\n";

  return 0;
}

/*****************************************************/
/// Удалить подписи из (fig).
int remove_labels(int argc, char** argv){

  if (argc < 2){
    cerr << "Remove labels from fig.\n"
         << "  usage: mapsoft_vmap remove_labels <conf> <fig>\n";
    return 1;
  }

  string cfile  = argv[0];
  string file   = argv[1];

  cerr << "removing labels objects from " << file <<": ";

  zn::zn_conv zconverter(cfile);

  int obj_cnt=0;

  if (!testext(file, ".fig")){ cerr << "ERR: file is not a .fig\n"; return 1;}

  fig::fig_world F;
  if (!fig::read(file.c_str(), F)) {
    cerr << "ERR: bad fig file\n"; return 1;
  }

  fig::fig_world::iterator i=F.begin();
  while (i!=F.end()){
    if (zn::get_label_key(*i).map!="")
      {i=F.erase(i); obj_cnt++;}
    else i++;
  }

  ofstream out(file.c_str());
  fig::write(out, F);

  cerr << obj_cnt << " labels removed\n";

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

  cerr << "copying labels from " << ifile <<" to " << ofile << ": ";

  zn::zn_conv zconverter(cfile);

  int obj_cnt=0;

  if (!testext(ifile, ".fig")){ cerr << "ERR: " << ifile << " is not a .fig\n"; return 1;}
  if (!testext(ofile, ".fig")){ cerr << "ERR: " << ofile << " is not a .fig\n"; return 1;}

  fig::fig_world IF;
  if (!fig::read(ifile.c_str(), IF)) {
    cerr << "ERR: bad fig file " << ifile << "\n"; return 1;
  }
  g_map iref = fig::get_ref(IF);
  if (iref.size()<3){
    cerr << "ERR: not a GEO-fig: "<< ifile << "\n"; return 1;
  }

  fig::fig_world OF;
  if (!fig::read(ofile.c_str(), OF)) {
    cerr << "ERR: bad fig file " << ofile << "\n"; return 1;
  }
  g_map oref = fig::get_ref(OF);
  if (oref.size()<3){
    cerr << "ERR: not a GEO-fig: "<< ofile << "\n"; return 1;
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

  cerr << obj_cnt << " labels copied\n";

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
      cerr << "ERR: bad fig file\n"; return 1;
    }

    int maxid=0;
    for (fig::fig_world::iterator i=F.begin(); i!=F.end(); i++){
      if (!zconverter.is_map_depth(*i)) continue;
      zn::zn_key key = zn::get_key(*i);
      if (key.map == "") continue;
      if (cnt.count(key.map)==0) cnt[key.map]=1;
      else cnt[key.map]++;
    }
  }
  else if (testext(file, ".mp")){
    mp::mp_world M;
    if (!mp::read(file.c_str(), M)) {
      cerr << "ERR: bad mp file\n"; return 1;
    }

    int maxid=0;
    for (mp::mp_world::iterator i=M.begin(); i!=M.end(); i++){
      zn::zn_key key = zn::get_key(*i);
      if (key.map == "") continue;
      if (cnt.count(key.map)==0) cnt[key.map]=1;
      else cnt[key.map]++;
    }
  }
  else { cerr << "ERR: file is not .fig or .mp\n"; return 1; }

  for (map<string,int>::const_iterator i=cnt.begin(); i!=cnt.end(); i++){
    cout << i->first << "\t" << i->second << "\n";
  }
  return 0;
}

/*****************************************************/
/// Create new keys for objects and their labels
/// which are came from other map.
int switch_map(int argc, char** argv){
  if (argc != 3){
    cerr << "Make all objects and their labels to be in one map (fig).\n"
         << "  usage: mapsoft_vmap one_map <conf> <map name> <fig>\n";
    return 1;
  }
  string cfile    = argv[0];
  string map_name = argv[1];
  string file     = argv[2];

  cerr << "renaming object maps to " << map_name <<": ";

  zn::zn_conv zconverter(cfile);

  fig::fig_world F;
  if (!fig::read(file.c_str(), F)) {
    cerr << "ERR: bad fig file\n"; return 1;
  }

  map<string, map<zn::id_type, zn::id_type> > cnv;
  int o_cnt=0;
  int l_cnt=0;

  // First pass. Rename map in object keys
  fig::fig_world::iterator i=F.begin();
  for (i=F.begin(); i!=F.end(); i++){
    if (!zconverter.is_map_depth(*i)) continue;
    zn::zn_key k = zn::get_key(*i);
    if (k.id==0) continue;
    if (k.map==map_name) continue;
    // id is not unique, so create new id
    zn::id_type newid=zn::make_id();
    cnv[k.map].insert(pair<zn::id_type, zn::id_type>(k.id, newid));
    // change key
    k.map=map_name;
    k.id=newid;
    zn::add_key(*i, k);
    o_cnt++;
  }
  // Second pass. Rename map in label keys
  for (i=F.begin(); i!=F.end(); i++){
    zn::zn_label_key k = zn::get_label_key(*i);
    if (k.id==0) continue;
    if (k.map==map_name) continue;
    map<zn::id_type, zn::id_type>::const_iterator p=cnv[k.map].find(k.id);
    if (p==cnv[k.map].end()){
      cerr << "Warning: label without object: " << k << "\n";
      continue;
    }
    k.map=map_name;
    k.id=p->second;
    zn::add_key(*i, k);
    l_cnt++;
  }

  ofstream out(file.c_str());
  fig::write(out, F);
  cerr << o_cnt << " objects, " << l_cnt << " labels\n";
}

/*****************************************************/
int main(int argc, char** argv){
  if (argc < 2){
    cerr << "usage: mapsoft_vmap <command> <args>\n"
         << "commands: \n"
         << "  - crop          -- crop map objects in (fig|mp)\n"
         << "  - copy          -- copy map objects from (fig|mp) to (fig|mp)\n"
         << "  - copy_labels   -- copy all labels from (fig) to (fig)\n"
         << "  - remove        -- remove map objects from (fig|mp) map\n"
         << "  - remove_grids  -- remove all but reference, lables, and map objects from fig\n"
         << "  - remove_labels -- remove lables from fig\n"
         << "  - remove_keys   -- remove keys from objects\n"
         << "  - update_fig    -- update fig (new function to replace keys+labels+pics)\n"
         << "  - update_mp     -- update mp  (new function to replace keys+labels+pics)\n"
         << "  - show_maps     -- show map names in (fig|mp)\n"
         << "  - switch_map    -- make all objects and their labels to be in one map\n"
;
    exit(0);
  }
  if (strcmp(argv[1], "crop")==0)           exit(crop(argc-2, argv+2));
  if (strcmp(argv[1], "copy")==0)           exit(copy(argc-2, argv+2));
  if (strcmp(argv[1], "copy_labels")==0)    exit(copy_labels(argc-2, argv+2));
  if (strcmp(argv[1], "remove")==0)         exit(remove(argc-2, argv+2));
  if (strcmp(argv[1], "remove_grids")==0)   exit(remove_grids(argc-2, argv+2));
  if (strcmp(argv[1], "remove_labels")==0)  exit(remove_labels(argc-2, argv+2));
  if (strcmp(argv[1], "remove_keys")==0)    exit(remove_keys(argc-2, argv+2));
  if (strcmp(argv[1], "update_fig")==0)     exit(update_fig(argc-2, argv+2));
  if (strcmp(argv[1], "update_mp")==0)      exit(update_mp(argc-2, argv+2));
  if (strcmp(argv[1], "show_maps")==0)      exit(show_maps(argc-2, argv+2));
  if (strcmp(argv[1], "switch_map")==0)     exit(switch_map(argc-2, argv+2));

  cerr << "unknown command: " << argv[1] << "\n";
  exit(1);
}
