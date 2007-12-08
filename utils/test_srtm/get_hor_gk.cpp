#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <stdexcept>
#include <cmath>

#include "../srtm3.h"
#include "../line.h"
#include "../point_int.h"
#include "../../geo_io/mp.h"
#include "../../geo_io/geo_data.h"
#include "../../geo_io/geo_convs.h"

// Проверка работы srtm3.h, line.h, point_int.h 
// Автоматическое построение карты района (горизонтали, вершины и т.п.)...
// Все делается в прямоугольнике в координатах ГК СК Пулково!

using namespace std;

void usage(){
    cerr << "usage: get_hor_gk lon0 x1 x2 y1 y2 step1 step2 srtm_dir > out.mp\n";
    exit(0);
}

//координаты угла по его номеру
Point<int> crn (int k){
  k%=4;
  return Point<int>(k/2, (k%3>0)?1:0);
}
//направление следующей за углом стороны (единичный вектор)
Point<int> dir (int k){
  return crn(k+1)-crn(k);
}


main(int argc, char** argv){
  if (argc != 9) usage();
  string lon0  = argv[1];
  double x1  = atof(argv[2]);
  double x2  = atof(argv[3]);
  double y1  = atof(argv[4]);
  double y2  = atof(argv[5]);
  int step1 = atoi(argv[6]);
  int step2 = atoi(argv[7]);
  string srtm_dir = argv[8];

  double acc = 10; // "точность", в метрах - используется при 
  // преобразовании границы карты и при генерализации горизонталей.

  if (x2<x1)   swap(x2,x1);
  if (y2<y1)   swap(y2,y1);
  if (step2<step1) swap(step2,step1);

  cerr << 
    "Рисование горизонталей и вершин по данным srtm\n"
    "Координаты границ района за даны в проекции ГК, в СК Пулково-42\n"
    "x = " << x1 << " - " << x2 << "\n"
    "y = " << y1 << " - " << y2 << "\n"
    "Осевой меридиан " << lon0 << " градусов\n"
    "точность " << acc << " м (используется для преобразования границ района\n" 
    "и для генерализации горизонталей\n"
    "srtm_dir: "<< srtm_dir<<"\n\n";


  srtm3 s(srtm_dir, 10, interp_mode_off);
  mp::mp_world MP;


  // нам нужно получить границы области в координатах lonlat.
  // для этого делаем привязку карты с размером точки,
  // соответствующей acc м.

  Options O;
  O["lon0"] = lon0;
  convs::pt2pt c1(Datum("pulkovo"), Proj("tmerc"), O,
                  Datum("wgs84"), Proj("lonlat"), Options());
  g_line brd;
  brd.push_back(g_point(x1,y1));
  brd.push_back(g_point(x1,y2));
  brd.push_back(g_point(x2,y2));
  brd.push_back(g_point(x2,y1));
  g_line brdll = c1.line_frw(brd, acc);

  if (brdll.size()<3) {cerr << "can't find ll boundary\n"; exit(0);}
  g_point min(brdll[0]), max(brdll[0]);
  for (int i=0; i<brdll.size(); i++){
    if (min.x > brdll[i].x) min.x = brdll[i].x;
    if (min.y > brdll[i].y) min.y = brdll[i].y;
    if (max.x < brdll[i].x) max.x = brdll[i].x;
    if (max.y < brdll[i].y) max.y = brdll[i].y;
  }

  cerr << "строим горизонтали в районе\n"
    "lon: " << min.x << " - " << max.x << "\n"
    "lat: " << min.y << " - " << max.y << "\n\n";
  
  int lon1  = int(floor(1200*min.x));
  int lon2  = int(ceil(1200*max.x));
  int lat1  = int(floor(1200*min.y));
  int lat2  = int(ceil(1200*max.y));

  // нарисуем горизонтали!
  cerr << "находим кусочки горизонталей: ";
  map<short, list<Line<double> > > hors;
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
          Line<double> hor;
          hor.push_back((Point<double>(p) + Point<double>(crn(int(x1))) + Point<double>(dir(int(x1)))*double(x1-int(x1)))/1200.0);
          hor.push_back((Point<double>(p) + Point<double>(crn(int(x2))) + Point<double>(dir(int(x2)))*double(x2-int(x2)))/1200.0);
          hors[h].push_back(hor);
          h=srtm_undef;
          count+=hor.size();
        }
      }
    }
  }
  cerr << count << " шт\n";

  count = 0; 
  cerr << "  сливаем кусочки горизонталей в линии: ";
  for(map<short, list<Line<double> > >::iterator im = hors.begin(); im!=hors.end(); im++){
    std::cerr << im->first << " ";
    merge(im->second, 1e-4);
    generalize(im->second, acc/6380000/2/M_PI*180.0);
    split(im->second, 100);
    
    for(list<Line<double> >::iterator iv = im->second.begin(); iv!=im->second.end(); iv++){
      if (iv->size()<3) continue;
      mp::mp_object O;
      O.Class = "POLYLINE";
      ostringstream s; s << im->first;
      O.Label = s.str();
      O.Type = 0x21;
      if (im->first%step2==0) O.Type = 0x22;
      O.insert(O.end(), iv->begin(), iv->end());
      MP.push_back(O);
      count++;
    }
  }
  cerr << count << " шт\n";
  

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
  count = 0;
  cerr << "ищем вершины: ";
  
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
          mp::mp_object mpo;
          mpo.Class = "POI";
          ostringstream s; s << h;
          mpo.Label = s.str();
          mpo.Type = 0x1100;
          mpo.push_back(Point<double>(p)/1200.0);
          MP.push_back(mpo);
          count++;
          break;
        }

        add_pb(maxpt, pts, brd);
        done.insert(maxpt);

      } while (true);
    }
  }
  cerr << count << " шт\n";

  std::set<Point<int> > aset;
  std::list<Line<double> > aline;


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
      Point<double> gr(double(hx-h)/londeg, double(hy-h)/latdeg);
      double a = atan(pdist(gr))*180/M_PI;
      if (a > 45) aset.insert(p);
    }
  }
  cerr << aset.size() << " точек\n";

  cerr << " преобразуем множество точек в многоугольники: ";
  aline = pset2line(aset);
  for(list<Line<double> >::iterator iv = aline.begin(); iv!=aline.end(); iv++){
    if (iv->size()<3) continue;
    Line<double> l = (*iv)/1200.0;
    mp::mp_object mpo;
    mpo.Class = "POLYGON";
    mpo.Label = "high slope";
    mpo.Type = 0x19;
    mpo.insert(mpo.end(), l.begin(), l.end());
    MP.push_back(mpo);
  }
  cerr << aline.size() << " шт\n";
*/
  // поиск дырок
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
  for(list<Line<double> >::iterator iv = aline.begin(); iv!=aline.end(); iv++){
    if (iv->size()<3) continue;
    Line<double> l = (*iv)/1200.0;
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
    list<Line<double> > lines; lines.push_back(*i);
    crop_lines(lines, brdll);
    i->clear();
    for (list<Line<double> >::iterator j = lines.begin(); j != lines.end(); j++){
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


}
