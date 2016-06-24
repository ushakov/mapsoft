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

srtm3::srtm3(const string & _srtm_dir, const unsigned cache_size) :
     srtm_cache(cache_size),
     srtm_dir(_srtm_dir),
     size0(6380e3 * M_PI/srtm_width/180),
     area0(size0*size0){
  if (srtm_dir == "") srtm_dir =
    string(getenv("HOME")? getenv("HOME"):"") + "/.srtm_data";
}

void
srtm3::set_dir(const string & str){
  srtm_dir = str;
}

const string &
srtm3::get_dir(void) const{
  return srtm_dir;
}


short
srtm3::geth(const iPoint & p, const bool interp){
  iPoint key = p/(srtm_width-1);
  iPoint crd = p - key*(srtm_width-1);

  if (key.x<0) {key.x--; crd.x+=srtm_width;}
  if (key.y<0) {key.y--; crd.y+=srtm_width;}
  crd.y=srtm_width-crd.y-1;


  int h;
  {
    Glib::Mutex::Lock lock(mutex);
    if ((!srtm_cache.contains(key)) && (!load(key))) return srtm_nofile;
    if (srtm_cache.get(key).empty()) return srtm_nofile;
    h = srtm_cache.get(key).get(crd);
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
srtm3::slope(const iPoint &p, const bool interp){
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
srtm3::geth(const int x, const int y, const bool interp){
  return geth(iPoint(x,y), interp);
}

double
srtm3::slope(const int x, const int y, const bool interp){
  return slope(iPoint(x,y), interp);
}

short
srtm3::seth(const iPoint & p, const short h){
  iPoint key = p/(srtm_width-1);
  iPoint crd = p - key*(srtm_width-1);

  if (key.x<0) {key.x--; crd.x+=srtm_width;}
  if (key.y<0) {key.y--; crd.y+=srtm_width;}
  crd.y=srtm_width-crd.y-1;

  {
    Glib::Mutex::Lock lock(mutex);
    if ((!srtm_cache.contains(key)) && (!load(key))) return srtm_nofile;
    if (srtm_cache.get(key).empty()) return srtm_nofile;
    srtm_cache.get(key).set(crd, h);
  }
  return h;
}

short
srtm3::geth4(const dPoint & p, const bool interp){
  double x = p.x*1200;
  double y = p.y*1200;
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
srtm3::slope4(const dPoint & p, const bool interp){
  double x = p.x*1200;
  double y = p.y*1200;
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
srtm3::geth16(const dPoint & p, const bool interp){

  double x = p.x*1200;
  double y = p.y*1200;
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
srtm3::plane(const iPoint& p, int max){
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
srtm3::move_to_extr(iPoint & p0, bool max){
  iPoint p1 = p0;
  do {
    short h = geth(p0, true);
    for (int i=0; i<8; i++){
      iPoint p=adjacent(p0,i);
      if ((max && (geth(p,true) > geth(p1,true))) ||
         (!max && (geth(p,true) < geth(p1,true)))) p1=p;
    }
    if (p1==p0) break;
    p0=p1;
  } while (1);
}

void
srtm3::move_to_min(iPoint & p0){
  move_to_extr(p0, false);
}
void
srtm3::move_to_max(iPoint & p0){
  move_to_extr(p0, true);
}
double
srtm3::area(const iPoint &p) const{
  return area0 * cos((double)p.y *M_PI/180.0/srtm_width);
}


/**********************************************************/

sImage
read_zfile(const string & file){
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
read_file(const string & file){
  FILE *F = fopen(file.c_str(), "rb");
  if (!F) return sImage(0,0);

  sImage im(srtm_width,srtm_width);
  int length = srtm_width*srtm_width*sizeof(short);

  if (length != fread(im.data, 1, length, F)){
    cerr << "bad file: " << file << '\n';
    return sImage(0,0);
  }
  for (int i=0; i<length/2; i++){ // swap bytes
    int tmp=(unsigned short)im.data[i];
    im.data[i] = (tmp >> 8) + (tmp << 8);
  }
  fclose(F);
  return im;
}

bool
srtm3::load(const iPoint & key){

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

  im = read_zfile(srtm_dir + "/fixed/" + file.str() + ".gz");
  if (!im.empty()) goto read_ok;
  im = read_file(srtm_dir + "/fixed/" + file.str());
  if (!im.empty()) goto read_ok;

  im = read_zfile(srtm_dir + "/" + file.str() + ".gz");
  if (!im.empty()) goto read_ok;
  im = read_file(srtm_dir + "/" + file.str());
  if (!im.empty()) goto read_ok;

  cerr << "can't find file " << file.str() << '\n';
  read_ok:

  srtm_cache.add(key, im);
  return !im.empty();
}



// see http://www.paulinternet.nl/?page=bicubic
short
srtm3::cubic_interp(const double h[4], const double x) const{
  return h[1] + 0.5 * x*(h[2] - h[0] + x*(2.0*h[0] - 5.0*h[1] + 4.0*h[2] -
              h[3] + x*(3.0*(h[1] - h[2]) + h[3] - h[0])));
}

void
srtm3::int_holes(double h[4]) const{
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
srtm3::find_contours(const dRect & range, int step){
  int lon1  = int(floor(1200*range.TLC().x));
  int lon2  = int( ceil(1200*range.BRC().x));
  int lat1  = int(floor(1200*range.TLC().y));
  int lat2  = int( ceil(1200*range.BRC().y));

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
      double x1,x2;

      for (multimap<short,double>::const_iterator i=pts.begin(); i!=pts.end(); i++){
        if (h!=i->first){
          h  = i->first;
          x1 = i->second;
        } else{
          x2 = i->second;
          dPoint p1=(dPoint(p + crn(int(x1))) + dPoint(dir(int(x1)))*double(x1-int(x1)))/1200.0;
          dPoint p2=(dPoint(p + crn(int(x2))) + dPoint(dir(int(x2)))*double(x2-int(x2)))/1200.0;
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

map<dPoint, short>
srtm3::find_peaks(const dRect & range, int DH, int PS){

  int lon1  = int(floor(1200*range.TLC().x));
  int lon2  = int( ceil(1200*range.BRC().x));
  int lat1  = int(floor(1200*range.TLC().y));
  int lat2  = int( ceil(1200*range.BRC().y));

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
          ret[dPoint(p)/1200.0] = h;
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
srtm3::find_holes(const dRect & range){

  int lon1  = int(floor(1200*range.TLC().x));
  int lon2  = int( ceil(1200*range.BRC().x));
  int lat1  = int(floor(1200*range.TLC().y));
  int lat2  = int( ceil(1200*range.BRC().y));

  set<iPoint> aset;
  for (int lat=lat2; lat>lat1; lat--){
    for (int lon=lon1; lon<lon2-1; lon++){
      iPoint p(lon,lat);
      short h = geth(p);
      dPoint p1 = dPoint(p)/1200.0;
      if (h!=srtm_undef) continue;
      aset.insert(p);
    }
  }
  // converting points to polygons
  return pset2line(aset)/1200.0;
}

/*
  // поиск крутых склонов
  cerr << "ищем крутые склоны: ";
  double latdeg = 6380000/1200.0/180.0*M_PI; 
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
    dLine l = (*iv)/1200.0;
    mp::mp_object mpo;
    mpo.Class = "POLYGON";
    mpo.Label = "high slope";
    mpo.Type = 0x19;
    mpo.insert(mpo.end(), l.begin(), l.end());
    MP.push_back(mpo);
  }
  cerr << aline.size() << " шт\n";
*/

