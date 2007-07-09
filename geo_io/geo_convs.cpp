#include "geo_convs.h"
#include <cmath>
#include <sstream>

#include "../loaders/image_r.h" // определение размеров картинки (image_r::size)
#include "../jeeps/gpsdatum.h"
#include "../jeeps/gpsproj.h"
#include "../jeeps/gpsmath.h"

namespace convs{
using namespace std;

// преобразование геодезических координат
// точки преобразуются по ссылке, чтобы можно было не копируя
// преобразовывать координаты в сложных штуках типа g_waypoint

pt2ll::pt2ll(const Datum & D, const Proj & P, const Options & Po){
    datum = D;
    proj = P;
    a = GPS_Ellipse[GPS_Datum[D.n].ellipse].a;
    f = 1/GPS_Ellipse[GPS_Datum[D.n].ellipse].invf;
    // разбираем параметры
    switch (proj.n){
	case 0: // lonlat
	  return;
	case 1: // tmerc
	  lon0 = Po.get_double("lon0",1e99);
          lat0 = Po.get_double("lat0",0);
          N0   = Po.get_double("N0",0);
          E0   = Po.get_double("E0",500000);
          k    = Po.get_double("k",1);
	  return;
	case 2: // UTM
          // Я не знаю, какие здесь нужны параметры... Разберемся потом.
          zone = Po.get_int("zone",0);
          zc   = Po.get_char("zc",'C');
	  return;
        case 3: // merc
          // Я не знаю, какие здесь нужны параметры... Разберемся потом.
          lon0 = Po.get_double("lon0",0);
          lat0 = Po.get_double("lat0",0);
          N0   = Po.get_double("N0",0);
          E0   = Po.get_double("E0",0);
	  return;
        case 4: // google
	  return;
        default:
          std::cerr << "unknown proj: " << P.n << "\n";
          return;
    }
}

void pt2ll::frw(g_point & p) const{
  double x,y;
  // сперва преобразуем к широте-долготе
  double l0=lon0;
  switch (proj.n){
    case 0: break;
    case 1: // tmerc 
       // Мы можем использовать префикс в координате X, как на советских картах.
       // и это должно иметь приоритет перед явной установкой lon0!
       if (p.x>999999){
         l0=((int)(p.x/1e6)-1)*6+3;
         p.x -= floor(p.x/1e6)*1e6;
       }
       if (l0>1e90) l0=0;
//         std::cerr << "pt2ll::frw: setting lon0 to " << l0 << "\n";

       GPS_Math_TMerc_EN_To_LatLon(p.x, p.y, &y, &x, lat0, l0, E0, N0, k, a, a*(1-f));
       p.x = x; p.y = y;
       break;
    case 2: //UTM
       GPS_Math_UTM_EN_To_WGS84(&y, &x, p.x, p.y, zone, zc);
       p.x = x; p.y = y;
       break;
    case 3: // merc
       GPS_Math_Mercator_EN_To_LatLon(p.x, p.y, &y, &x, lat0, l0, E0, N0, a, a*(1-f));
       p.x = x; p.y = y;
       break;
    case 4: // google
       p.y = 360/M_PI*atan(exp(p.y*M_PI/180)) - 90;
       break;
    default:
       std::cerr << "unknown proj: " << proj.n << "\n";
       break;
  }
}

void pt2ll::bck(g_point & p){
  double x,y;

  // преобразуем в нужную нам проекцию
  switch (proj.n){
  case 0: return;
  case 1: //tmerc
    // если lon0 не указали явно - определим его автоматически:
    // причем (это важно) - только один раз, по первой точке!
    if (lon0>1e90){
      lon0 = floor( p.x/6.0 ) * 6 + 3;
//      std::cerr << "pt2ll::bck: setting lon0 to " << lon0 << " (lon = " << p.x << ")\n";
    }
    GPS_Math_TMerc_LatLon_To_EN(p.y, p.x, &x, &y, lat0, lon0, E0, N0, k, a, a*(1-f));
    // Добавим к координате префикс - как на советских картах:
    x += 1e6 * (floor((lon0-3)/6)+1);
    p.x = x; p.y = y;
    return;
  case 2: //UTM
    std::cerr << "conversion latlon -> utm is not supported. fixme!\n";
    return;
  case 3: // merc
    std::cerr << "conversion latlon -> utm is not supported. fixme!\n";
    return;
  case 4: // google
    p.y = 180/M_PI * log(tan(M_PI/4*(1+p.y/90.0)));
    return;
  default:
    std::cerr << "unknown proj: " << proj.n << "\n";
    return;
  }
}

ll2wgs::ll2wgs(const Datum & D): datum(D){}
void ll2wgs::frw(g_point & p) const{
  if (datum.n!=0){
    double x,y,h;
    GPS_Math_Known_Datum_To_WGS84_M(p.y, p.x, 0, &y, &x, &h, datum.n);
    p.x=x; p.y=y;
  }
}
void ll2wgs::bck(g_point & p) const{
  if (datum.n!=0){
    double x,y,h;
    GPS_Math_WGS84_To_Known_Datum_M(p.y, p.x, 0, &y, &x, &h, datum.n);
    p.x=x; p.y=y;
  }
}


pt2pt::pt2pt(const Datum & sD, const Proj & sP, const Options & sPo, 
             const Datum & dD, const Proj & dP, const Options & dPo):
pc1(sD,sP,sPo), pc2(dD,dP,dPo), dc1(sD), dc2(dD), 
triv1((sP.n==dP.n) && (sPo==dPo) && (sD.n==dD.n)), triv2(sD.n==dD.n){}

pt2pt::pt2pt(): triv1(true), triv2(true){}

void pt2pt::frw(g_point & p){
    if (triv1) return;
    pc1.frw(p);
    if (!triv2){
      dc1.frw(p);
      dc2.bck(p);
    }
    pc2.bck(p);
}
void pt2pt::bck(g_point & p){
    if (triv1) return;
    pc2.frw(p);
    if (!triv2){
      dc2.frw(p);
      dc1.bck(p);
    }
    pc1.bck(p);
}

/*******************************************************************/
/* Приведение матрицы к диагональному виду */
// здесь все неаккуратно написано: фукция подходит для любого N, 
// а макрос A7 предполагает N==6!
// кстати, A7 используется и дальше!
#define A7(x,y) a[(x)+(y)*7]

int mdiag(int N, double *a){
  int i,j,k,l;
  double tmp;

  for (k=0; k<N; ++k){  /* идем по строчкам сверху вниз */
    /* циклически переставляем строчки от k до N-1, чтобы на месте (k,k) встал не ноль */
    for (i=k; i<N; ++i){ if (A7(k,k)!=0) break;
      for (j=0;j<N+1;++j){ /* по всем столбцам */
        tmp=A7(j,k);        /* сохраним верхнее */
        for (l=k; l<N-1; ++l) A7(j,l)=A7(j,l+1); /* сдвинем */
        A7(j,N-1)=tmp; /* бывлее верхнее -- вниз */
      }
    }
    /* если так сделать нельзя, уравнения зависимы */
    if (A7(k,k)==0) return 1;
    /* делим строчку на A(k,k) (от N до k, т.к. в начале -- нули)*/
    for (j=N; j>=k; --j) A7(j,k)=A7(j,k)/A7(k,k);
    /* вычитаем из всех остальных строчек эту, помноженную на A(k,*) */
    for (i=0; i<N; ++i) if (i!=k) for (j=N; j>=k; --j) A7(j,i)-=A7(k,i)*A7(j,k);
  }
  return 0;
}


/*******************************************************************/
// преобразование из точки карты в геодезическую точку
// здесь же - выяснение всяких параметров карты (размер изображения, масштам метров/точку)
// сюда же - преобразование линий!

map2pt::map2pt(const g_map & sM, 
               const Datum & dD, const Proj & dP, const Options & dPo):
pc1(dD, sM.map_proj, dPo), pc2(dD, dP, dPo), dc(dD), 
border(sM.border){
  // идеи про преобразование карт - прежние:
  // считается, что преобразование СК замена осевого меридиана - линейны в пределах карты.

  // Разберемся с границей. Она нужна нам для всяких рисований и т.п. 
  // Но модифицировать карту мы не хотим - так что работаем со своей копией.
  // Граница должна или быть пустой или содержать больше двух точек.
  // если она пустая - можно попробовать определить ее по граф.файлу
  if ((border.size()>0)&&(border.size()<3)){
    cerr << "One or two points in border of map "
         << sM.comm << " (" << sM.file << ")\n";
    border.clear();
  }
  if (border.size()==0) {
    Point<int> wh = image_r::size(sM.file.c_str());
    border.push_back(g_point(0, 0));
    border.push_back(g_point(wh.x, 0));
    border.push_back(g_point(wh.x, wh.y));
    border.push_back(g_point(0, wh.y));
  }

  // А теперь следите за руками...

  // у нас точки привязки в lon-lat wgs84,
  // карта нарисована в некоторой другой проекции
  // а получить мы хотим третью проекцию.

  // при этом какая в какой СК нарисована карта и какие параметры проекции
  // используются - нам не важно - это станет частью лин.преобразования!

  // поэтому строятся три преобразования:
  // pt2ll  pc1(dD, sM.map_proj, dPo) -- из проекции карты в lon-lat (!!!)
  // pt2ll  pc2(dD, dP, dPo) -- из нужной нам проекции в lon-lat
  // ll2wgs dc(dD)

  // преобразуем (c подменой параметров) точки привязки в те координаты, 
  // в которых карта линейна и найдем соответствующее линейное преобразование.

  // копия точек привязки
  vector<g_refpoint> points = sM;

  //  a x + b y + c = X
  //  d x + e y + f = Y
  // надо решать такую систему:
  // d/d(a..f) SUMi(  a xi + b yi + c - Xi )^2 + SUMi(  d xi + e yi + f - Yi )^2 = 0

  // a Sxx + b Sxy + c Sx - SXx = 0
  // a Sxy + b Syy + c Sy - SXy = 0
  // a Sx  + b Sy  + c S  - SX  = 0
  // d Sxx + e Sxy + f Sx - SYx = 0
  // d Sxy + e Syy + f Sy - SYy = 0
  // d Sx  + e Sy  + f S  - SY  = 0

  // Sxx Sxy Sx 0   0   0  = SXx
  // Sxy Syy Sy 0   0   0  = SXy
  // Sx  Sy  S  0   0   0  = SX
  // 0   0   0  Sxx Sxy Sx = SYx
  // 0   0   0  Sxy Syy Sy = SYy
  // 0   0   0  Sx  Sy  S  = SY

  double a[6*7];
  for (int i=0; i<7*6; i++) a[i]=0;

  // попробуем повысить точность. 
  // мы не хотим работать с большими координатами (6000000м),
  // поэтому перед исчислением коэффициентов вычтем некоторое значение, а потом добавим его :)

/*  double lon0=0, lat0=0;*/

  for (unsigned i = 0; i < points.size(); i++){

    dc.bck(points[i]); // в нашу СК
    pc1.bck(points[i]); // в проекцию карты

    double x = points[i].xr;
    double y = points[i].yr;
    double lon = points[i].x;
    double lat = points[i].y;

/*    if ((lon0==0) && (lat0==0)){
	lon0=lon; lat0=lat;
    }
    lon-=lon0; lat-=lat0;*/


    A7(0,0)+=x*x; A7(3,3)+=x*x;
    A7(1,0)+=x*y; A7(4,3)+=x*y;
    A7(2,0)+=x;   A7(5,3)+=x;
    A7(0,1)+=x*y; A7(3,4)+=x*y;
    A7(1,1)+=y*y; A7(4,4)+=y*y;
    A7(2,1)+=y;   A7(5,4)+=y;
    A7(0,2)+=x; A7(3,5)+=x;
    A7(1,2)+=y; A7(4,5)+=y;
    A7(2,2)+=1; A7(5,5)+=1;

    A7(6,0)+=lon*x; A7(6,1)+=lon*y; A7(6,2)+=lon;
    A7(6,3)+=lat*x; A7(6,4)+=lat*y; A7(6,5)+=lat;
  }

  if (mdiag (6, a) != 0) {
    cerr << "Bad reference points for map "
         << sM.file << " (" << sM.comm << ")\n";
    for (int i=0;i<points.size();i++){
      cerr << "x=" << points[i].x << " y=" << points[i].y << "\n";
    }
    exit(0);
  }


  for (int i=0; i<6; i++){
    k_map2geo[i] = A7(6,i);
  }

/*  k_map2geo[2]+=lon0;
  k_map2geo[5]+=lat0;*/

  // Make the inverse transformation
  double D = k_map2geo[0] * k_map2geo[4] - k_map2geo[1] * k_map2geo[3];

  if (D==0) {
    cerr << "Bad reference points for map "
         << sM.file << " (" << sM.comm << ")\n";
    for (int i=0;i<points.size();i++){
      cerr << "x=" << points[i].x << " y=" << points[i].y << "\n";
    }
    exit(0);
  }

  k_geo2map[0] = k_map2geo[4] / D;
  k_geo2map[1] = - k_map2geo[1] / D;
  k_geo2map[2] = (k_map2geo[1] * k_map2geo[5] - k_map2geo[2] * k_map2geo[4]) / D;

  k_geo2map[3] = - k_map2geo[3] / D;
  k_geo2map[4] = k_map2geo[0] / D;
  k_geo2map[5] = (- k_map2geo[0] * k_map2geo[5] + k_map2geo[3] * k_map2geo[2]) / D;

  border_geo = line_frw(border);

}

void map2pt::frw(g_point & p){
  // линейное преобразование в проекцию карты, заданную pc1
  g_point p1(k_map2geo[0]*p.x + k_map2geo[1]*p.y + k_map2geo[2],
             k_map2geo[3]*p.x + k_map2geo[4]*p.y + k_map2geo[5]);

  // если карта в той же проекции, что нам нужна - то это все (!)
  
  if (pc1.proj.n != pc2.proj.n){
    pc1.frw(p1); // преобразование к lon-lat
    pc2.bck(p1); // к нужной проекции
  }
  p.x=p1.x;
  p.y=p1.y;

  return;
}

void map2pt::bck(g_point & p){
  if (pc1.proj.n != pc2.proj.n){
    pc2.frw(p); // преобразование к lon-lat
    pc1.bck(p); // к проекции карты
  }
  g_point p1(k_geo2map[0]*p.x + k_geo2map[1]*p.y + k_geo2map[2],
             k_geo2map[3]*p.x + k_geo2map[4]*p.y + k_geo2map[5]);
  p.x=p1.x;
  p.y=p1.y;

  return;
}

vector<g_point> map2pt::line_frw(const vector<g_point> & l) {

  vector<g_point> ret;
  // добавим первую точку
  if (l.size()==0) return ret;
  g_point P1 = l[0], P1a =P1; frw(P1a); ret.push_back(P1a);
  g_point P2, P2a;

  for (int i=1; i<l.size(); i++){
    P1 = l[i-1];
    P2 = l[i];
    do {
      P1a = P1; frw(P1a);
      P2a = P2; frw(P2a);
      g_point C1 = (P1+P2)/2; // середина отрезка
      g_point C2 = C1 + 0.5*g_point(P1.y-P2.y, -P1.x+P2.x)/pdist(P1,P2); // отступим на 0.5 в сторону от середины.
      g_point C1a = C1; frw(C1a);
      g_point C2a = C2; frw(C2a);
      if (pdist(C1a, (P1a+P2a)/2) < pdist(C1a,C2a)){
        ret.push_back(P2a);
        P1 = P2;
        P2 = l[i];
      }
      else {
        P2 = C1;
      }
    } while (P1!=P2);
  }
  return ret;
}


vector<g_point> map2pt::line_bck(const vector<g_point> & l) {

  vector<g_point> ret;
  // добавим первую точку
  if (l.size()==0) return ret;
  g_point P1 = l[0], P1a =P1; bck(P1a); ret.push_back(P1a);
  g_point P2, P2a;

  for (int i=1; i<l.size(); i++){
    P1 = l[i-1];
    P2 = l[i];
    do {
      P1a = P1; bck(P1a);
      P2a = P2; bck(P2a);
      g_point C1 = (P1+P2)/2; // середина отрезка
      g_point C1a = C1; bck(C1a);

      if (pdist(C1a, (P1a+P2a)/2) < 0.5){
        ret.push_back(P2a);
        P1 = P2;
        P2 = l[i];
      }
      else {
        P2 = C1;
      }
    } while (P1!=P2);
  }
  return ret;
}


/*******************************************************************/
// преобразование из карты в карту
// здесь может быть суровое разбиение карты на куски и аппроксимация линейными преобразованиями...
// здесь же - преобразование линий
// здесь же - преобразование картинок (с интерфейсом как у image loader'a)

map2map::map2map(const g_map & sM, const g_map & dM) :
    c1(sM, Datum("wgs84"), Proj("lonlat"), Options()),
    c2(dM, Datum("wgs84"), Proj("lonlat"), Options()),
    tst_frw(c1.border),
    tst_bck(c1.border)
{

  // нам надо, чтобы координаты преобразовывались правильно.
  // к сожалению, 

  // чтобы в преобразованиях pc1 и pc2 установился правильный осевой меридиан,
  // если его не установили явно, надо прогнать через них какую-то точку на карте.
  g_point p1(0,0);
  for (int i=0; i<dM.size(); i++){
    p1+=g_point(dM[i]);
  }
  p1/=dM.size();
  Options O;
  double lon0 = floor( p1.x/6.0 ) * 6 + 3;
  std::ostringstream slon0; slon0 << lon0;
  O["lon0"] = slon0.str();
  O["E0"] = "500000";

  c1 = map2pt(sM, Datum("wgs84"), sM.map_proj, O),
  c2 = map2pt(dM, Datum("wgs84"), sM.map_proj, O),

  border_src = c1.border;
  border_dst = line_frw(c1.border);
  tst_frw = border_tester(border_dst);
  tst_bck = border_tester(border_src);
}

void map2map::frw(g_point & p) {c1.frw(p); c2.bck(p);}
void map2map::bck(g_point & p) {c2.frw(p); c1.bck(p);}


vector<g_point> map2map::line_frw(const vector<g_point> & l) {

  vector<g_point> ret;
  // добавим первую точку
  if (l.size()==0) return ret;
  g_point P1 = l[0], P1a =P1; frw(P1a); ret.push_back(P1a);
  g_point P2, P2a;

  for (int i=1; i<l.size(); i++){
    P1 = l[i-1];
    P2 = l[i];
    do {
      P1a = P1; frw(P1a);
      P2a = P2; frw(P2a);
      g_point C1 = (P1+P2)/2; // середина отрезка
      g_point C1a = C1; frw(C1a);
      if (pdist(C1a, (P1a+P2a)/2) < 0.5){
        ret.push_back(P2a);
        P1 = P2;
        P2 = l[i];
      }
      else {
        P2 = C1;
      }
    } while (P1!=P2);
  }
  return ret;
}


vector<g_point> map2map::line_bck(const vector<g_point> & l) {

  vector<g_point> ret;
  // добавим первую точку
  if (l.size()==0) return ret;
  g_point P1 = l[0], P1a =P1; bck(P1a); ret.push_back(P1a);
  g_point P2, P2a;

  for (int i=1; i<l.size(); i++){
    P1 = l[i-1];
    P2 = l[i];
    do {
      P1a = P1; bck(P1a);
      P2a = P2; bck(P2a);
      g_point C1 = (P1+P2)/2; // середина отрезка
      g_point C1a = C1; bck(C1a);
      if (pdist(C1a, (P1a+P2a)/2) < 0.5){
        ret.push_back(P2a);
        P1 = P2;
        P2 = l[i];
      }
      else {
        P2 = C1;
      }
    } while (P1!=P2);
  }
  return ret;
}


// ****************

int map2map::image_frw(Image<int> & src_img, int src_scale, Rect<int> cnv_rect,
                       Image<int> & dst_img, Rect<int> dst_rect){
//    clip_rects_for_image_loader(
//      src_img.range(), src_img.range(), dst_img.range(), dst_rect);

    if (cnv_rect.empty() || dst_rect.empty()) return 1;

//      std::cerr  << "map2map: " << cnv_rect 
//                 << " -> " << dst_rect << " at " << dst_img << "\n";
    g_point p0;
    for (int dst_y = dst_rect.y; dst_y<dst_rect.y+dst_rect.h; dst_y++){
      // откуда мы хотим взять строчку
      p0.y = cnv_rect.y + ((dst_y-dst_rect.y)*cnv_rect.h)/dst_rect.h;

      for (int dst_x = dst_rect.x; dst_x<dst_rect.x+dst_rect.w; dst_x++){
        p0.x = cnv_rect.x + ((dst_x-dst_rect.x)*cnv_rect.w)/dst_rect.w;
	g_point p(p0);
        bck(p);
        if (!tst_bck.test(int(p.x), int(p.y))) continue;
	p/=src_scale;
	unsigned int c = src_img.safe_get(int(p.x),int(p.y));
	if (c != 0) {
	    dst_img.set_na(dst_x, dst_y, c);
	}
      }
    }
    return 0;
}

int map2map::image_bck(Image<int> & src_img, int src_scale, Rect<int> cnv_rect, 
                       Image<int> & dst_img, Rect<int> dst_rect){
//    clip_rects_for_image_loader(
//      src_img.range(), src_rect, dst_img.range(), dst_rect);
    if (cnv_rect.empty() || dst_rect.empty()) return 1;

    g_point p0;
    for (int dst_y = dst_rect.y; dst_y<dst_rect.y+dst_rect.h; dst_y++){
      // откуда мы хотим взять строчку
      p0.y = cnv_rect.y + ((dst_y-dst_rect.y)*cnv_rect.h)/dst_rect.h;
      for (int dst_x = dst_rect.x; dst_x<dst_rect.x+dst_rect.w; dst_x++){
        p0.x = cnv_rect.x + ((dst_x-dst_rect.x)*cnv_rect.w)/dst_rect.w;
//        if (cnv_x == cnv_rect.BRC().x) cnv_x--;
        g_point p(p0);
        if (!tst_bck.test(int(p.x), int(p.y))) continue;
        bck(p); p/=src_scale;
	unsigned int c = src_img.safe_get(int(p.x),int(p.y));
	if (c != 0) {
	    dst_img.set_na(dst_x, dst_y, c);
	}
      }
    }
    return 0;
}

Rect<int> map2map::bb_frw(const Rect<int> & R){
  vector<g_point> v1;
  v1.push_back(g_point(R.TLC().x, R.TLC().y));
  v1.push_back(g_point(R.TRC().x, R.TRC().y));
  v1.push_back(g_point(R.BRC().x, R.BRC().y));
  v1.push_back(g_point(R.BLC().x, R.BLC().y));
  v1.push_back(g_point(R.TLC().x, R.TLC().y));
  vector<g_point> v2 = line_frw(v1);
  double minx = v2[0].x, maxx = v2[0].x;
  double miny = v2[0].y, maxy = v2[0].y;
  for (vector<g_point>::const_iterator i = v2.begin(); i!=v2.end(); i++){
    if (i->x < minx) minx = i->x;
    if (i->x > maxx) maxx = i->x;
    if (i->y < miny) miny = i->y;
    if (i->y > maxy) maxy = i->y;
  }
  minx = floor(minx); miny = floor(miny);
  maxx = ceil(maxx);  maxy = ceil(maxy);
  return Rect<int>(int(minx), int(miny), int(maxx-minx), int(maxy-miny));
}

Rect<int> map2map::bb_bck(const Rect<int> & R){
  vector<g_point> v1;
  v1.push_back(g_point(R.TLC().x, R.TLC().y));
  v1.push_back(g_point(R.TRC().x, R.TRC().y));
  v1.push_back(g_point(R.BRC().x, R.BRC().y));
  v1.push_back(g_point(R.BLC().x, R.BLC().y));
  v1.push_back(g_point(R.TLC().x, R.TLC().y));
  vector<g_point> v2 = line_bck(v1);
  double minx = v2[0].x, maxx = v2[0].x;
  double miny = v2[0].y, maxy = v2[0].y;
  for (vector<g_point>::const_iterator i = v2.begin(); i!=v2.end(); i++){
    if (i->x < minx) minx = i->x;
    if (i->x > maxx) maxx = i->x;
    if (i->y < miny) miny = i->y;
    if (i->y > maxy) maxy = i->y;
  }
  minx = floor(minx); miny = floor(miny);
  maxx = ceil(maxx);  maxy = ceil(maxy);
  return Rect<int>(int(minx), int(miny), int(maxx-minx), int(maxy-miny));
}

Rect<double> map2pt::bb_frw(const Rect<int> & R){
  vector<g_point> v1;
  v1.push_back(g_point(R.TLC().x, R.TLC().y));
  v1.push_back(g_point(R.TRC().x, R.TRC().y));
  v1.push_back(g_point(R.BRC().x, R.BRC().y));
  v1.push_back(g_point(R.BLC().x, R.BLC().y));
  v1.push_back(g_point(R.TLC().x, R.TLC().y));
  vector<g_point> v2 = line_frw(v1);
  double minx = v2[0].x, maxx = v2[0].x;
  double miny = v2[0].y, maxy = v2[0].y;
  for (vector<g_point>::const_iterator i = v2.begin(); i!=v2.end(); i++){
    if (i->x < minx) minx = i->x;
    if (i->x > maxx) maxx = i->x;
    if (i->y < miny) miny = i->y;
    if (i->y > maxy) maxy = i->y;
  }
  minx = floor(minx); miny = floor(miny);
  maxx = ceil(maxx);  maxy = ceil(maxy);
  return Rect<double>(minx, miny, maxx-minx, maxy-miny);
}

Rect<int> map2pt::bb_bck(const Rect<double> & R){
  vector<g_point> v1;
  v1.push_back(g_point(R.TLC().x, R.TLC().y));
  v1.push_back(g_point(R.TRC().x, R.TRC().y));
  v1.push_back(g_point(R.BRC().x, R.BRC().y));
  v1.push_back(g_point(R.BLC().x, R.BLC().y));
  v1.push_back(g_point(R.TLC().x, R.TLC().y));
  vector<g_point> v2 = line_bck(v1);
  double minx = v2[0].x, maxx = v2[0].x;
  double miny = v2[0].y, maxy = v2[0].y;
  for (vector<g_point>::const_iterator i = v2.begin(); i!=v2.end(); i++){
    if (i->x < minx) minx = i->x;
    if (i->x > maxx) maxx = i->x;
    if (i->y < miny) miny = i->y;
    if (i->y > maxy) maxy = i->y;
  }
  minx = floor(minx); miny = floor(miny);
  maxx = ceil(maxx);  maxy = ceil(maxy);
  return Rect<int>(int(minx), int(miny), int(maxx-minx), int(maxy-miny));
}



// Быстрая проверка границ
  border_tester::border_tester(std::vector<g_point> & brd) : border(brd){
    sides.clear();
    int n = border.size();
    for (int i = 0; i < n; i++){
      side S;
      S.x1 = int(border[i%n].x);
      S.y1 = int(border[i%n].y);
      S.x2 = int(border[(i+1)%n].x);
      S.y2 = int(border[(i+1)%n].y);
      if (S.y1==S.y2) continue; // горизонтальные стороны не интересны
      S.k = double(S.x2-S.x1)/double(S.y2-S.y1);
      sides.push_back(S);
    }
  }

