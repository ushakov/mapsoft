#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <queue>

#include <cstdio>

#include <2d/point_int.h>
#include <2d/line_utils.h>
#include "srtm3.h"
#include <zlib.h>

using namespace std;

SRTM3::SRTM3(const string & _srtm_dir, const unsigned cache_size):
     srtm_dir(_srtm_dir), srtm_cache(cache_size) {
  set_dir(srtm_dir);
}

void
SRTM3::set_dir(const string & str){
  srtm_dir = str;
  if (srtm_dir == "") srtm_dir =
    string(getenv("HOME")? getenv("HOME"):"") + "/.srtm_data";

  ifstream ws(srtm_dir + "/srtm_width.txt");
  if (!ws) srtm_width = 1201;
  else ws >> srtm_width;
  if (srtm_width<1) srtm_width = 1201;

  size0 = 6380e3 * M_PI/srtm_width/180;
  area0 = pow(6380e3 * M_PI/srtm_width/180, 2);
  Glib::Mutex::Lock lock(mutex);
  srtm_cache.clear();
}

const string &
SRTM3::get_dir(void) const{
  return srtm_dir;
}

const unsigned
SRTM3::get_width(void) const{
  return srtm_width;
}

// find tile number and coordinate on the tile
void
get_crd(int x, int w, int &k, int &c){
  if (x>=0) k=x/(w-1);
  else      k=(x+1)/(w-1)-1;
  c = x - k*(w-1);
}

short
SRTM3::geth(const iPoint & p, const bool interp){

  // find tile number and coordinate on the tile
  iPoint key, crd;
  get_crd(p.x, srtm_width, key.x, crd.x);
  get_crd(p.y, srtm_width, key.y, crd.y);
  crd.y = srtm_width-crd.y-1;


  int h;
  {
    Glib::Mutex::Lock lock(mutex);
    if ((!srtm_cache.contains(key)) && (!load(key))) return srtm_nofile;
    if (srtm_cache.get(key).empty()) return srtm_nofile;
    h = srtm_cache.get(key).safe_get(crd);
  }

  if (interp){
    if (h>srtm_min_interp) return h - srtm_zer_interp;
    if (h!=srtm_undef) return h;

    // найдем всю дырку и заинтерполируем ее!
    set<iPoint> pset = plane(p);
    set<iPoint> bord = border(pset);

    set<iPoint>::iterator pi, bi;
    for (pi = pset.begin(); pi != pset.end(); pi++){
      double Srh = 0;
      double Sr  = 0;
      for (bi = bord.begin(); bi != bord.end(); bi++){
        int bh = geth(*bi);

        if (bh>srtm_min){
          double k = cos(double(pi->y)/srtm_width/180.0*M_PI);
          double r = 1.0/(pow(k*(bi->x - pi->x),2) + pow(double(bi->y - pi->y),2));
          Sr += r;
          Srh+= bh * r;
        }
      }
      seth(*pi, (short)Srh/Sr+srtm_zer_interp);
    }
    return geth(p,true);
  }
  else {
    if (h>srtm_min_interp) return srtm_undef;
    else return h;
  }

  return h;
}

double
SRTM3::slope(const iPoint &p, const bool interp){
  short h  = geth(p, interp);
  short h1 = geth(p.x-1, p.y, interp);
  short h2 = geth(p.x+1, p.y, interp);
  if (h1 < srtm_min && h > srtm_min && h2 > srtm_min) h1 = 2*h - h2;
  if (h2 < srtm_min && h > srtm_min && h1 > srtm_min) h1 = 2*h - h2;

  short h3 = geth(p.x, p.y-1, interp);
  short h4 = geth(p.x, p.y+1, interp);
  if (h3 < srtm_min && h > srtm_min && h4 > srtm_min) h3 = 2*h - h4;
  if (h4 < srtm_min && h > srtm_min && h3 > srtm_min) h4 = 2*h - h3;

  if (h1 > srtm_min && h2 > srtm_min && h3 > srtm_min && h4 > srtm_min){
    const double kx =  cos(M_PI*p.y/180/srtm_width);
    const double  U = hypot((h2-h1)/kx, h4-h3)/size0/2.0;
    return atan(U)*180.0/M_PI;
  }
  return 0;
}

