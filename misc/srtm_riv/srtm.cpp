#include <string>
#include <vector>
#include <set>
#include <queue>

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <stdexcept>
#include <cmath>

#include "srtm.h"


srtm::srtm(const std::string srtm_dir, const unsigned cache_size, const unsigned mode){

  if (cache_size < 1) throw std::domain_error("Wrong cache size!\n");

  world = std::vector<file *>(2*max_lat * 2*max_lon, (file *)0);
  cache = std::vector<file>(cache_size, file());

  new_file = cache.begin();  
  dir = srtm_dir;

  if (mode == mode_interp) interp = true;
  else interp = false;
}

inline unsigned nxny2i(int nx, int ny) {return nx+max_lon + (ny+max_lat)*2*max_lon;} 
inline unsigned xy2i(int x, int y) {return x + (srtm_width-y)*(srtm_width+1);} 



bool srtm::load(short *d, int nx, int ny){
  char NS='N';
  char EW='E';
  if (ny<0) {NS='S'; ny=-ny;}
  if (nx<0) {EW='W'; nx=-nx;}

  std::ostringstream file;
  file << dir << NS << std::setfill('0') << std::setw(2) << ny 
              << EW << std::setw(3) << nx << ".hgt";

  std::ifstream in(file.str().c_str());
  if (!in) return false;
  char c1, c2;
  int i;
  for (i=0;i<(srtm_width+1)*(srtm_width+1); i++){
    if (in.get(c1) && in.get(c2))
      d[i] = (short)(unsigned char)c2 + ((short)(unsigned char)c1 <<8);
    else throw std::domain_error("Read error!\n");
  }
  return true;
}

// Узнать высоту точки, без интерполяции дыр
int srtm::geth_(int lat, int lon){ 
  int nx = lon/srtm_width;
  int ny = lat/srtm_width;
  int x  = lon%srtm_width;
  int y  = lat%srtm_width;

  while (nx < -max_lon) nx+=2*max_lon;
  while (nx >= max_lon) nx-=2*max_lon;
  if ((ny<-max_lat)||(ny>=max_lat)) return srtm_nofile;

  if(world[nxny2i(nx,ny)]==NULL){
    if (!load(new_file->data, nx,ny)){
      std::cerr << "can't find file " << ny << " " << nx << '\n';
      world[nxny2i(nx,ny)] = (file*)&empty;
      return srtm_nofile;
    }

//    std::cerr << '-' << new_file->nx << ' ' << new_file->ny << '\n';
//    std::cerr << '-' << nx << ' ' << ny << '\n';
    world[nxny2i(new_file->nx, new_file->ny)] = NULL;
    new_file->nx = nx; new_file->ny = ny;
    
    world[nxny2i(nx,ny)] = & (*new_file);
    new_file++;
    if (new_file == cache.end()) new_file = cache.begin();
  }
  if(world[nxny2i(nx,ny)]==(file*)&empty){return srtm_nofile;}

  return (world[nxny2i(nx,ny)] -> data[xy2i(x,y)]);
}


// Поменять высоту точки. Данные теряются при выкидывании кусочка из кэша
int srtm::seth_(int lat, int lon, short h){ 

  int h0 = geth_(lat,lon);
  if (h0==srtm_nofile) return h0;
  
  int nx = lon/srtm_width;
  int ny = lat/srtm_width;
  int x  = lon%srtm_width;
  int y  = lat%srtm_width;

  return (world[nxny2i(nx,ny)] -> data[xy2i(x,y)] = h);
}



// Узнать высоту точки. Дыры интерполируются, для них возвращается
// высота+srtm_zer_interp
int srtm::geth(int lat, int lon){ 

  int h = geth_(lat,lon);
  // интерполяция не требуется:
  if ((h!=srtm_undef)||(!interp)) return h;

  // найдем всю дырку и заинтерполируем ее!
  std::set<point> pset = plane(point(lat,lon));
  std::set<point> bord = border(pset);

  for (std::set<point>::iterator p  = pset.begin();
                                 p != pset.end(); p++){
    double Srh = 0;
    double Sr  = 0;
    for (std::set<point>::iterator b  = bord.begin();
                                   b != bord.end(); b++){
      int bh = geth_(b->x, b->y);

      if (bh>srtm_min){
        if (bh>srtm_min_interp) bh-=srtm_zer_interp;
        double k = cos(double(p->y)/srtm_width/180.0*M_PI);
        double r = 1.0/(pow(k*(b->x - p->x),2) + pow(b->y - p->y,2));
        Sr += r;
        Srh+= bh * r; 
      }
    }
    seth_(p->x, p->y,(short)(Srh/Sr)+srtm_zer_interp);
  }
  return geth_(lat,lon);

}


// найти множество соседних точек одной высоты
std::set<point> srtm::plane(const point& p){
  std::set<point> ret;
  std::queue<point> q;
  short h = geth_(p.x,p.y);

  q.push(p);
  ret.insert(p);

  while (!q.empty()){
    point p1 = q.front();
    q.pop();
    for (int i=0; i<point_adjs; i++){
      point p2 = p1.adjacent(i);
      if ((geth_(p2.x, p2.y) == h)&&(ret.insert(p2).second)) q.push(p2);
    }
  }
  return ret;
}
