#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <sys/stat.h>

#include "img_io/gobj_map.h"
#include "geo_io/io.h"
#include "geo/geo_convs.h"
#include "loaders/image_r.h"

#include "geo_io/geofig.h"
#include "err/err.h"

using namespace std;

// добавление растровой карты в fig-файл.
// диапазон карты - по объекту с комментарием BRD или по диапазону точек привязки...
// карта режится на куски размером не более tsize x tsize
// картинки кладутся в директорию <fig>.img под названиями m1-1.jpg m1-2.jpg и т.д.

const int tsize = 1024;

void usage(){
  cerr << "usage: \n"
    " mapsoft_map2fig <fig> map <depth> <map file 1> [<map file 2> ...]\n";
   exit(0);
}

int
main(int argc, char **argv){
    if (argc<5) usage();

    string fig_name = argv[1];
    string source   = argv[2];
    string depth = argv[3];


    if (source != "map") usage();

    // read data
    geo_data *world = new geo_data;
    for(int i=4;i<argc;i++){
      try {io::in(string(argv[i]), *world);}
      catch (Err e) {cerr << e.get_error() << endl;}
    }

    // put all maps into one map_list
    g_map_list maps;
    for (vector<g_map_list>::const_iterator mi = world->maps.begin();
       mi!=world->maps.end(); mi++) maps.insert(maps.end(), mi->begin(), mi->end());

    // create gobj, get ref
    GObjMAP ml(&maps);
    g_map map_ref = ml.get_myref();

    // read fig
    fig::fig_world F;
    if (!fig::read(fig_name.c_str(), F)) {
      cerr << "Read error. File is not modified, exiting.\n";
      exit(1);
    }

    // нам нужно установить в gobj привязку fig-файла, но перемаштабированную
    // нужным образом 
    g_map fig_ref = fig::get_ref(F);

    // rescale > 1, если точки fig меньше точки растра
    double rescale = convs::map_mpp(map_ref, map_ref.map_proj)/
                     convs::map_mpp(fig_ref, map_ref.map_proj); // scales in one proj

    fig_ref/=rescale; // теперь fig_ref - в координатах растра
    ml.set_ref(fig_ref);

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

      iImage im = ml.get_image(iRect(tlc, tlc+dPoint(dx,dy)));
      if (im.empty()) continue;
      ostringstream fname; fname << dir_name << "/" << source[0] << depth << "-" << i << "-" << j << ".jpg"; 
      image_r::save(im, fname.str().c_str());

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
}