short
SRTM3::geth(const int x, const int y, const bool interp){
  return geth(iPoint(x,y), interp);
}

double
SRTM3::slope(const int x, const int y, const bool interp){
  return slope(iPoint(x,y), interp);
}

short
SRTM3::seth(const iPoint & p, const short h){

  // find tile number and coordinate on the tile
  iPoint key, crd;
  get_crd(p.x, srtm_width, key.x, crd.x);
  get_crd(p.y, srtm_width, key.y, crd.y);
  crd.y = srtm_width-crd.y-1;

  {
    Glib::Mutex::Lock lock(mutex);
    if ((!srtm_cache.contains(key)) && (!load(key))) return srtm_nofile;
    if (srtm_cache.get(key).empty()) return srtm_nofile;
    srtm_cache.get(key).safe_set(crd, h);
  }
  return h;
}

short
SRTM3::geth4(const dPoint & p, const bool interp){
  double x = p.x*(srtm_width-1);
  double y = p.y*(srtm_width-1);
  int x1 = floor(x), x2 = x1+1;
  int y1 = floor(y), y2 = y1+1;

  short h1=geth(x1,y1,interp);
  short h2=geth(x1,y2,interp);

  if ((h1<srtm_min)||(h2<srtm_min)) return srtm_undef;
  short h12 = (int)( h1+ (h2-h1)*(y-y1) );

  short h3=geth(x2,y1,interp);
  short h4=geth(x2,y2,interp);
  if ((h3<srtm_min)||(h4<srtm_min)) return srtm_undef;
  short h34 = (int)( h3 + (h4-h3)*(y-y1) );

  return (short)( h12 + (h34-h12)*(x-x1) );
}

double
SRTM3::slope4(const dPoint & p, const bool interp){
  double x = p.x*(srtm_width-1);
  double y = p.y*(srtm_width-1);
  int x1 = floor(x), x2 = x1+1;
  int y1 = floor(y), y2 = y1+1;

  double h1=slope(x1,y1, interp);
  double h2=slope(x1,y2, interp);
  double h3=slope(x2,y1, interp);
  double h4=slope(x2,y2, interp);

  double h12 = h1+ (h2-h1)*(y-y1);
  double h34 = h3 + (h4-h3)*(y-y1);
  return h12 + (h34-h12)*(x-x1);
}

short
SRTM3::geth16(const dPoint & p, const bool interp){

  double x = p.x*(srtm_width-1);
  double y = p.y*(srtm_width-1);
  int x0 = floor(x);
  int y0 = floor(y);

  double hx[4], hy[4];

  for (int i=0; i<4; i++){
    for (int j=0; j<4; j++) hx[j]=geth(x0+j-1, y0+i-1, interp);
    int_holes(hx);
    hy[i]= cubic_interp(hx, x-x0);
  }
  int_holes(hy);
  return cubic_interp(hy, y-y0);
}

// найти множество соседних точек одной высоты (не более max точек)
set<iPoint>
SRTM3::plane(const iPoint& p, size_t max){
  set<iPoint> ret;
  queue<iPoint> q;
  short h = geth(p);

  q.push(p);
  ret.insert(p);

  while (!q.empty()){
    iPoint p1 = q.front();
    q.pop();
    for (int i=0; i<8; i++){
      iPoint p2 = adjacent(p1, i);
      if ((geth(p2) == h)&&(ret.insert(p2).second)) q.push(p2);
    }
    if ((max!=0)&&(ret.size()>max)) break;
  }
  return ret;
}

