#include <string>
#include <cstring>
#include "geo_io/geofig.h"
#include "mp/mp.h"

#include "vmap/zn.h"
#include "2d/line_utils.h"

#include "2d/line_rectcrop.h"
#include "geo_io/io.h"
#include "geo/geo_nom.h"

using namespace std;

bool testext(const string & nstr, const char *ext){
    int pos = nstr.rfind(ext);
    return ((pos>0)&&(pos == nstr.length()-strlen(ext)));
}

/*****************************************************/
/// Копировать картографические объекты из (mp|fig) в (mp|fig)
// в соответствии с системой знаков
// fig (как входной так и выходной) должен обязательно содержать привязку
// Если выходного файла mp нет - он создается

int copy(int argc, char** argv){

  if (argc < 2){
    cerr << "Copy map objects.\n"
         << "  usage: mapsoft_vmap copy <in mp|fig> <out mp|fig> [<source>]\n"
         << "FIG files must have geo-reference.\n";
    return 1;
  }

  string ifile = argv[0];
  string ofile = argv[1];
  string source;
  if (argc>2) source = argv[2];

  cerr << "copying from " << ifile << " to " << ofile << ": ";

  fig::fig_world IF, OF;
  mp::mp_world   IM, OM;


  // читаем входной файл, преобразуем все в IM
  if (testext(ifile, ".fig")){
    if (!fig::read(ifile.c_str(), IF)) {
      cerr << "ERR: bad fig file " << ifile << "\n"; return 1;
    }

    string style=IF.opts.get<string>("style", "default");
    zn::zn_conv zconverter(style);

    g_map ref = fig::get_ref(IF);
    if (ref.size()<3){
      cerr << "ERR: not a GEO-fig\n"; return 1;
    }
    convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"));
    for (fig::fig_world::iterator i=IF.begin(); i!=IF.end(); i++){
      if (i->type==6) zn::fig_copy_comment(i, IF.end());
      if (zn::is_to_skip(*i) || !zconverter.is_map_depth(*i)) continue;
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

    string style=OF.opts.get<string>("style", "default");
    zn::zn_conv zconverter(style);

    g_map ref = fig::get_ref(OF);
    if (ref.size()<3){
      cerr << "ERR: not a GEO-fig\n"; return 1;
    }
    convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"));
    for (mp::mp_world::const_iterator i=IM.begin(); i!=IM.end(); i++){
      if ((source == "") || (i->Opts.get("Source", string()) == source)){
        obj_cnt++;
        list<fig::fig_object> tmp=zconverter.mp2fig(*i, cnv);
        OF.insert(OF.end(), tmp.begin(), tmp.end());
      }
    }
    fig::write(ofile, OF);
  }
  else if (testext(ofile, ".mp")){
    mp::read(ofile.c_str(), OM);
    // если файла нет - OM остается пустым
    for (mp::mp_world::const_iterator i=IM.begin(); i!=IM.end(); i++){
      if ((source == "") || (i->Opts.get("Source", string()) == source)){
        obj_cnt++;
        OM.push_back(*i);
      }
    }
    mp::write(ofile, OM);
  }
  else { cerr << "ERR: output file is not .fig or .mp\n"; return 1; }

  cerr << obj_cnt << " map objects copied\n";

  return 0;
}

/*****************************************************/
/// Удалить картографические объекты (fig|mp)
int remove(int argc, char** argv){

  if (argc < 1){
    cerr << "Remove map objects from mp or fig.\n"
         << "  usage: mapsoft_vmap remove <fig|mp> [<source>]\n"
         << "FIG file must have geo-reference.\n";
    return 1;
  }

  string file   = argv[0];
  string source;
  if (argc>1) source = argv[1];

  cerr << "removing map objects from " << file <<": ";

  int obj_cnt=0;

  if (testext(file, ".fig")){
    fig::fig_world F;
    if (!fig::read(file.c_str(), F)) {
      cerr << "ERR: bad fig file\n"; return 1;
    }

    string style=F.opts.get<string>("style", "default");
    zn::zn_conv zconverter(style);

    fig::fig_world::iterator i=F.begin();
    while (i!=F.end()){
      if (!zn::is_to_skip(*i) && zconverter.is_map_depth(*i) &&
        ((source == "") || (i->opts.get("Source", string()) == source)))
             {i=F.erase(i); obj_cnt++;}
      else i++;
    }
    fig::write(file, F);
  }

  else if (testext(file, ".mp")){
    mp::mp_world M;
    if (!mp::read(file.c_str(), M)) {
      cerr << "ERR: bad mp file\n"; return 1;
    }

    mp::mp_world::iterator i=M.begin();
    while (i!=M.end()){
      if ((source == "") || (i->Opts.get("Source", string()) == source))
        {i=M.erase(i); obj_cnt++;}
      else i++;
    }
    mp::write(file, M);
  }
  else { cerr << "ERR: file is not .fig or .mp\n"; return 1; }

  cerr << obj_cnt << " map objects removed\n";

  return 0;
}

/*****************************************************/
///  Обновление подписей (fig)
// - если к объекту есть подписи - оставим их
// - если нет - создадим
// - если у объекта поменялось название - подпись меняется
// - если исчез объект, к которому привязана подпись - подпись удаляется
/*****************************************************/
/// Обновление картинок (fig)
//  - раскрываются все составные объекты
//  - удаляются все старые картинки (объекты с MapType=pic)
//  - fig-файл сортируется
//  - новые картинки создаются
//  объект и его картинка оборачиваются в compound, комментарий объекта
//  переносится в комментарий compound'a

/*****************************************************/
/// update fig map
int update(int argc, char** argv){

  if (argc != 1){
    cerr << "Update fig.\n"
         << "  usage: mapsoft_vmap update <fig>\n";
    return 1;
  }
  string file     = argv[0];

  // read file
  cerr << "updating FIG file " << file <<"\n";
  fig::fig_world F;
  if (!fig::read(file.c_str(), F)) {
    cerr << "ERR: bad fig file\n"; return 1;
  }

  string style=F.opts.get<string>("style", "default");
  zn::zn_conv zconverter(style);

  zn::fig_remove_pics(F);

  zconverter.fig_update_labels(F);
  F.sort();  // sort fig before creating compounds
  zconverter.fig_add_pics(F);

  fig::write(file, F);
  return 0;
}

/*****************************************************/
/// Обрезать картографические объекты (fig|mp)
int crop(const string & mode, int argc, char** argv){

  if ((argc != 4) && (argc!=2)){
    cerr << "crop map.\n"
         << "  usage: mapsoft_vmap " << mode << " <proj> <datum> <geom> <fig|mp>\n"
         << "         mapsoft_vmap " << mode << " <nom> <fig|mp>\n";
    return 1;
  }

  string proj;
  string datum;
  string file;
  dRect cutter;
  Options O;
  if (argc==4){
    proj     = argv[0];
    datum    = argv[1];
    cutter=boost::lexical_cast<dRect>(argv[2]);
    O.put("lon0", convs::lon_pref2lon0(cutter.x));
    cutter.x=convs::lon_delprefix(cutter.x);
    file     = argv[3];
  } else {
    proj     = "lonlat";
    datum    = "pulkovo";
    cutter=convs::nom_to_range(argv[0]);
    file     = argv[1];
  }

  cerr << mode << " " << file <<" to " << cutter << ": ";

  int obj_cnt1=0;
  int obj_cnt2=0;

  if (testext(file, ".fig")){
    fig::fig_world F;
    if (!fig::read(file.c_str(), F)) {
      cerr << "ERR: bad fig file\n"; return 1;
    }

    string style=F.opts.get<string>("style", "default");
    zn::zn_conv zconverter(style);

    g_map ref = fig::get_ref(F);
    if (ref.size()<3){
      cerr << "ERR: not a GEO-fig\n"; return 1;
    }
    convs::map2pt cnv(ref, Datum(datum), Proj(proj), O);

    fig::fig_world::iterator i=F.begin();
    while (i!=F.end()){
      if (zn::is_to_skip(*i) || !zconverter.is_map_depth(*i)) { i++; continue;}
      obj_cnt1++;
      dLine l = cnv.line_frw(*i);
      bool closed= i->is_closed() || (i->area_fill != -1);
      rect_crop(cutter, l, closed);

      if (mode=="range_crop"){
        i->set_points(cnv.line_bck(l));
        if (l.size()==0) {i=F.erase(i); obj_cnt2++;} else i++;
      }
      else
      if (mode=="range_select"){
        if (l.size()==0) {i=F.erase(i); obj_cnt2++;} else i++;
      }
      else
      if (mode=="range_remove"){
        if (l.size()!=0) {i=F.erase(i); obj_cnt2++;} else i++;
      }
      else {
        cerr << "mapsoft_vmap crop: unknown mode\n";
        break;
      }
    }

    fig::fig_object brd = fig::make_object("2 3 0 1 0 * * * * * * * * * * *");
    zconverter.fig_update(brd,0x10001D);
    brd.set_points(cnv.line_bck(rect2line(cutter)));
    brd.opts["Source"] = "border";
    F.push_back(brd);

    fig::write(file, F);
  }

  else if (testext(file, ".mp")){
    mp::mp_world M;
    if (!mp::read(file.c_str(), M)) {
      cerr << "ERR: bad mp file\n"; return 1;
    }
    convs::pt2pt cnv(Datum("wgs84"), Proj("lonlat"), Options(),
                     Datum(datum), Proj(proj), O);

    // Run cnv on point from cutter
    // to avoid automatic setting of lon0 from the first point of mp-file
    // (it can be in wrong zone!)
    dPoint tmp=cutter.TLC();
    cnv.bck(tmp);

    mp::mp_world::iterator i=M.begin();
    while (i!=M.end()){
      obj_cnt1++;
      mp::mp_object::iterator l=i->begin();
      while (l!=i->end()){
        dLine line = cnv.line_frw(*l, 1e-7);
        bool closed= (i->Class == "POLYGON");
        rect_crop(cutter, line, closed);

        if (mode=="range_crop"){
          *l=cnv.line_bck(line, 1e-7);
          if (line.size()==0) l=i->erase(l); else l++;
        }
        else
        if (mode=="range_select"){
          if (line.size()==0) l=i->erase(l); else l++;
        }
        else
        if (mode=="range_remove"){
          if (line.size()!=0) l=i->erase(l); else l++;
        }
        else {
          cerr << "mapsoft_vmap crop: unknown mode\n";
          break;
        }
      }
      if (i->size()==0) {i=M.erase(i); obj_cnt2++;} else i++;
    }

    mp::mp_object brd=mp::make_object("POLYLINE 0x1D * *");
    brd.push_back(cnv.line_bck(rect2line(cutter)));
    brd.Opts["Source"] = "border";
    M.push_back(brd);

    mp::write(file, M);
  }
  else { cerr << "ERR: file is not .fig or .mp\n"; return 1; }

  cerr << " removed " << obj_cnt2 << " objects from " << obj_cnt1 << "\n";
  return 0;
}

/*****************************************************/
/// show map object range (fig|mp)
int range(int argc, char** argv){

  if (argc != 3){
    cerr << "Get range.\n"
         << "  usage: mapsoft_vmap range <proj> <datum> <fig|mp>\n";
    return 1;
  }

  string proj  = argv[0];
  string datum = argv[1];
  string file  = argv[2];

  dRect ret;

  if (testext(file, ".fig")){
    fig::fig_world F;
    if (!fig::read(file.c_str(), F)) {
      cerr << "ERR: bad fig file\n"; return 1;
    }

    string style=F.opts.get<string>("style", "default");
    zn::zn_conv zconverter(style);

    g_map ref = fig::get_ref(F);
    if (ref.size()<3){
      cerr << "ERR: not a GEO-fig\n"; return 1;
    }

    fig::fig_world::iterator i=F.begin();
    while (i!=F.end()){
      if (zn::is_to_skip(*i) || !zconverter.is_map_depth(*i)) { i++; continue;}
      ret=rect_bounding_box(ret, dRect(i->range()));
      i++;
    }
    Options O;
    if (Proj(proj) == Proj("tmerc")){
      O.put("lon0", convs::lon2lon0(ref.center().x));
      O.put("E0",   500000.0);
      O.put("N0",   0.0);
    }
    convs::map2pt cnv(ref, Datum(datum), Proj(proj),O);
    std::cout << cnv.bb_frw(ret);
  }

  else if (testext(file, ".mp")){
    mp::mp_world M;
    if (!mp::read(file.c_str(), M)) {
      cerr << "ERR: bad mp file\n"; return 1;
    }

    mp::mp_world::iterator i=M.begin();
    while (i!=M.end()){
      mp::mp_object::iterator l=i->begin();
      while (l!=i->end()){
        ret=rect_bounding_box(ret, l->range());
        l++;
      }
      i++;
    }
    convs::pt2pt cnv(Datum("wgs84"), Proj("lonlat"), Options(),
                     Datum(datum), Proj(proj), Options());
    std::cout << cnv.bb_frw(ret, 1e-6);
  }
  else { cerr << "ERR: file is not .fig or .mp\n"; return 1; }

  return 0;
}


/*****************************************************/
/// Удалить сетку и т.п.оформление (fig). Остаются: привязка, подписи, объекты
int remove_grids(int argc, char** argv){

  if (argc < 1){
    cerr << "Remove non-map objects from fig. Keep reference, labels, map-objects\n"
         << "  usage: mapsoft_vmap remove_grids <fig>\n";
    return 1;
  }

  string file   = argv[0];

  cerr << "removing non-map objects from " << file <<": ";

  int obj_cnt=0;

  if (!testext(file, ".fig")){ cerr << "ERR: file is not a .fig\n"; return 1;}

  fig::fig_world F;
  if (!fig::read(file.c_str(), F)) {
    cerr << "ERR: bad fig file\n"; return 1;
  }

  string style=F.opts.get<string>("style", "default");
  zn::zn_conv zconverter(style);

  fig::fig_world::iterator i=F.begin();
  while (i!=F.end()){
    if (!zconverter.is_map_depth(*i) &&
        (i->opts.get("MapType", string()) != "label") &&
        ((i->comment.size()==0) || (0!=strncmp(i->comment[0].c_str(), "REF",3))))
           {i=F.erase(i); obj_cnt++;}
    else i++;
  }
  fig::write(file, F);

  cerr << obj_cnt << " fig objects removed\n";

  return 0;
}

/*****************************************************/
/// Удалить подписи из (fig).
int remove_labels(int argc, char** argv){

  if (argc < 1){
    cerr << "Remove labels from fig.\n"
         << "  usage: mapsoft_vmap remove_labels <fig>\n";
    return 1;
  }

  string file   = argv[0];

  cerr << "removing labels objects from " << file <<": ";

  int obj_cnt=0;

  if (!testext(file, ".fig")){ cerr << "ERR: file is not a .fig\n"; return 1;}

  fig::fig_world F;
  if (!fig::read(file.c_str(), F)) {
    cerr << "ERR: bad fig file\n"; return 1;
  }

  fig::fig_world::iterator i=F.begin();
  while (i!=F.end()){
    if (i->opts.get("MapType", string()) == "label")
      {i=F.erase(i); obj_cnt++;}
    else i++;
  }
  fig::write(file, F);

  cerr << obj_cnt << " labels removed\n";

  return 0;
}

/*****************************************************/
/// Копировать все подписи (объекты, имеющие ключ подписи) из (fig) в (fig).
int copy_labels(int argc, char** argv){

  if (argc < 2){
    cerr << "Copy labels from fig to fig.\n"
         << "  usage: mapsoft_vmap copy_labels <infig> <outfig>\n";
    return 1;
  }

  string ifile   = argv[0];
  string ofile   = argv[1];

  cerr << "copying labels from " << ifile <<" to " << ofile << ": ";

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
    if (i->opts.get("MapType", string()) != "label") continue;
    obj_cnt++;
    fig::fig_object j=*i; j.clear();
    j.set_points(cnv.line_frw(*i));

    dPoint p=i->opts.get("RefPt", iPoint());
    cnv.frw(p);
    j.opts.put("RefPt", iPoint(p));

    OF.push_back(j);
  }
  fig::write(ofile, OF);

  cerr << obj_cnt << " labels copied\n";

  return 0;
}

