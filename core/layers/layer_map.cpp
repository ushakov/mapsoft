#include "layer_map.h"
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

LayerMAP::LayerMAP(g_map_list *_data, const Options & opt) :
      data(_data), image_cache(4){
  make_m2ms();
  status_set(SHOW_BRD, opt.exists("map_show_brd"));
}

g_map
LayerMAP::get_myref() const {
  // return ref of first map, swapped if needed
  if (data->size() && (*data)[0].size()){
    g_map ret=(*data)[0];
    convs::map2pt c(ret, Datum("wgs84"), Proj("lonlat"), Options());
    if (!c.swapped()) return ret;
    double ym=ret[0].yr;
    for (int i=1; i<ret.size(); i++){ if (ret[i].yr>ym) ym=ret[i].yr; } // find max yr
    for (int i=0; i<ret.size(); i++){ ret[i].yr = ym-ret[i].yr; } //swap y
    return ret;
  }
  // else return some simple ref
  g_map ret;
  ret.map_proj = Proj("lonlat");
  ret.push_back(g_refpoint(0,  45, 0, 45*3600));
  ret.push_back(g_refpoint(180, 0, 180*3600,90*3600));
  ret.push_back(g_refpoint(0,   0, 0, 90*3600));
  return ret;
}

g_map_list *
LayerMAP::get_data() const{
  return data;
}

g_map *
LayerMAP::get_map(const int n) const{
  return &(*data)[n];
}

int
LayerMAP::find_map(const iPoint & pt) const{
  for (int i=0; i< m2ms.size(); i++){
    if (point_in_line(pt, borders[i])) return i;
  }
  return -1;
}
int
LayerMAP::find_map(const iRect & r) const{
  for (int i=0; i< m2ms.size(); i++){
    if (rect_in_line(r, borders[i])) return i;
  }
  return -1;
}