void
SRTM3::move_to_extr(iPoint & p0, bool down, int maxst){
  iPoint p1 = p0;
  int i=0;
  do {
    for (int i=0; i<8; i++){
      iPoint p=adjacent(p0,i);
      if ((down && (geth(p,true) < geth(p1,true))) ||
         (!down && (geth(p,true) > geth(p1,true)))) p1=p;
    }
    if (p1==p0) break;
    p0=p1;
    i++;
  } while (maxst<0 || i<maxst);
}

void
SRTM3::move_to_min(iPoint & p0, int maxst){ move_to_extr(p0, true, maxst); }
void
SRTM3::move_to_max(iPoint & p0, int maxst){ move_to_extr(p0, false, maxst); }
double
SRTM3::area(const iPoint &p) const{
  return area0 * cos((double)p.y *M_PI/180.0/srtm_width);
}


/**********************************************************/

sImage
read_zfile(const string & file, const size_t srtm_width){
  gzFile F = gzopen(file.c_str(), "rb");
  if (!F) return sImage(0,0);

  sImage im(srtm_width,srtm_width);
  int length = srtm_width*srtm_width*sizeof(short);

  if (length != gzread(F, im.data, length)){
    cerr << "bad file: " << file << '\n';
    return sImage(0,0);
  }
  for (int i=0; i<length/2; i++){ // swap bytes
    int tmp=(unsigned short)im.data[i];
    im.data[i] = (tmp >> 8) + (tmp << 8);
  }
  gzclose(F);
  return im;
}

sImage
read_file(const string & file, const size_t srtm_width){
  FILE *F = fopen(file.c_str(), "rb");
  if (!F) return sImage(0,0);

  sImage im(srtm_width,srtm_width);
  size_t length = srtm_width*srtm_width*sizeof(short);

  if (length != fread(im.data, 1, length, F)){
    cerr << "bad file: " << file << '\n';
    return sImage(0,0);
  }
  for (size_t i=0; i<length/2; i++){ // swap bytes
    int tmp=(unsigned short)im.data[i];
    im.data[i] = (tmp >> 8) + (tmp << 8);
  }
  fclose(F);
  return im;
}

bool
SRTM3::load(const iPoint & key){

  if ((key.x < -max_lon) ||
      (key.x >= max_lon) ||
      (key.y < -max_lat) ||
      (key.y >= max_lat)) return false;

  char NS='N';
  char EW='E';
  if (key.y<0) {NS='S';}
  if (key.x<0) {EW='W';}

  ostringstream file;
  file << NS << setfill('0') << setw(2) << abs(key.y)
       << EW << setw(3) << abs(key.x) << ".hgt";

  // try f2.gz, f2, f1.gz, f1
  sImage im;

  im = read_zfile(srtm_dir + "/" + file.str() + ".gz", srtm_width);
  if (!im.empty()) goto read_ok;
  im = read_file(srtm_dir + "/" + file.str(), srtm_width);
  if (!im.empty()) goto read_ok;

  cerr << "can't find file " << file.str() << '\n';
  read_ok:

  srtm_cache.add(key, im);
  return !im.empty();
}



// see http://www.paulinternet.nl/?page=bicubic
short
SRTM3::cubic_interp(const double h[4], const double x) const{
  return h[1] + 0.5 * x*(h[2] - h[0] + x*(2.0*h[0] - 5.0*h[1] + 4.0*h[2] -
              h[3] + x*(3.0*(h[1] - h[2]) + h[3] - h[0])));
}