/*****************************************************/
/// Count objects with different sources (fig|mp)
int show_sources(int argc, char** argv){

  if (argc != 1){
    cerr << "Show sources.\n"
         << "  usage: mapsoft_vmap show_sources <fig|mp>\n";
    return 1;
  }

  map<string, int> cnt;

  string file     = argv[0];

  if (testext(file, ".fig")){
    fig::fig_world F;
    if (!fig::read(file.c_str(), F)) {
      cerr << "ERR: bad fig file\n"; return 1;
    }

    string style=F.opts.get<string>("style", "default");
    zn::zn_conv zconverter(style);

    int maxid=0;
    for (fig::fig_world::iterator i=F.begin(); i!=F.end(); i++){
      if (zn::is_to_skip(*i) || !zconverter.is_map_depth(*i)) continue;
      string s = i->opts.get("Source", string());
      if (cnt.count(s)==0) cnt[s]=1;
      else cnt[s]++;
    }
  }
  else if (testext(file, ".mp")){
    mp::mp_world M;
    if (!mp::read(file.c_str(), M)) {
      cerr << "ERR: bad mp file\n"; return 1;
    }

    int maxid=0;
    for (mp::mp_world::iterator i=M.begin(); i!=M.end(); i++){
      string s = i->Opts.get("Source", string());
      if (cnt.count(s)==0) cnt[s]=1;
      else cnt[s]++;
    }
  }
  else { cerr << "ERR: file is not .fig or .mp\n"; return 1; }

  for (map<string,int>::const_iterator i=cnt.begin(); i!=cnt.end(); i++){
    cout << i->first << "\t" << i->second << "\n";
  }
  return 0;
}

