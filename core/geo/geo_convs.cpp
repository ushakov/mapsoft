#include "geo_convs.h"
#include <cmath>
#include <cstring>
#include <sstream>
#include <cassert>

#include "2d/line_utils.h"

#include "loaders/image_r.h" // определение размеров картинки (image_r::size)

namespace convs{
using namespace std;

// create PROJ4 projection object from our D, P and options
projPJ mkproj(const Datum & D, const Proj & P, const Options & o){

    int old_enum_fmt=Enum::output_fmt;
    Enum::output_fmt = Enum::proj_fmt;

    ostringstream projpar;

    if (P==Proj("google"))  projpar << " +proj=merc";
    else  projpar << " +proj=" << P;

    if (D==Datum("pulkovo"))
       projpar << " +ellps=krass +towgs84=+28,-130,-95";
    else if (D==Datum("sphere"))
       projpar << " +ellps=sphere";
    else projpar << " +datum=" << D;

    Enum::output_fmt = old_enum_fmt;

    switch (P.val){

	case 0: // lonlat
          break;

	case 1: // tmerc
          if (o.count("lon0")==0){
            std::cerr << "mkproj: can't create proj: \""
                      << projpar.str() << "\" without lon0\n";
            exit(1);
          }
          projpar << " +lon_0=" << o.get("lon0", 0.0);
          projpar << " +lat_0=" << o.get("lat0", 0.0);
          projpar << " +k="     << o.get("k",    1.0);
          projpar << " +x_0="   << o.get("E0",   500000.0);
          projpar << " +y_0="   << o.get("N0",   0.0);
          break;

        case 3: // merc
          projpar << " +lon_0="  << o.get("lon0", 0.0);
          projpar << " +lat_ts=" << o.get("lat0", 0.0);
          projpar << " +x_0="    << o.get("E0",   0.0);
          projpar << " +y_0="    << o.get("N0",   0.0);
          break;

	case 4: // google
          break;

        default:
          std::cerr << "unknown proj: " << P.val << "\n";
          exit(1);
    }

    projPJ ret=pj_init_plus(projpar.str().c_str());
    if (!ret){
        std::cerr << "mkproj: Error: can't create proj: \"" << projpar.str() << "\"\n";
        exit(1);
    }
    return ret;
}

void pjconv(const projPJ P1, const projPJ P2, dPoint & p){

  double z=0;
  if (pj_is_latlong(P1)) p*=M_PI/180.0;
  pj_transform(P1, P2, 1, 1, &p.x, &p.y, &z);
  if (pj_is_latlong(P2)) p*=180.0/M_PI;
}

// преобразование геодезических координат
// точки преобразуются по ссылке, чтобы можно было не копируя
// преобразовывать координаты в сложных штуках типа g_waypoint
pt2pt::pt2pt(const Datum & sD, const Proj & sP, const Options & sPo,
             const Datum & dD, const Proj & dP, const Options & dPo){
  pr_src = mkproj(sD, sP, sPo);
  pr_dst = mkproj(dD, dP, dPo);
  refcounter   = new int;
  *refcounter  = 1;
}

///
pt2pt::pt2pt(const pt2pt & other){
  copy(other);
}
pt2pt & pt2pt::operator=(const pt2pt & other){
  if (this != &other){
    destroy();
    copy(other);
  }
  return *this;
}
pt2pt::~pt2pt(){
  destroy();
}
void pt2pt::copy(const pt2pt & other){
   refcounter = other.refcounter;
   pr_src = other.pr_src;
   pr_dst = other.pr_dst;
   (*refcounter)++;
   assert(*refcounter >0);
}
void pt2pt::destroy(void){
  (*refcounter)--;
  if (*refcounter<=0){
    delete refcounter;
    pj_free(pr_src);
    pj_free(pr_dst);
  }
}
///

void pt2pt::frw(dPoint & p) const{
  pjconv(pr_src, pr_dst, p);
}

void pt2pt::bck(dPoint & p) const{
  pjconv(pr_dst, pr_src, p);
}


/*******************************************************************/
/* Приведение матрицы (N+1)xN к диагональному виду */
#define AN(x,y) a[(x)+(y)*(N+1)]

int mdiag(int N, double *a){
  int i,j,k,l;
  double tmp;

  for (k=0; k<N; ++k){  /* идем по строчкам сверху вниз */
    /* циклически переставляем строчки от k до N-1, чтобы на месте (k,k) встал не ноль */
    for (i=k; i<N; ++i){ if (AN(k,k)!=0) break;
      for (j=0;j<N+1;++j){ /* по всем столбцам */
        tmp=AN(j,k);        /* сохраним верхнее */
        for (l=k; l<N-1; ++l) AN(j,l)=AN(j,l+1); /* сдвинем */
        AN(j,N-1)=tmp; /* бывшее верхнее -- вниз */
      }
    }
    /* если так сделать нельзя, уравнения зависимы */
    if (AN(k,k)==0) return 1;
    /* делим строчку на A(k,k) (от N до k, т.к. в начале -- нули)*/
    for (j=N; j>=k; --j) AN(j,k)=AN(j,k)/AN(k,k);
    /* вычитаем из всех остальных строчек эту, помноженную на A(k,*) */
    for (i=0; i<N; ++i) if (i!=k) for (j=N; j>=k; --j) AN(j,i)-=AN(k,i)*AN(j,k);
  }
  return 0;
}

// autodetect map projection options (lon0) if needed
Options map_popts(const g_map & M, Options O){
  switch (M.map_proj.val){
  case 0: break; //lonlat
  case 1:        //tmerc
    if (O.count("lon0")==0){
      O.put("lon0", lon2lon0(M.center().x));
      O.put("E0",   500000.0);
      O.put("N0",   0.0);
    }
    break;
  case 2:        //UTM
    std::cerr << "utm map is not supported. fixme!\n";
    break;
  case 3:        // merc
    break;
  case 4:        // google
    break;
  case 5:        // ks
    break;
  case 6:        // cea
    break;
  default:
    std::cerr << "unknown map proj: " << M.map_proj << "\n";
    break;
  }
  return O;
}

/*******************************************************************/
// преобразование из точки карты в геодезическую точку
// здесь же - выяснение всяких параметров карты (размер изображения, масштам метров/точку)
// сюда же - преобразование линий!

// Считается, что преобразование СК и замена осевого меридиана - линейны в пределах карты.
// Проекция карты берется из sM. Системы координат и параметры проекции
// (вроде lon0) берутся из dD и dPo.
//
// Практичекое следствие: если мы хотим работать с к-л определенным осевым
// меридианом - мы должны запихать его в dPo! (даже, если мы преобразуем из 
// карты с lon0 в lonlat!)
// преобразовать из карты с одним ос.м. в проекцию с другим - нельзя 
// (у нас же все линейно :)), надо использовать два последовательных преобразования...

map2pt::map2pt(const g_map & sM,
               const Datum & dD, const Proj & dP, const Options & dPo){

// у нас точки привязки в lon-lat wgs84, карта нарисована в некоторой другой проекции
// а получить мы хотим третью проекцию.
// При этом в какой СК нарисована карта и какие параметры проекции
// используются - нам не важно - это станет частью лин.преобразования!

    // projection for reference points
    pr_ref = mkproj(Datum("WGS84"), Proj("lonlat"), Options()); // for ref points

    // destination projection
    pr_dst = mkproj(dD, dP, dPo);

    // "map" projection
    if (sM.map_proj == dP)
      pr_map = pr_dst;
    else
      pr_map = mkproj(dD, sM.map_proj, map_popts(sM));

    refcounter   = new int;
    *refcounter  = 1;

    border = sM.border;

  // Разберемся с границей. Она нужна нам для всяких рисований и т.п. 
  // Но модифицировать карту мы не хотим - так что работаем со своей копией.
  // Граница должна или быть пустой или содержать больше двух точек.
  // если она пустая - можно попробовать определить ее по граф.файлу
  if ((border.size()>0)&&(border.size()<3)){
    cerr << "map2pt: one or two points in border of map "
         << sM.comm << " (" << sM.file << ")\n";
    border.clear();
  }
  if (border.size()==0) {
    iPoint wh = image_r::size(sM.file.c_str());
    border.push_back(dPoint(0, 0));
    border.push_back(dPoint(wh.x, 0));
    border.push_back(dPoint(wh.x, wh.y));
    border.push_back(dPoint(0, wh.y));
  }

  // А теперь следите за руками...

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

#define A7(x,y) a[(x)+(y)*7]
  double a[6*7];
  memset(a, 0, sizeof(a));

  for (unsigned i = 0; i < points.size(); i++){

    double x = points[i].xr;
    double y = points[i].yr;
    dPoint lp = points[i];

    pjconv(pr_ref, pr_map, lp);

    A7(0,0)+=x*x; A7(3,3)+=x*x;
    A7(1,0)+=x*y; A7(4,3)+=x*y;
    A7(2,0)+=x;   A7(5,3)+=x;
    A7(0,1)+=x*y; A7(3,4)+=x*y;
    A7(1,1)+=y*y; A7(4,4)+=y*y;
    A7(2,1)+=y;   A7(5,4)+=y;
    A7(0,2)+=x; A7(3,5)+=x;
    A7(1,2)+=y; A7(4,5)+=y;
    A7(2,2)+=1; A7(5,5)+=1;

    A7(6,0)+=lp.x*x; A7(6,1)+=lp.x*y; A7(6,2)+=lp.x;
    A7(6,3)+=lp.y*x; A7(6,4)+=lp.y*y; A7(6,5)+=lp.y;
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


///
map2pt::map2pt(const map2pt & other){
  copy(other);
}
map2pt & map2pt::operator=(const map2pt & other){
  if (this != &other){
    destroy();
    copy(other);
  }
  return *this;
}
map2pt::~map2pt(){
  destroy();
}
void map2pt::copy(const map2pt & other){
   refcounter = other.refcounter;
   pr_ref = other.pr_ref;
   pr_map= other.pr_map;
   pr_dst = other.pr_dst;
   memcpy(k_map2geo, other.k_map2geo, sizeof(k_map2geo));
   memcpy(k_geo2map, other.k_geo2map, sizeof(k_geo2map));
   border=other.border;
   border_geo=other.border_geo;
   (*refcounter)++;
   assert(*refcounter >0);
}
void map2pt::destroy(void){
  (*refcounter)--;
  if (*refcounter<=0){
    delete refcounter;
    pj_free(pr_ref);
    if (pr_map != pr_dst) pj_free(pr_map);
    pj_free(pr_dst);
  }
}
///


void map2pt::frw(dPoint & p) const{
  // do linear transformation
  double x = k_map2geo[0]*p.x + k_map2geo[1]*p.y + k_map2geo[2];
  double y = k_map2geo[3]*p.x + k_map2geo[4]*p.y + k_map2geo[5];
  p.x=x;
  p.y=y;
  if (pr_map!=pr_dst) pjconv(pr_map, pr_dst, p);
  return;
}

void map2pt::bck(dPoint & p) const{
  if (pr_map!=pr_dst) pjconv(pr_dst, pr_map, p);
  // do linear transformation
  double x = k_geo2map[0]*p.x + k_geo2map[1]*p.y + k_geo2map[2];
  double y = k_geo2map[3]*p.x + k_geo2map[4]*p.y + k_geo2map[5];
  p.x=x;
  p.y=y;
  return;
}


/*******************************************************************/
// преобразование из карты в карту
// здесь может быть суровое разбиение карты на куски и аппроксимация линейными преобразованиями...
// здесь же - преобразование линий
// здесь же - преобразование картинок (с интерфейсом как у image loader'a)

map2map::map2map(const g_map & sM, const g_map & dM, bool test_brd_) :
    c1(sM, Datum("wgs84"), dM.map_proj, map_popts(dM)),
    c2(dM, Datum("wgs84"), dM.map_proj, map_popts(dM)),
    tst_frw(c1.border),
    tst_bck(c1.border),
    test_brd(test_brd_)
{

  if (sM.map_proj == Proj("google")){
    c1=map2pt(sM, Datum("sphere"), Proj("lonlat"), Options());
    c2=map2pt(dM, Datum("wgs84"), Proj("lonlat"), Options());
  }

  border_src = c1.border;
  tst_bck = border_tester(border_src);
  if (test_brd){
    border_dst = line_frw(c1.border);
    tst_frw = border_tester(border_dst);
  }
}

void map2map::frw(dPoint & p) const {c1.frw(p); c2.bck(p);}
void map2map::bck(dPoint & p) const {c2.frw(p); c1.bck(p);}


// ****************

int map2map::image_frw(iImage & src_img, int src_scale, iRect cnv_rect,
                       iImage & dst_img, iRect dst_rect) const{

    if (cnv_rect.empty() || dst_rect.empty()) return 1;
    // во сколько раз придется растягивать картинку
    int xscale = int(floor(dst_rect.w/cnv_rect.w));
    int yscale = int(floor(dst_rect.h/cnv_rect.h));
    if (xscale<1) xscale=1;
    if (yscale<1) yscale=1;

    double x,y;
    for (int dst_y = dst_rect.y; dst_y<dst_rect.y+dst_rect.h; dst_y+=yscale){
      // откуда мы хотим взять строчку
      y = cnv_rect.y + ((dst_y-dst_rect.y)*cnv_rect.h)/dst_rect.h;

      for (int dst_x = dst_rect.x; dst_x<dst_rect.x+dst_rect.w; dst_x+=xscale){
        x = cnv_rect.x + ((dst_x-dst_rect.x)*cnv_rect.w)/dst_rect.w;
	dPoint p(x,y);
        bck(p);
        if (test_brd && !tst_bck.test(int(p.x), int(p.y))) continue;
	p/=src_scale;
	unsigned int c = src_img.safe_get(int(p.x),int(p.y));
	if (c != 0){
	  for (int jj=0;jj<yscale;jj++){
	    for (int ii=0;ii<xscale;ii++){
   	      dst_img.set_na(dst_x+ii, dst_y+jj, c);
            }
          }
	}
      }
    }
    return 0;
}

int map2map::image_bck(iImage & src_img, int src_scale, iRect cnv_rect, 
                       iImage & dst_img, iRect dst_rect) const{
    cerr << "WARNING(ushakov): I suspect image_bck is broken" << endl;
    if (cnv_rect.empty() || dst_rect.empty()) return 1;
    // во сколько раз придется растягивать картинку
    int xscale = int(floor(dst_rect.w/cnv_rect.w));
    int yscale = int(floor(dst_rect.h/cnv_rect.h));
    if (xscale<1) xscale=1;
    if (yscale<1) yscale=1;

    double x,y;
    for (int dst_y = dst_rect.y; dst_y<dst_rect.y+dst_rect.h; dst_y++){
      // откуда мы хотим взять строчку
      y = cnv_rect.y + ((dst_y-dst_rect.y)*cnv_rect.h)/dst_rect.h;
      for (int dst_x = dst_rect.x; dst_x<dst_rect.x+dst_rect.w; dst_x++){
        x = cnv_rect.x + ((dst_x-dst_rect.x)*cnv_rect.w)/dst_rect.w;
        dPoint p(x,y);
        if (test_brd && !tst_bck.test(int(p.x), int(p.y))) continue;
        bck(p); p/=src_scale;
	unsigned int c = src_img.safe_get(int(p.x),int(p.y));
	if (c != 0){
	  for (int jj=0;jj<yscale;jj++){
	    for (int ii=0;ii<xscale;ii++){
   	      dst_img.set_na(dst_x+ii, dst_y+jj, c);
            }
          }
        }
      }
    }
    return 0;
}

iRect map2map::bb_frw_i(const iRect & R) const{
  dLine l = line_frw(rect2line(R));
  dRect r = l.range();
  return iRect(
    iPoint(int(floor(r.TLC().x)), int(floor(r.TLC().y))),
    iPoint(int(ceil(r.BRC().x)), int(ceil(r.BRC().y)))
  );
}

iRect map2map::bb_bck_i(const iRect & R) const{
  dLine l = line_bck(rect2line(R));
  dRect r = l.range();
  return iRect(
    iPoint(int(floor(r.TLC().x)), int(floor(r.TLC().y))),
    iPoint(int(ceil(r.BRC().x)), int(ceil(r.BRC().y)))
  );
}

// Быстрая проверка границ
  border_tester::border_tester(dLine & brd) : border(brd){
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
  bool border_tester::test_range(iRect range) const{
    int lx = 0; int ly=0;
    int rx = 0; int ry=0;
    iPoint p1 = range.TLC();
    iPoint p2 = range.BRC();
    dLine::const_iterator p;
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

double lon2lon0(const double lon){
  return floor( lon/6.0 ) * 6 + 3;
}
double lon_pref2lon0(const double lon){
  return floor( lon/1e6 ) * 6 - 3;
}
double lon_delprefix(const double lon){
  return lon - floor( lon/1e6 ) * 1e6;
}

g_map mymap(const geo_data & world){ // естественная привязка геоданных
    // тип проекции -- по первой карте, или lonlat, если карт нет
    // осевой меридиан -- 6n+3, наиболее близкий к середине диапазона треков и точек,
    // а если их нет - к середине диапазона карт

    g_map ret;
    Options O;
    if (world.maps.size()>0){
      ret.map_proj=world.maps[0].map_proj;
      O=map_popts(world.maps[0]);
    } else ret.map_proj=Proj("lonlat");

    dRect rd=world.range_geodata();
    dRect rm=world.range_map();
    double lon0 = rm.x+rm.w/2;
    if (!rd.empty()) lon0=rd.x+rd.w/2;
    O.put("lon0", lon2lon0(lon0)); // todo - use map_popts here?

    // масштаб -- соответствующий минимальному масштабу карт, если они есть,
    // или 1/3600 градуса на точку, если карт нет
    double mpp=1e99;
    for (int i=0;i<world.maps.size();i++){ 
      double tmp=map_mpp(world.maps[i], world.maps[i].map_proj);
      if (mpp>tmp) mpp=tmp;
    }
    if ((mpp>1e90)||(mpp<1e-90)) mpp=1/3600.0;


    // точки привязки
    pt2pt cnv(Datum("WGS84"), ret.map_proj, O, Datum("WGS84"), Proj("lonlat"), O);
    dPoint p(lon0,0); cnv.bck(p);
    dPoint p1=p+dPoint(mpp*1000,0); cnv.frw(p1);
    dPoint p2=p+dPoint(0,mpp*1000); cnv.frw(p2);
    cnv.frw(p);

    ret.push_back(g_refpoint(p.x,p.y,   0,1000));
    ret.push_back(g_refpoint(p1.x,p1.y, 1000,1000));
    ret.push_back(g_refpoint(p2.x,p2.y, 0,0));
    // чтоб не пытались определять границы из файла
    for (int i=0;i<3;i++)
      ret.border.push_back(dPoint(0,0));
    return ret;
}

double map_mpp(const g_map &map, Proj P){
  if (map.size()<3) return 0;
  double l1=0, l2=0;
  g_map map1=map; map1.map_proj=P;
  convs::pt2pt c(Datum("wgs84"), P, map_popts(map1), Datum("wgs84"), Proj("lonlat"), Options());
  for (int i=1; i<map.size();i++){
    dPoint p1(map[i-1].x,map[i-1].y);
    dPoint p2(map[i].x,  map[i].y);
    c.bck(p1); c.bck(p2);
    l1+=pdist(p1,p2);
    l2+=pdist(dPoint(map[i].xr, map[i].yr), dPoint(map[i-1].xr, map[i-1].yr));
  }
  if (l2==0) return 0;
  return l1/l2;
}


}//namespace