void
SRTM3::int_holes(double h[4]) const{
  // interpolate 1-point or 2-points holes
  // maybe this can be written smarter...
  if ((h[0]>srtm_min) && (h[1]>srtm_min) && (h[2]>srtm_min) && (h[3]>srtm_min)) return;
  for (int cnt=0; cnt<2; cnt++){
    if      ((h[0]<=srtm_min) && (h[1]>srtm_min) && (h[2]>srtm_min)) h[0]=2*h[1]-h[2];
    else if ((h[1]<=srtm_min) && (h[0]>srtm_min) && (h[2]>srtm_min)) h[1]=(h[0]+h[2])/2;
    else if ((h[1]<=srtm_min) && (h[2]>srtm_min) && (h[3]>srtm_min)) h[1]=2*h[2]-h[3];
    else if ((h[2]<=srtm_min) && (h[1]>srtm_min) && (h[3]>srtm_min)) h[2]=(h[1]+h[3])/2;
    else if ((h[2]<=srtm_min) && (h[0]>srtm_min) && (h[1]>srtm_min)) h[2]=2*h[1]-h[0];
    else if ((h[3]<=srtm_min) && (h[1]>srtm_min) && (h[2]>srtm_min)) h[3]=2*h[2]-h[1];
    else break;
  }
  if ((h[1]<=srtm_min) && (h[2]<=srtm_min) && (h[0]>srtm_min) && (h[3]>srtm_min)){
    h[1]=(2*h[0] + h[3])/3;
    h[2]=(h[0] + 2*h[3])/3;
  }
}

/**********************************************************/

//координаты угла единичного квадрата по его номеру
iPoint crn (int k){ k%=4; return iPoint(k/2, (k%3>0)?1:0); }

//направление следующей за углом стороны (единичный вектор)
iPoint dir (int k){ return crn(k+1)-crn(k); }

map<short, dMultiLine>
SRTM3::find_contours(const dRect & range, int step){
  int lon1  = int(floor((srtm_width-1)*range.TLC().x));
  int lon2  = int( ceil((srtm_width-1)*range.BRC().x));
  int lat1  = int(floor((srtm_width-1)*range.TLC().y));
  int lat2  = int( ceil((srtm_width-1)*range.BRC().y));

  map<short, dMultiLine> ret;
  int count = 0;
  for (int lat=lat2; lat>lat1; lat--){
    for (int lon=lon1; lon<lon2; lon++){

      iPoint p(lon,lat);
      // пересечения четырех сторон клетки с горизонталями:
      // при подсчетах мы опустим все данные на полметра,
      // чтоб не разбирать кучу случаев с попаданием горизонталей в узлы сетки
      multimap<short, double> pts;

      for (int k=0; k<4; k++){
        iPoint p1 = p+crn(k);
        iPoint p2 = p+crn(k+1);
        short h1 = geth(p1);
        short h2 = geth(p2);
        if ((h1<srtm_min) || (h2<srtm_min)) continue;
        int min = (h1<h2)? h1:h2;
        int max = (h1<h2)? h2:h1;
        min = int( floor(double(min)/step)) * step;
        max = int( ceil(double(max)/step))  * step;
        if (h2==h1) continue;
        for (int hh = min; hh<=max; hh+=step){
          double x = double(hh-h1+0.1)/double(h2-h1);
          if ((x<0)||(x>1)) continue;
          pts.insert(pair<short, double>(hh,x+k));
        }
      }

      // найдем, какие горизонтали пересекают квадрат дважды,
      // поместим их в список горизонталей ret
      short h=srtm_undef;
      double x1=0,x2=0;

      for (multimap<short,double>::const_iterator i=pts.begin(); i!=pts.end(); i++){
        if (h!=i->first){
          h  = i->first;
          x1 = i->second;
        } else{
          x2 = i->second;
          dPoint p1=(dPoint(p + crn(int(x1))) + dPoint(dir(int(x1)))*double(x1-int(x1)))/(double)(srtm_width-1);
          dPoint p2=(dPoint(p + crn(int(x2))) + dPoint(dir(int(x2)))*double(x2-int(x2)))/(double)(srtm_width-1);
          // we found segment p1-p2 with height h
          // first try to append it to existing line in ret[h]
          bool done=false;
          for (dMultiLine::iterator l=ret[h].begin(); l!=ret[h].end(); l++){
            int e=l->size()-1;
            if (e<=0) continue; // we have no 1pt lines!
            if (pdist((*l)[0], p1) < 1e-4){ l->insert(l->begin(), p2); done=true; break;}
            if (pdist((*l)[0], p2) < 1e-4){ l->insert(l->begin(), p1); done=true; break;}
            if (pdist((*l)[e], p1) < 1e-4){ l->push_back(p2); done=true; break;}
            if (pdist((*l)[e], p2) < 1e-4){ l->push_back(p1); done=true; break;}
          }
          if (!done){ // insert new line into ret[h]
            dLine hor;
            hor.push_back(p1);
            hor.push_back(p2);
            ret[h].push_back(hor);
          }
          h=srtm_undef;
          count++;
        }
      }
    }
  }

  // merge contours
  for(map<short, dMultiLine>::iterator im = ret.begin(); im!=ret.end(); im++)
    merge(im->second, 1e-4);

  return ret;
}