/*****************************************************/
/// Set source parameter for each object
int set_source(int argc, char** argv){
  if (argc != 2){
    cerr << "Make all objects and their labels to be in one map (fig).\n"
         << "  usage: mapsoft_vmap set_source <fig> <source>\n";
    return 1;
  }
  string file   = argv[0];
  string source = argv[1];

  int o_cnt=0;

  cerr << "setting source parameter to " << source <<": ";

  if (testext(file, ".fig")){
    fig::fig_world F;
    if (!fig::read(file.c_str(), F)) {
      cerr << "ERR: bad fig file\n"; return 1;
    }

    string style=F.opts.get<string>("style", "default");
    zn::zn_conv zconverter(style);


    fig::fig_world::iterator i=F.begin();
    for (i=F.begin(); i!=F.end(); i++){
      if (zn::is_to_skip(*i) || !zconverter.is_map_depth(*i)) continue;
      i->opts["Source"]=source;
      o_cnt++;
    }
    fig::write(file, F);
  }
  else if (testext(file, ".mp")){
    mp::mp_world M;
    if (!mp::read(file.c_str(), M)) {
      cerr << "ERR: bad mp file\n"; return 1;
    }
    mp::mp_world::iterator i=M.begin();
    for (i=M.begin(); i!=M.end(); i++){
      i->Opts["Source"]=source;
      o_cnt++;
    }
    mp::write(file, M);
  }
  else { cerr << "ERR: file is not .fig or .mp\n"; return 1; }

  cerr << o_cnt << " objects\n";
  return 0;
}

