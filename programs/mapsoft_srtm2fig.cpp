#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <stdexcept>
#include <cmath>

#include "../utils/srtm3.h"
#include "../utils/line.h"
#include "../utils/point_int.h"
#include "../geo_io/geofig.h"
#include "../geo_io/geo_data.h"
#include "../geo_io/geo_convs.h"

// Перенесение данных srtm в привязанный fig-файл.

using namespace std;

void usage(){
    cerr << "usage: \n"
     " mapsoft_srtm2fig <fig> hor   <srtm_dir> <step1> <step2> [<acc, def=10>]\n"
     " mapsoft_srtm2fig <fig> ver   <srtm_dir> [<DH, def=20> [<PS, def=500>]] \n"
     " mapsoft_srtm2fig <fig> holes <srtm_dir>\n";
    exit(0);
}

//координаты угла единичного квадрата по его номеру
Point<int> crn (int k){
  k%=4;
  return Point<int>(k/2, (k%3>0)?1:0);
}
//направление следующей за углом стороны (единичный вектор)
Point<int> dir (int k){
  return crn(k+1)-crn(k);
}


main(int argc, char** argv){
  if (argc < 4) usage();

  std::string fig_name = argv[1];
  std::string cmd      = argv[2];
  std::string srtm_dir = argv[3];

  srtm3 s(srtm_dir, 10, interp_mode_off);

  // читаем fig
  fig::fig_world F = fig::read(fig_name.c_str());
  g_map fig_ref = fig::get_ref(F);
  convs::map2pt fig_cnv(fig_ref, Datum("wgs84"), Proj("lonlat"), Options());

  // диапазон картинки в lonlat
  Rect<double> range = fig_ref.range();
  int lon1  = int(floor(1200*range.TLC().x));
  int lon2  = int( ceil(1200*range.BRC().x));
  int lat1  = int(floor(1200*range.TLC().y));
  int lat2  = int( ceil(1200*range.BRC().y));

  // граница картинки в lonlat
  g_line border_ll = fig_cnv.line_frw(fig_ref.border);


  if (cmd == "hor"){
    if (argc < 6) usage();
    int step1 = atoi(argv[4]);
    int step2 = atoi(argv[5]);
    if (step2<step1) swap(step2,step1);
    double acc = 10; // "точность", в метрах - для генерализации горизонталей.
    if (argc>6) acc = atoi(argv[6]);

    cerr << "Рисование горизонталей по данным srtm\n";

    cerr << "находим кусочки горизонталей: ";
    map<short, list<g_line > > hors;
    int count = 0; 
    for (int lat=lat2; lat>lat1; lat--){
      for (int lon=lon1; lon<lon2; lon++){
  
        Point<int> p(lon,lat);
        // пересечения четырех сторон клетки с горизонталями:
        // при подсчетах мы опустим все данные на полметра,
        // чтоб не разбирать кучу случаев с попаданием горизонталей в узлы сетки
        multimap<short, double> pts;
  
        for (int k=0; k<4; k++){
          Point<int> p1 = p+crn(k);
          Point<int> p2 = p+crn(k+1);
          short h1 = s.geth(p1);
          short h2 = s.geth(p2);
          if ((h1<srtm_min) || (h2<srtm_min)) continue;
          int min = (h1<h2)? h1:h2;
          int max = (h1<h2)? h2:h1;
          min = int( floor(double(min)/step1)) * step1;
          max = int( ceil(double(max)/step1))  * step1;
          if (h2==h1) continue;
          for (int hh = min; hh<=max; hh+=step1){
            double x = double(hh-h1+0.1)/double(h2-h1);
            if ((x<0)||(x>1)) continue;
            pts.insert(pair<short, double>(hh,x+k));
          }
        } 
  
        // найдем, какие горизонтали пересекают квадрат дважды,
        // поместим их в список горизонталей hors
        short h=srtm_undef;
        double x1,x2;
  
        for (multimap<short,double>::const_iterator i=pts.begin(); i!=pts.end(); i++){
          if (h!=i->first){
            h  = i->first;
            x1 = i->second;
          } else{
            x2 = i->second;
            g_line hor;
            hor.push_back((g_point(p) + g_point(crn(int(x1))) + g_point(dir(int(x1)))*double(x1-int(x1)))/1200.0);
            hor.push_back((g_point(p) + g_point(crn(int(x2))) + g_point(dir(int(x2)))*double(x2-int(x2)))/1200.0);
            hors[h].push_back(hor);
            h=srtm_undef;
            count+=hor.size();
          }
        }
      }
    }
    cerr << " - " << count << " шт\n";
  

    count = 0; 
    cerr << "  сливаем кусочки горизонталей в линии: ";
    fig::fig_object o = fig::make_object("2 1 0 1 30453904 7 90 -1 -1 0.000 1 1 0 0 0 0");
    for(map<short, list<g_line> >::iterator im = hors.begin(); im!=hors.end(); im++){
      std::cerr << im->first << " ";
      merge(im->second, 1e-4);
      generalize(im->second, acc/6380000/2/M_PI*180.0);
      split(im->second, 200);
      list<g_line> tmp;
      crop_lines(im->second, tmp, border_ll, true);

      for(list<g_line>::iterator iv = im->second.begin(); iv!=im->second.end(); iv++){
        if (iv->size()<3) continue;
        o.clear();
        if (im->first%step2==0) o.thickness = 2;
        else o.thickness = 1;
        o.set_points(fig_cnv.line_bck(*iv));
        o.comment.clear();
        o.comment.push_back(boost::lexical_cast<std::string>(im->first));
        F.push_back(o);
        count++;
      }
    }
    cerr << count << " шт\n";

  } 
  else if (cmd == "ver"){
    // поиск вершин: 
    // 1. найдем все локальные максимумы (не забудем про максимумы из многих точек!)
    // 2. от каждого будем строить множество точек, добавляя наивысшую точку границы
    // 3. если высота последней добаленной точки ниже исходной более чем на DH м,
    //    или если размер множества больше PS точек - процедуру прекращаем, 
    //    объявляем исходную точку вершиной.
    // 4. Если высота последней добавленной точки больше исходной - процедуру
    //    прекращаем

    
    int DH = 20;
    int PS = 500;
    if (argc>4) DH = atoi(argv[4]);
    if (argc>5) PS = atoi(argv[5]);

    int count = 0;
    std::cerr << "ищем вершины: ";
    
    set<Point<int> > done;
    for (int lat=lat2; lat>lat1; lat--){
      for (int lon=lon1; lon<lon2-1; lon++){
  
        Point<int> p(lon,lat);
        if (done.find(p)!=done.end()) continue;
        short h = s.geth(p);
        if (h<srtm_min) continue;
  
        set<Point<int> > pts; pts.insert(p);
        set<Point<int> > brd = border(pts);
        // ищем максимум границы
  
        do{
          short max = srtm_undef;
          Point<int> maxpt;
          for (set<Point<int> >::const_iterator i = brd.begin(); i!=brd.end(); i++){
            short h1 = s.geth(*i);
            // исходная точка слишком близка к краю данных
            if ((h1<srtm_min) && (pdist(*i,p)<1.5)) {max = h1; break;}
            if (h1>max) {max = h1; maxpt=*i;}
          }
          if (max < srtm_min) break;
  
          // если максимум выше исходной точки - выходим.
          if (max > h) { break; }
  
          // если мы спустились от исходной точки более чем на DH или размер области более PS
          if ((h - max > DH ) || (pts.size() > PS)) {
            g_point p1 = g_point(p)/1200;
            if (!test_pt(p1, border_ll)) break;
            fig::fig_object o = fig::make_object("2 1 0 3 24 7  57 -1 -1 0.000 0 1 -1 0 0 1");
            fig_cnv.bck(p1);
            o.push_back(p1);
            o.comment.clear(); 
            o.comment.push_back(boost::lexical_cast<std::string>(h));
            F.push_back(o);
            count++;
            break;
          }
          add_pb(maxpt, pts, brd);
          done.insert(maxpt);
        } while (true);
      }
    }
    cerr << count << " шт\n";

  } 
  else if (cmd == "holes"){
  }
  else usage();

  std::ofstream f(fig_name.c_str());
  fig::write(f, F);


/*
  // поиск крутых склонов
  cerr << "ищем крутые склоны: ";
  double latdeg = 6380000/1200.0/180.0*M_PI; 
  double londeg = latdeg * cos(double(lat2+lat1)/2400.0/180.0*M_PI);

  for (int lat=lat2; lat>lat1; lat--){
    for (int lon=lon1; lon<lon2-1; lon++){
      Point<int> p(lon,lat);
      short h = s.geth(p);
      short hx = s.geth(p+Point<int>(1,0));
      short hy = s.geth(p+Point<int>(0,1));
      if ((h<srtm_min) || (hx<srtm_min) || (hy<srtm_min)) continue;
      g_point gr(double(hx-h)/londeg, double(hy-h)/latdeg);
      double a = atan(pdist(gr))*180/M_PI;
      if (a > 45) aset.insert(p);
    }
  }
  cerr << aset.size() << " точек\n";

  cerr << " преобразуем множество точек в многоугольники: ";
  aline = pset2line(aset);
  for(list<g_line>::iterator iv = aline.begin(); iv!=aline.end(); iv++){
    if (iv->size()<3) continue;
    g_line l = (*iv)/1200.0;
    mp::mp_object mpo;
    mpo.Class = "POLYGON";
    mpo.Label = "high slope";
    mpo.Type = 0x19;
    mpo.insert(mpo.end(), l.begin(), l.end());
    MP.push_back(mpo);
  }
  cerr << aline.size() << " шт\n";
*/

/*  // поиск дырок
  cerr << "ищем дырки srtm: ";
  aset.clear();
  aline.clear();
  for (int lat=lat2; lat>lat1; lat--){
    for (int lon=lon1; lon<lon2-1; lon++){
      Point<int> p(lon,lat);
      short h = s.geth(p);
      if (h==srtm_undef) aset.insert(p);
    }
  }
  cerr << aset.size() << " точек\n";

  cerr << " преобразуем множество точек в многоугольники: ";
  aline = pset2line(aset);
  for(list<g_line>::iterator iv = aline.begin(); iv!=aline.end(); iv++){
    if (iv->size()<3) continue;
    g_line l = (*iv)/1200.0;
    mp::mp_object mpo;
    mpo.Class = "POLYGON";
    mpo.Label = "no data";
    mpo.Type = 0xA;
    mpo.insert(mpo.end(), l.begin(), l.end());
    MP.push_back(mpo);
  }
  cerr << aline.size() << " шт\n";

  cerr << "Обрезаем данные до нужного нам района\n";


  // обрезание mp-файла - унести в какую-нибудь библиотеку!
  for(mp::mp_world::iterator i = MP.begin(); i!=MP.end(); i++){
    list<g_line> lines; lines.push_back(*i);
    crop_lines(lines, brdll);
    i->clear();
    for (list<g_line>::iterator j = lines.begin(); j != lines.end(); j++){
      mp::mp_object o = *i;
      o.insert(o.begin(), j->begin(), j->end());
      MP.insert(i, o);
    }
    i=MP.erase(i); i--;
  }

  mp::mp_object mpo;
  mpo.Class = "POLYLINE";
  mpo.Label = "border";
  mpo.Type = 0x16;
  mpo.insert(mpo.end(), brdll.begin(), brdll.end());
  mpo.push_back(brdll[0]);
  MP.push_back(mpo);

  cerr << "записываем все в файл: ";
  mp::write(cout, MP);
  cerr << MP.size() << " объектов\n";

*/
}