dMultiLine
SRTM3::find_slope_contour(const dRect & range, double val){
  int lon1  = int(floor((srtm_width-1)*range.TLC().x));
  int lon2  = int( ceil((srtm_width-1)*range.BRC().x));
  int lat1  = int(floor((srtm_width-1)*range.TLC().y));
  int lat2  = int( ceil((srtm_width-1)*range.BRC().y));

  dMultiLine ret;
  for (int lat=lat2+1; lat>=lat1-1; lat--){
    for (int lon=lon1-1; lon<=lon2+1; lon++){

      iPoint p(lon,lat);
      // пересечения четырех сторон клетки с контуром:
      std::vector<double> pts;

      for (int k=0; k<4; k++){
        iPoint p1 = p+crn(k);
        iPoint p2 = p+crn(k+1);
        double h1 = 0, h2 = 0;
        if (p1.y>=lat1 && p1.y<=lat2 && p1.x>=lon1 && p1.x<=lon2) h1 = slope(p1);
        if (p2.y>=lat1 && p2.y<=lat2 && p2.x>=lon1 && p2.x<=lon2) h2 = slope(p2);
        double min = (h1<h2)? h1:h2;
        double max = (h1<h2)? h2:h1;
        if (min < val && max >= val){
          double x = double(val-h1)/double(h2-h1);
          pts.push_back(x+k);
        }
      }

      // нам интересны случаи 2 или 4 пересечений
      for (size_t i=1; i<pts.size(); i+=2){
        double x1 = pts[i-1], x2 = pts[i];
        dPoint p1=(dPoint(p + crn(int(x1))) + dPoint(dir(int(x1)))*double(x1-int(x1)))/(double)(srtm_width-1);
        dPoint p2=(dPoint(p + crn(int(x2))) + dPoint(dir(int(x2)))*double(x2-int(x2)))/(double)(srtm_width-1);
        // first try to append it to existing line in ret[h]
        bool done=false;
        for (auto & l:ret){
          int e=l.size()-1;
          if (e<=0) continue; // we have no 1pt lines!
          if (pdist(l[0], p1) < 1e-4){ l.insert(l.begin(), p2); done=true; break;}
          if (pdist(l[0], p2) < 1e-4){ l.insert(l.begin(), p1); done=true; break;}
          if (pdist(l[e], p1) < 1e-4){ l.push_back(p2); done=true; break;}
          if (pdist(l[e], p2) < 1e-4){ l.push_back(p1); done=true; break;}
        }
        // insert new line into ret
        if (!done){
          dLine hor;
          hor.push_back(p1);
          hor.push_back(p2);
          ret.push_back(hor);
        }
      }

    }
  }

  // merge contours
  merge(ret, 1e-4);
  return ret;
}