void
LayerMAP::status_set(int mask, bool val, const g_map * m){
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
LayerMAP::show_ref(const g_map * m){ status_set(SHOW_REF, true,  m); }
void
LayerMAP::hide_ref(const g_map * m){ status_set(SHOW_REF, false, m); }
void
LayerMAP::show_brd(const g_map * m){ status_set(SHOW_BRD, true,  m); }
void
LayerMAP::hide_brd(const g_map * m){ status_set(SHOW_BRD, false, m); }
void
LayerMAP::show_map(const g_map * m){ status_set(SHOW_MAP, true,  m); }
void
LayerMAP::hide_map(const g_map * m){ status_set(SHOW_MAP, false, m); }


void
LayerMAP::refresh(){
  make_m2ms();
}

int
LayerMAP::draw(iImage & image, const iPoint & origin){
  iRect src_rect = image.range() + origin;
  dPoint dorigin(origin);

#ifdef DEBUG_LAYER_GEOMAP
  cerr  << "LayerMAP: draw " << src_rect << " my: " << myrange << endl;
#endif
  if (rect_intersect(myrange, src_rect).empty()) return GOBJ_FILL_NONE;
  if ((data == NULL)||(data->size()==0)) return GOBJ_FILL_NONE;
  int maxscale=32;
  CairoWrapper cr(image);
  cr->set_line_width(1);

  for (int i=0; i < data->size(); i++){
      if (!rect_in_line(src_rect, borders[i])) continue;

      const g_map * m = &(*data)[i];

      int scale = int(scales[i]+0.05);
      if (scale <=0) scale = 1;

      // show map if needed
      if ((scale<=maxscale) && (status[m]&SHOW_MAP)){

        // check image file
        struct stat st_buf;
        if (stat(m->file.c_str(), &st_buf) != 0){
          std::cerr << "Layer_map: can't find image file: " << m->file << endl;
          continue;
        }

        if (S_ISDIR(st_buf.st_mode)){ // tiled map
          // find/create image cache for this map
          if (tmap_cache.count(m->file)==0)
            tmap_cache.insert(make_pair(m->file, Cache<iPoint, iImage>(100)));
          Cache<iPoint, iImage> & C = tmap_cache.find(m->file)->second;
          // clear cache if scale have changed
          if (iscales[i] != scale){
            C.clear();
            iscales[i] = scale;
          }
          dLine brd=m2ms[i]->line_frw(m->border);
          iLineTester brd_tester(brd);
          // look for download script
          bool download = stat((m->file+"/download").c_str(), &st_buf) == 0 &&
                          S_ISREG(st_buf.st_mode);
          // convert image points
          for (int y=0; y<image.h; y++){
            std::vector<int> cr = brd_tester.get_cr(y+origin.y);
            for (int x=0; x<image.w; x++){
              if (!brd_tester.test_cr(cr, x+origin.x)) continue;
              dPoint p(x,y); p+=origin; m2ms[i]->bck(p); p/=scale;
              int tsize = m->tsize/scale;
              iPoint tile = iPoint(p)/tsize;
              iPoint pt = p - dPoint(tile)*tsize;
              pt.y=tsize-pt.y-1;
              if (!C.contains(tile)){
                char fn[PATH_MAX];
                snprintf(fn, sizeof(fn), m->tfmt.c_str(), tile.x, tile.y);
                string tfile = m->file+'/'+fn;
                iImage img = image_r::load(tfile.c_str(), scale);
                if (img.empty() && download){ // try to download file
                  ostringstream ss;
                  ss << "./download " << tile.x << " " << tile.y;
                  char cwd[PATH_MAX];
                  if (getcwd(cwd, PATH_MAX) && chdir(m->file.c_str())==0){
                    if (system(ss.str().c_str())==0)
                       img = image_r::load(fn, scale);
                    if (chdir(cwd)!=0) cerr << "Layer_map: can't do chdir!\n";
                  }
                }
                img.set_border((m->border - tile*m->tsize)/scale);
                C.add(tile, img);
              }
              else{
                image.set_a(x,y,C.get(tile).safe_get(pt));
              }
            }
          }
        }

        else { // normal map
          if (!image_cache.contains(i) || (iscales[i] > scale)) {
            iImage img = image_r::load(m->file.c_str(), scale);
            img.set_border(m->border/scale);
            image_cache.add(i, img);
            iscales[i] = scale;
          }
          m2ms[i]->image_frw(image_cache.get(i), image, origin, 1.0/iscales[i]);
        }
      }


      //draw border
      if ((status[m]&SHOW_BRD) || (scale > maxscale)){
        for (iLine::const_iterator p=borders[i].begin();
                                   p!=borders[i].end(); p++){
          if (p==borders[i].begin())
            cr->move_to((*p)-origin);
          else
            cr->line_to((*p)-origin);
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
        for (g_map::const_iterator p=(*data)[i].begin();
                             p!=(*data)[i].end(); p++){
          dPoint pr(p->xr, p->yr); // raster coords
          m2ms[i]->frw(pr);
          pr-=dorigin;
          cr->move_to(pr + dPoint(-1,0)*dr);
          cr->line_to(pr + dPoint(1,0)*dr);
          cr->move_to(pr + dPoint(0,-1)*dr);
          cr->line_to(pr + dPoint(0,1)*dr);
          cr->set_line_width(3);
          cr->set_color(0xFF0000);
          cr->stroke();

          dPoint pg(p->x, p->y);   // geo coords
          cnv->bck(pg);
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
  return GOBJ_FILL_PART;
}

iRect
LayerMAP::range() const{
  return myrange;
}

void
LayerMAP::dump_maps(const char *file){
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
    int bs = borders[i].size();
    f << "2 3 0 1 4 29 8 -1 20 0.000 0 0 -1 0 0 "
      << bs << "\n\t";
    double minx=1e99, maxx=-1e99;
	for (int j=0; j<bs; j++){
      double x=borders[i][j].x;
      double y=borders[i][j].y;
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
        << int(borders[i][0].x+100) << " " 
        << int(borders[i][0].y+500 + l*letth) << " " 
        << s1 << "\\001\n";
        s1=""; l++;
      }
    }
  }
  f.close();
}

void
LayerMAP::make_m2ms(){
  if (!data || !data->size() || !cnv) return;

  if (cnv_hint<0) cnv_hint=0;

  m2ms.clear();
  scales.clear();
  borders.clear();
  map<const g_map *, int> nstatus;

  for (int i=0; i< data->size(); i++){
      // map -> layer conversion

      g_map * m = &(*data)[i];
      boost::shared_ptr<Conv> c;

      if (cnv_hint == m->map_proj.val){ // the same proj!
        map<dPoint,dPoint> points;
        for (g_map::const_iterator i=m->begin(); i!=m->end(); i++){
          dPoint pr(i->xr, i->yr), pl(*i);
          cnv->bck(pl);
          points[pr] = pl;
        }
        c.reset(new ConvAff(points));
      }
      else {
        std::map<dPoint, dPoint> ref1, ref2;
        for (g_map::const_iterator i=m->begin(); i!=m->end(); i++){
          dPoint p1(i->xr, i->yr); // p1 - map point
          dPoint p2(i->x, i->y);   // p2 - wgs point
          dPoint p3(p2);
          cnv->bck(p3); // p3 - screen point
          ref1[p1]=p2;
          ref2[p3]=p2;
        }
        c.reset(new convs::map2map(m->map_proj, Proj(cnv_hint), ref1, ref2));
      }
      m2ms.push_back(c);

      // converted border
      dLine brd = c->line_frw(m->border);
      borders.push_back(brd);

      // map scaling factor
      dPoint p1(0,0), p2(1000,0), p3(0,1000);
      c->frw(p1); c->frw(p2); c->frw(p3);
      double sc = min(1000/pdist(p1,p2), 1000/pdist(p1,p3));
      scales.push_back(sc);

      if ( i == 0 )
         myrange = brd.range();
      else
         myrange = rect_bounding_box(myrange, iRect(brd.range()));

      // pump range to include all ref points with some radius
      const iPoint rr(10,10);
      for (int j=0; j<m->size(); j++){
        dPoint pr((*m)[j].xr, (*m)[j].yr);
        c->frw(pr);
        myrange = rect_bounding_box(myrange,
           iRect(iPoint(pr)-rr, iPoint(pr)+rr));
      }
      // check old status for this map
      if (status.count(m)) nstatus[m] = status[m];
      else nstatus[m] = SHOW_MAP;
  }
  status=nstatus;

  // старые данные нам тоже интересны (мы можем использовать уже загруженные картинки)
  if (iscales.size() != data->size())
    iscales.resize(data->size(),1);
#ifdef DEBUG_LAYER_GEOMAP
  cerr << "LayerMAP: Setting map conversions. Range: " << myrange << "\n";
#endif
}