/*****************************************************/
int main(int argc, char** argv){
  if (argc < 2){
    cerr << "usage: mapsoft_vmap <command> <args>\n"
         << "commands: \n"
         << "  - range_crop    -- crop map objects in (fig|mp)\n"
         << "  - range_select  -- \n"
         << "  - range_remove  -- \n"
         << "  - range         -- get range of map objects in (fig|mp)\n"

         << "  - copy          -- copy map objects from (fig|mp) to (fig|mp)\n"
         << "  - remove        -- remove map objects from (fig|mp) map\n"

         << "  - remove_grids  -- remove all but reference, lables, and map objects from fig\n"
         << "  - remove_labels -- remove lables from fig\n"
         << "  - copy_labels   -- copy all labels from (fig) to (fig)\n"

         << "  - update        -- update labels and pics on a fig\n"
         << "  - show_sources  -- show sources in (fig|mp)\n"
         << "  - set_source    -- set source parameter for each object\n"
;
    exit(0);
  }
  if (strcmp(argv[1], "range_crop")==0)     exit(crop("range_crop", argc-2, argv+2));
  if (strcmp(argv[1], "range_select")==0)   exit(crop("range_select",  argc-2, argv+2));
  if (strcmp(argv[1], "range_remove")==0)   exit(crop("range_remove",  argc-2, argv+2));

  if (strcmp(argv[1], "range")==0)          exit(range(argc-2, argv+2));

  if (strcmp(argv[1], "copy")==0)           exit(copy(argc-2, argv+2));
  if (strcmp(argv[1], "remove")==0)         exit(remove(argc-2, argv+2));

  if (strcmp(argv[1], "remove_grids")==0)   exit(remove_grids(argc-2, argv+2));
  if (strcmp(argv[1], "remove_labels")==0)  exit(remove_labels(argc-2, argv+2));
  if (strcmp(argv[1], "copy_labels")==0)    exit(copy_labels(argc-2, argv+2));

  if (strcmp(argv[1], "update")==0)         exit(update(argc-2, argv+2));
  if (strcmp(argv[1], "show_sources")==0)   exit(show_sources(argc-2, argv+2));
  if (strcmp(argv[1], "set_source")==0)     exit(set_source(argc-2, argv+2));

  cerr << "unknown command: " << argv[1] << "\n";
  exit(1);
}