map<dPoint, short>
SRTM3::find_peaks(const dRect & range, int DH, size_t PS){

  int lon1  = int(floor((srtm_width-1)*range.TLC().x));
  int lon2  = int( ceil((srtm_width-1)*range.BRC().x));
  int lat1  = int(floor((srtm_width-1)*range.TLC().y));
  int lat2  = int( ceil((srtm_width-1)*range.BRC().y));

  // поиск вершин:
  // 1. найдем все локальные максимумы (не забудем про максимумы из многих точек!)
  // 2. от каждого будем строить множество точек, добавляя наивысшую точку границы
  // 3. если высота последней добаленной точки ниже исходной более чем на DH м,
  //    или если размер множества больше PS точек - процедуру прекращаем, 
  //    объявляем исходную точку вершиной.
  // 4. Если высота последней добавленной точки больше исходной - процедуру
  //    прекращаем

  set<iPoint> done;
  map<dPoint, short> ret;
  for (int lat=lat2; lat>lat1; lat--){
    for (int lon=lon1; lon<lon2-1; lon++){

      iPoint p(lon,lat);
      if (done.find(p)!=done.end()) continue;
      short h = geth(p);
      if (h<srtm_min) continue;

      set<iPoint> pts; pts.insert(p);
      set<iPoint> brd = border(pts);
      // ищем максимум границы

      do{
        short max = srtm_undef;
        iPoint maxpt;
        for (set<iPoint>::const_iterator i = brd.begin(); i!=brd.end(); i++){
          short h1 = geth(*i);
          // исходная точка слишком близка к краю данных
          if ((h1<srtm_min) && (pdist(*i,p)<1.5)) {max = h1; break;}
          if (h1>max) {max = h1; maxpt=*i;}
        }
        if (max < srtm_min) break;

        // если максимум выше исходной точки - выходим.
        if (max > h) { break; }

        // если мы спустились от исходной точки более чем на DH или размер области более PS
        if ((h - max > DH ) || (pts.size() > PS)) {
          ret[dPoint(p)/(double)(srtm_width-1)] = h;
          break;
        }
        add_pb(maxpt, pts, brd);
        done.insert(maxpt);
      } while (true);
    }
  }
  return ret;
}

dMultiLine
SRTM3::find_holes(const dRect & range){

  int lon1  = int(floor((srtm_width-1)*range.TLC().x));
  int lon2  = int( ceil((srtm_width-1)*range.BRC().x));
  int lat1  = int(floor((srtm_width-1)*range.TLC().y));
  int lat2  = int( ceil((srtm_width-1)*range.BRC().y));

  set<iPoint> aset;
  for (int lat=lat2; lat>lat1; lat--){
    for (int lon=lon1; lon<lon2-1; lon++){
      iPoint p(lon,lat);
      short h = geth(p);
      if (h!=srtm_undef) continue;
      aset.insert(p);
    }
  }
  // converting points to polygons
  return pset2line(aset)/(double)(srtm_width-1);
}

/*
  // поиск крутых склонов
  cerr << "ищем крутые склоны: ";
  double latdeg = 6380000/(double)(srtm_width-1)/180.0*M_PI; 
  double londeg = latdeg * cos(double(lat2+lat1)/2400.0/180.0*M_PI);

  for (int lat=lat2; lat>lat1; lat--){
    for (int lon=lon1; lon<lon2-1; lon++){
      iPoint p(lon,lat);
      short h = geth(p);
      short hx = geth(p+iPoint(1,0));
      short hy = geth(p+iPoint(0,1));
      if ((h<srtm_min) || (hx<srtm_min) || (hy<srtm_min)) continue;
      dPoint gr(double(hx-h)/londeg, double(hy-h)/latdeg);
      double a = atan(pdist(gr))*180/M_PI;
      if (a > 45) aset.insert(p);
    }
  }
  cerr << aset.size() << " точек\n";

  cerr << " преобразуем множество точек в многоугольники: ";
  aline = pset2line(aset);
  for(dMultiLine::iterator iv = aline.begin(); iv!=aline.end(); iv++){
    if (iv->size()<3) continue;
    dLine l = (*iv)/(double)(srtm_width-1);
    mp::mp_object mpo;
    mpo.Class = "POLYGON";
    mpo.Label = "high slope";
    mpo.Type = 0x19;
    mpo.insert(mpo.end(), l.begin(), l.end());
    MP.push_back(mpo);
  }
  cerr << aline.size() << " шт\n";
*/

