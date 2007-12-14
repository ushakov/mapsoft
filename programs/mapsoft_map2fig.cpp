#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <sys/stat.h>

#include "../utils/point.h"
#include "../layers/layer_geomap.h"
#include "../layers/layer_google.h"
#include "../layers/layer_ks.h"
#include "../geo_io/io.h"
#include "../geo_io/geo_convs.h"

#include "../geo_io/geofig.h"

// добавление растровой карты в fig-файл.
// диапазон карты - по объекту с комментарием BRD или по диапазону точек привязки...
// карта режится на куски размером не более tsize x tsize
// картинки кладутся в директорию <fig>.img под названиями m1-1.jpg m1-2.jpg и т.д.

const int tsize = 1024;

void usage(){
  std::cerr << "usage: "
    "mapsoft_map2fig <fig> map <map file 1> [<map file 2> ...]\n";
    "mapsoft_map2fig <fig> google <scale> <google_dir>\n";
    "mapsoft_map2fig <fig> ks <scale> <ks_dir>\n";
   exit(0);
}

int
main(int argc, char **argv){
    if (argc<4) usage();

    std::string fig_name = argv[1];
    std::string source   = argv[2];
    std::string depth;

    LayerGeo *ml;    


    if (source == "map") {
      geo_data *world = new geo_data;
      ml = new LayerGeoMap(world);
      for(int i=3;i<argc;i++)
        io::in(std::string(argv[i]), *world, Options());
      depth = "500"; 
   
    }
    else if (source == "ks") {
      if (argc!=5) usage();
      LayerKS *l = new LayerKS(argv[4], atoi(argv[3]));
      l->set_downloading(true);
      ml=l;
      depth = "501"; 
    }
    else if (source == "google") {
      if (argc!=5) usage();
      LayerGoogle *l = new LayerGoogle(argv[4], atoi(argv[3]));
      l->set_downloading(true);
      ml=l;
      depth = "502"; 
    } else usage();

    // читаем fig
    fig::fig_world F = fig::read(fig_name.c_str());
    g_map fig_ref = fig::get_ref(F);

    // в LayerGeo установим привязку, подходящую для карты
    ml->set_ref();

    // нам нужно установить туда привязку fig-файла, но перемаштабированную
    // нужным образом 
    g_map map_ref = ml->get_ref();


    // rescale > 1, если точки fig меньше точки растра
    double rescale;
    if (source == "map") rescale = convs::map_mpp(map_ref)/convs::map_mpp(fig_ref);
    else { // с google map_mpp не работает (т.к. во всех точках все разное)
      // например, горизонтальный масштаб
      Rect<double> range = fig_ref.border.range();
      g_point p1(range.TLC()), p2(range.TRC());
      convs::map2map c(map_ref, fig_ref);
      double l1=0,l2=0;
      for (int i=1; i<fig_ref.size();i++){
        g_point p1(fig_ref[i-1].xr,fig_ref[i-1].yr);
        g_point p2(fig_ref[i].xr,  fig_ref[i].yr);
        c.bck(p1); c.bck(p2);
        l1+=pdist(p1,p2);
        l2+=pdist(g_point(fig_ref[i].xr, fig_ref[i].yr), g_point(fig_ref[i-1].xr, fig_ref[i-1].yr));
      }
      rescale = l2/l1;
    }


    fig_ref/=rescale; // теперь fig_ref - в координатах растра

    ml->set_ref(fig_ref);

    // диапазон картинки в координатах растра
    Rect<double> range = fig_ref.border.range();

    // создадим директорию для картинок
    std::string dir_name = fig_name + ".img";

    struct stat st;
    if (stat(dir_name.c_str(), &st)!=0){
      if (mkdir(dir_name.c_str(), 0755)!=0){
        std::cerr << "can't mkdir " << dir_name << "\n";
        exit(0);
      }
    }
    else if (!S_ISDIR(st.st_mode)){
      std::cerr << dir_name << " is not a directory\n";
      exit(0);
    }

    int nx = int(range.w)/(tsize-1)+1; // число плиток
    int ny = int(range.h)/(tsize-1)+1;
    double dx = range.w / double(nx);  // размер плиток
    double dy = range.h / double(ny);

std::cerr << " rescale: " << rescale << "\n";
std::cerr << " range: " << range << "\n";
std::cerr << nx << " x " << ny << " tiles\n";
std::cerr << dx << " x " << dy << " tile_size\n";

    for (int j = 0; j<ny; j++){
    for (int i = 0; i<nx; i++){
      g_point tlc(range.x+i*dx, range.y+j*dy);

      Image<int> im = ml->get_image(Rect<int>(tlc, tlc+g_point(dx,dy)));
      std::ostringstream fname; fname << dir_name << "/" << source[0] << i << "-" << j << ".jpg"; 
      image_r::save(im, fname.str().c_str(), Options());

      fig::fig_object o = fig::make_object("2 5 0 1 0 -1 "+depth+" -1 -1 0.000 0 0 -1 0 0 *");
      
      o.push_back(tlc*rescale);
      o.push_back((tlc+g_point(dx,0))*rescale);
      o.push_back((tlc+g_point(dx,dy))*rescale);
      o.push_back((tlc+g_point(0,dy))*rescale);
      o.push_back(tlc*rescale);
      o.image_file = fname.str();
      F.push_back(o);
    }
    }
    std::ofstream f(fig_name.c_str());
    fig::write(f, F);
    delete(ml);
}