  // проверка, попадает ли точка в пределы границы
  bool border_tester::test(const int x, const int y) const{
    int k=0; // считаем число k пересечений сторон лучем (x,y) - (inf,y)
    int e = sides.size();
    for (int i = 0; i < e; ++i){
        side const & S = sides[i];
        if ((S.y1 > y)&&(S.y2 > y)) continue; // сторона выше луча
        if ((S.y1 < y)&&(S.y2 < y)) continue; // сторона ниже луча
        if ((S.x2 < x)&&(S.x1 < x)) continue; // вся сторона левее луча
        int x0 = int(S.k * double(y - S.y1)) + S.x1;
        if (x0 < x) continue; // сторона левее нашей точки

	// тут есть проблемы во-первых со стыками сторон, которые учитываются дважды,
        // а во вторых с нижней точкой v-образной границы, которая должна учитываться дважды.
        // решение такое: у сторон, идущих вниз не учитываем первую точку, 
        // а у сторон, идущих вверх - последнюю! 
        if (((S.y2<y)&&(S.y1==y)) || 
            ((S.y1<y)&&(S.y2==y))) continue;
        k++;
    }
    return k%2==1;
  }

  // расстояние до ближайшей границы справа
  int border_tester::nearest_border (const int x, const int y) const {
    int dist=0xFFFFFF;
    int k=0;

    int e = sides.size();
    for (int i = 0; i < e; ++i){
        side const & S = sides[i];
        if ((S.y1 > y)&&(S.y2 > y)) continue; // сторона выше луча
        if ((S.y1 < y)&&(S.y2 < y)) continue; // сторона ниже луча
        if ((S.x2 < x)&&(S.x1 < x)) continue; // вся сторона левее луча
        int x0 = int(S.k * double(y - S.y1)) + S.x1;

        if (x0 < x) continue; // сторона левее нашей точки
        if (((S.y2<y)&&(S.y1==y)) || 
            ((S.y1<y)&&(S.y2==y))) continue;
        k++;
        if (dist > x0 - x) dist = x0 - x;
    }
    return k%2==1 ? dist:-dist;
  }

