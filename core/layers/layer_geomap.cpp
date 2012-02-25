#include "layer_geomap.h"
#include <sstream>
#include <fstream>
#include <cmath>

#include "loaders/image_r.h"
#include "loaders/image_i.h"
#include "utils/cairo_wrapper.h"

using namespace std;

#define SHOW_MAP 1
#define SHOW_BRD 2
#define SHOW_REF 4

LayerGeoMap::LayerGeoMap(g_map_list *_data) :
      data(_data),
      image_cache(4),
      mymap(convs::mymap(*_data)){
  make_m2ms();
}

g_map
LayerGeoMap::get_ref() const {
  return mymap;
}

convs::map2pt
LayerGeoMap::get_cnv() const{
  return convs::map2pt(mymap, Datum("wgs84"), Proj("lonlat"));
}


g_map
LayerGeoMap::get_myref() const {
  return convs::mymap(*data);
}

void
LayerGeoMap::set_ref(const g_map & map){
  mymap=map;
  make_m2ms();
}

g_map_list *
LayerGeoMap::get_data() const{
  return data;
}

g_map *
LayerGeoMap::get_map(const int n) const{
  return &(*data)[n];
}

int
LayerGeoMap::find_map(const iPoint & pt) const{
  for (int i=0; i< m2ms.size(); i++){
    if (m2ms[i].tst_frw.test(pt)) return i;
  }
  return -1;
}
int
LayerGeoMap::find_map(const iRect & r) const{
  for (int i=0; i< m2ms.size(); i++){
    if (m2ms[i].tst_frw.test(r)) return i;
  }
  return -1;
}

void
LayerGeoMap::status_set(int mask, bool val, const g_map * m){
  if (!m){
    map<const g_map*, int>::iterator i;
    for (i=status.begin(); i!=status.end(); i++){
      if (val) i->second |= mask;
      else     i->second &= ~mask;
    }
  }
  else{
    if (!status.count(m)) return;
    if (val) status[m] |= mask;
    else     status[m] &= ~mask;
  }
}

void
LayerGeoMap::show_ref(const g_map * m){ status_set(SHOW_REF, true,  m); }
void
LayerGeoMap::hide_ref(const g_map * m){ status_set(SHOW_REF, false, m); }
void
LayerGeoMap::show_brd(const g_map * m){ status_set(SHOW_BRD, true,  m); }
void
LayerGeoMap::hide_brd(const g_map * m){ status_set(SHOW_BRD, false, m); }
void
LayerGeoMap::show_map(const g_map * m){ status_set(SHOW_MAP, true,  m); }
void
LayerGeoMap::hide_map(const g_map * m){ status_set(SHOW_MAP, false, m); }


void
LayerGeoMap::refresh(){
  image_cache.clear();
}

iImage
LayerGeoMap::get_image (iRect src){
  if (rect_intersect(myrange, src).empty()) return iImage(0,0);
  iImage ret(src.w, src.h, 0);
  draw(src.TLC(), ret);
  return ret;
}

void
LayerGeoMap::draw(const iPoint origin, iImage & image){
  iRect src_rect = image.range() + origin;
  dPoint dorigin(origin);

#ifdef DEBUG_LAYER_GEOMAP
  cerr  << "LayerMap: draw " << src_rect << " my: " << myrange << endl;
#endif
  if (rect_intersect(myrange, src_rect).empty()) return;
  if ((data == NULL)||(data->size()==0)) return;
  int maxscale=32;
  CairoWrapper cr(image);
  cr->set_line_width(1);

  for (int i=0; i < data->size(); i++){
      string file = (*data)[i].file;
      if (!m2ms[i].tst_frw.test(src_rect)) continue;

      int scale = int(scales[i]+0.05);
      if (scale <=0) scale = 1;

      const g_map * m = &(*data)[i];

      if ((scale<=maxscale) && (status[m]&SHOW_MAP)){
        if (!image_cache.contains(i) || (iscales[i] > scale)) {
#ifdef DEBUG_LAYER_GEOMAP
          cerr  << "LayerMap: Loading Image " << file
 	        << " at scale " << scale << "\n";
#endif
          image_cache.add(i, image_r::load(file.c_str(), scale));
          iscales[i] = scale;
        }
#ifdef DEBUG_LAYER_GEOMAP
        cerr  << "LayerMap: Using Image " << file
		 << " at scale " << scale << " (loaded at scale " << iscales[i] <<", scales[i]: " << scales[i] << ")\n";
#endif
        iImage im = image_cache.get(i);
        m2ms[i].image_frw(im, iscales[i], src_rect, image, image.range());
      }

      //draw border
      if ((status[m]&SHOW_BRD) || (scale > maxscale)){
        for (dLine::const_iterator p=m2ms[i].border_dst.begin();
                             p!=m2ms[i].border_dst.end(); p++){
          if (p==m2ms[i].border_dst.begin())
            cr->move_to((*p)-dorigin);
          else
            cr->line_to((*p)-dorigin);
        }
        cr->close_path();
        if (scale > maxscale){ // fill map area
          cr->set_color_a(0x800000FF);
          cr->fill_preserve();
        }
        cr->set_line_width(3);
        cr->set_color(0x0000FF);
        cr->stroke();
      }

      // draw refpoints
      double dr=10, dg=5;
      if (status[m]&SHOW_REF){
        convs::map2pt c1(mymap, Datum("wgs84"), Proj("lonlat"), Options());
        for (g_map::const_iterator p=(*data)[i].begin();
                             p!=(*data)[i].end(); p++){
          dPoint pr(p->xr, p->yr); // raster coords
          m2ms[i].frw(pr);
          pr-=dorigin;
          cr->move_to(pr + dPoint(-1,0)*dr);
          cr->line_to(pr + dPoint(1,0)*dr);
          cr->move_to(pr + dPoint(0,-1)*dr);
          cr->line_to(pr + dPoint(0,1)*dr);
          cr->set_line_width(3);
          cr->set_color(0xFF0000);
          cr->stroke();

          dPoint pg(p->x, p->y);   // geo coords
          c1.bck(pg);
          pg-=dorigin;
          cr->move_to(pg + dPoint(-1,-1)*dg);
          cr->line_to(pg + dPoint(1,1)*dg);
          cr->move_to(pg + dPoint(1,-1)*dg);
          cr->line_to(pg + dPoint(-1,1)*dg);
          cr->set_line_width(3);
          cr->set_color(0x00FF00);
          cr->stroke();
        }

      }
  }
}

