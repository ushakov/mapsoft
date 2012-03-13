#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <sys/stat.h>

#include "layers/layer_geomap.h"
#include "layers/layer_google.h"
#include "layers/layer_ks.h"
#include "geo_io/io.h"
#include "geo/geo_convs.h"
#include "loaders/image_r.h"

#include "geo_io/geofig.h"
#include "utils/err.h"

using namespace std;

// добавление растровой карты в fig-файл.
// диапазон карты - по объекту с комментарием BRD или по диапазону точек привязки...
// карта режится на куски размером не более tsize x tsize
// картинки кладутся в директорию <fig>.img под названиями m1-1.jpg m1-2.jpg и т.д.

const int tsize = 1024;

void usage(){
  cerr << "usage: \n"
    " mapsoft_map2fig <fig> map <depth> <map file 1> [<map file 2> ...]\n"
    " mapsoft_map2fig <fig> google <depth> <scale> <google_dir>\n"
    " mapsoft_map2fig <fig> ks <depth> <scale> <ks_dir>\n";
   exit(0);
}

int
main(int argc, char **argv){
    if (argc<5) usage();

    string fig_name = argv[1];
    string source   = argv[2];
    string depth = argv[3];

    LayerGeo *ml;
    g_map_list maps;

    if (source == "map") {
      // read data
      geo_data *world = new geo_data;
      for(int i=4;i<argc;i++){
        try {io::in(string(argv[i]), *world);}
        catch (MapsoftErr e) {cerr << e.str() << endl;}
      }
      // put all maps into one map_list
      for (vector<g_map_list>::const_iterator mi = world->maps.begin();
         mi!=world->maps.end(); mi++) maps.insert(maps.end(), mi->begin(), mi->end());
      ml = new LayerGeoMap(&maps);
    }
    else if (source == "ks") {
      if (argc!=6) usage();
      LayerKS *l = new LayerKS(argv[5], atoi(argv[4]));
      l->set_downloading(true);
      ml=l;
    }
    else if (source == "google") {
      if (argc!=6) usage();
      LayerGoogle *l = new LayerGoogle(argv[5], atoi(argv[4]));
      l->set_downloading(true);
      ml=l;
    } else usage();

    // читаем fig
    fig::fig_world F;
    if (!fig::read(fig_name.c_str(), F)) {
      cerr << "File is not modified, exiting.\n";
      exit(1);
    }

    // нам нужно установить в layer привязку fig-файла, но перемаштабированную
    // нужным образом 
    g_map fig_ref = fig::get_ref(F);
    g_map map_ref = ml->get_myref();

    // rescale > 1, если точки fig меньше точки растра
    double rescale;
    if (source == "map"){
      rescale = convs::map_mpp(map_ref, map_ref.map_proj)/
                convs::map_mpp(fig_ref, map_ref.map_proj); // scales in one proj
    }
    else { // с google map_mpp не работает (т.к. во всех точках все разное)
      // например, горизонтальный масштаб
      dRect range = fig_ref.border.range();
      dPoint p1(range.TLC()), p2(range.TRC());
      convs::map2map c(map_ref, fig_ref);


Options o; o["lon0"]="39";
convs::map2pt c1(fig_ref, Datum("WGS84"), Proj("tmerc"), o);
convs::map2pt c2(map_ref, Datum("WGS84"), Proj("tmerc"), o);

      double l1=0,l2=0;
      for (int i=1; i<fig_ref.size();i++){
        dPoint p1(fig_ref[i-1].xr,fig_ref[i-1].yr);
        dPoint p2(fig_ref[i].xr,  fig_ref[i].yr);
        c.bck(p1); c.bck(p2);
        l1+=pdist(p1,p2);
        l2+=pdist(dPoint(fig_ref[i].xr, fig_ref[i].yr), dPoint(fig_ref[i-1].xr, fig_ref[i-1].yr));
cerr << "fig->g: " << fig_ref[i].xr << " " << fig_ref[i].yr << " -> "
                        << p2 << "\n";
p2=dPoint(fig_ref[i].xr,  fig_ref[i].yr);
c1.frw(p2);
cerr << ": " << p2 << "\n";
c2.bck(p2);
cerr << ": " << p2 << "\n";
      }
      rescale = l2/l1;
    }


    fig_ref/=rescale; // теперь fig_ref - в координатах растра

    ml->set_ref(fig_ref);

    // диапазон картинки в координатах растра
    dRect range = fig_ref.border.range();

    // создадим директорию для картинок
    string dir_name = fig_name + ".img";

    struct stat st;
    if (stat(dir_name.c_str(), &st)!=0){
      if (mkdir(dir_name.c_str(), 0755)!=0){
        cerr << "can't mkdir " << dir_name << "\n";
        exit(0);
      }
    }
    else if (!S_ISDIR(st.st_mode)){
      cerr << dir_name << " is not a directory\n";
      exit(0);
    }

    int nx = int(range.w)/(tsize-1)+1; // число плиток
    int ny = int(range.h)/(tsize-1)+1;
    double dx = range.w / double(nx);  // размер плиток
    double dy = range.h / double(ny);

cerr << " rescale: " << rescale << "\n";
cerr << " range: " << range << "\n";
cerr << nx << " x " << ny << " tiles\n";
cerr << dx << " x " << dy << " tile_size\n";

    for (int j = 0; j<ny; j++){
    for (int i = 0; i<nx; i++){
      dPoint tlc(range.x+i*dx, range.y+j*dy);

      iImage im = ml->get_image(iRect(tlc, tlc+dPoint(dx,dy)));
      if (im.empty()) continue;
      ostringstream fname; fname << dir_name << "/" << source[0] << depth << "-" << i << "-" << j << ".jpg"; 
      image_r::save(im, fname.str().c_str(), Options());

      fig::fig_object o = fig::make_object("2 5 0 1 0 -1 "+depth+" -1 -1 0.000 0 0 -1 0 0 *");

      o.push_back(tlc*rescale);
      o.push_back((tlc+dPoint(dx,0))*rescale);
      o.push_back((tlc+dPoint(dx,dy))*rescale);
      o.push_back((tlc+dPoint(0,dy))*rescale);
      o.push_back(tlc*rescale);
      o.image_file = fname.str();
      o.comment.push_back("MAP "+fname.str());
      F.push_back(o);
    }
    }
    fig::write(fig_name, F);
    delete(ml);
}