  // проверка, "задевает" ли карта данный район
  bool border_tester::test_range(Rect<int> range) const{
    int lx = 0; int ly=0;
    int rx = 0; int ry=0;
    Point<int> p1 = range.TLC();
    Point<int> p2 = range.BRC();
    vector<g_point>::const_iterator p;
//    std::cerr << "brd::tst: " << p1 << " " << p2 << "\n";
    for (p = border.begin(); p !=border.end(); p++){
//    std::cerr << *p << " ";
      if (p->x < p1.x) lx++;
      if (p->x > p2.x) rx++;
      if (p->y < p1.y) ly++;
      if (p->y > p2.y) ry++;
    }
//    std::cerr << "\nbrd::tst: " << lx << " " << ly << " " << rx << " " << ry << "\n";

    int s = border.size();
    return !((lx == s) ||
             (ly == s) ||
             (rx == s) ||
             (ry == s));
  }


g_map mymap(const geo_data & world){ // естественная привязка геоданных
    // тип проекции -- по первой карте, или lonlat, если карт нет
    // осевой меридиан -- 6n+3, наиболее близкий к середине диапазона треков и точек,
    // а если их нет - к середине диапазона карт

    g_map ret;
    if (world.maps.size()>0) ret.map_proj=world.maps[0].map_proj;
    Rect<double> rd=world.range_geodata();
    Rect<double> rm=world.range_map();
    double lon0 = rm.x+rm.w/2;
    if (!rd.empty()) lon0=rd.x+rd.w/2;
    lon0 = floor( lon0/6.0 ) * 6 + 3;
    std::ostringstream slon0; slon0 << lon0;
    Options O;
    O["lon0"] = slon0.str();
    O["E0"] = "500000";
    // масштаб -- соответствующий минимальному масштабу карт, если они есть,
    // или 1/3600 градуса на точку, если карт нет
    double mpp=1e99;
    for (int i=0;i<world.maps.size();i++){ if (mpp>map_mpp(world.maps[i])) mpp=map_mpp(world.maps[i]);}
    if ((mpp>1e90)||(mpp<1e-90)) mpp=1/3600.0;


    // точки привязки
    pt2pt cnv(Datum("WGS84"), ret.map_proj, O, Datum("WGS84"), Proj("lonlat"), O);
    g_point p(lon0,0); cnv.bck(p);
    g_point p1=p+g_point(mpp*1000,0); cnv.frw(p1);
    g_point p2=p+g_point(0,mpp*1000); cnv.frw(p2);
    cnv.frw(p);

    ret.push_back(g_refpoint(p.x,p.y,   0,1000));
    ret.push_back(g_refpoint(p1.x,p1.y, 1000,1000));
    ret.push_back(g_refpoint(p2.x,p2.y, 0,0));
    // чтоб не пытались определять границы из файла
    for (int i=0;i<3;i++)
      ret.border.push_back(g_point(0,0));
    return ret;
}

double map_mpp(const g_map &map){ // масштаб, метров или градусов (в зав.от проекции) в точке
  if (map.size()<3) return 0;
  double l1=0, l2=0;
  convs::pt2pt c(Datum("wgs84"), map.map_proj, Options(), Datum("wgs84"), Proj("lonlat"), Options());
  for (int i=1; i<map.size();i++){
    g_point p1(map[i-1].x,map[i-1].y);
    g_point p2(map[i].x,  map[i].y);
    c.bck(p1); c.bck(p2);
    l1+=pdist(p1,p2);
    l2+=pdist(g_point(map[i].xr, map[i].yr), g_point(map[i-1].xr, map[i-1].yr));
  }
  if (l2==0) return 0;
  return l1/l2;
}


}//namespace