iRect
LayerGeoMap::range() const{
  return myrange;
}

void
LayerGeoMap::dump_maps(const char *file){
  ofstream f(file);
  f<< "#FIG 3.2\n"
   << "Portrait\n"
   << "Center\n"
   << "Metric\n"
   << "A4\n"
   << "100.0\n"
   << "Single\n"
   << "-2\n"
   << "1200 2\n";
  for (int i=0;i<m2ms.size();i++){
    int bs = m2ms[i].border_dst.size();
    f << "2 3 0 1 4 29 8 -1 20 0.000 0 0 -1 0 0 "
      << bs << "\n\t";
    double minx=1e99, maxx=-1e99;
	for (int j=0; j<bs; j++){
      double x=m2ms[i].border_dst[j].x;
      double y=m2ms[i].border_dst[j].y;
      if (x<minx) minx=x;
      if (x>maxx) maxx=x;
      f << " " << int(x) << " " << int(y);
    }
    f << "\n";
    if (bs==0) continue;
    double lettw=190;
    double letth=400;
    string s1;
    int n=0, l=0;
    while (n<(*data)[i].comm.size()>0){
      s1+=(*data)[i].comm[n];
      n++;
      if ((n==(*data)[i].comm.size()) || (s1.size()*lettw > maxx-minx)){
      f << "4 0 4 6 -1 18 20 0.0000 4 225 630 " 
        << int(m2ms[i].border_dst[0].x+100) << " " 
        << int(m2ms[i].border_dst[0].y+500 + l*letth) << " " 
        << s1 << "\\001\n";
        s1=""; l++;
      }
    }
  }
  f.close();
}

void
LayerGeoMap::make_m2ms(){
  if ((data == NULL)||(data->size()==0)) return;

  m2ms.clear();
  scales.clear();
  map<const g_map *, int> nstatus;

  for (int i=0; i< data->size(); i++){
      g_map * m = &(*data)[i];
      convs::map2map c(*m, mymap);
      m2ms.push_back(c);
      dPoint p1(0,0), p2(1000,0), p3(0,1000);
      c.frw(p1); c.frw(p2); c.frw(p3);
      double sc_x = 1000/pdist(p1,p2);
      double sc_y = 1000/pdist(p1,p3);

      scales.push_back(sc_x<sc_y ? sc_x:sc_y); // каков масштаб карты в соотв.с проекцией

      if ((i==0) && (c.border_dst.size()!=0)){
        myrange=iRect(c.border_dst[0], c.border_dst[0]);
      }
      // pump range to include all border points
      for (int j=0; j<c.border_dst.size(); j++){
        myrange = rect_pump(myrange, iPoint(c.border_dst[j]));
      }
      // pump range to include all ref points
      for (int j=0; j<m->size(); j++){
        dPoint pr((*m)[j].xr, (*m)[j].yr);
        c.frw(pr);
        const iPoint rr(10,10);
        myrange = rect_bounding_box(myrange,
           iRect(iPoint(pr)-rr, iPoint(pr)+rr));
      }
      if (status.count(m)) nstatus[m] = status[m];
      else nstatus[m] = SHOW_MAP;
  }
  status=nstatus;

  // старые данные нам тоже интересны (мы можем использовать уже загруженные картинки)
  if (iscales.size() != data->size())
    iscales.resize(data->size(),1);
#ifdef DEBUG_LAYER_GEOMAP
  cerr << "LayerMap: Setting map conversions. Range: " << myrange << "\n";
#endif
}
