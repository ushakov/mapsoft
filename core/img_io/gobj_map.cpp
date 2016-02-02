#include "gobj_map.h"
#include <sstream>
#include <fstream>
#include <cmath>

#include "loaders/image_r.h"
#include "utils/cairo_wrapper.h"
#include "2d/line_utils.h"

using namespace std;

GObjMAP::GObjMAP(g_map_list *_data, const Options & opt) :
      data(_data), image_cache(4){
  refresh();
  status_set(SHOW_BRD, opt.exists("map_show_brd"));
}

g_map
GObjMAP::get_myref() const {
  // return ref of first map, swapped if needed
  if (data->size() && (*data)[0].size()){
    g_map ret=(*data)[0];
    convs::map2wgs c(ret);
    if (!c.swapped()) return ret;
    double ym=ret[0].yr;
    for (int i=1; i<ret.size(); i++){ if (ret[i].yr>ym) ym=ret[i].yr; } // find max yr
    for (int i=0; i<ret.size(); i++){ ret[i].yr = ym-ret[i].yr; } //swap y
    return ret;
  }
  // else return some simple ref
  return GObjGeo::get_myref();
}

int
GObjMAP::find_map(const iPoint & pt) const{
  for (int i=0; i< m2ms.size(); i++){
    if (point_in_line(pt, borders[i])) return i;
  }
  return -1;
}
int
GObjMAP::find_map(const iRect & r) const{
  for (int i=0; i< m2ms.size(); i++){
    if (rect_in_line(r, borders[i])) return i;
  }
  return -1;
}

void
GObjMAP::status_set(int mask, bool val, const g_map * m){
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

int
GObjMAP::draw(iImage & image, const iPoint & origin){
  iRect src_rect = image.range() + origin;
  dPoint dorigin(origin);

#ifdef DEBUG_LAYER_GEOMAP
  cerr  << "GObjMAP: draw " << src_rect << " my: " << myrange << endl;
#endif
  if (rect_intersect(myrange, src_rect).empty()) return GObj::FILL_NONE;
  if ((data == NULL)||(data->size()==0)) return GObj::FILL_NONE;
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
          iLineTester brd_tester(borders[i]);
          // look for download script
          bool download = stat((m->file+"/download").c_str(), &st_buf) == 0 &&
                          S_ISREG(st_buf.st_mode) && scale==1;
          // convert image points
          for (int y=0; y<image.h; y++){
            std::vector<int> cr = brd_tester.get_cr(y+origin.y);
            for (int x=0; x<image.w; x++){
              if (!brd_tester.test_cr(cr, x+origin.x)) continue;
              dPoint p(x,y); p+=origin; m2ms[i]->bck(p);
              iPoint tile = iPoint(p)/m->tsize;
              dPoint p1 = (p - dPoint(tile)*(m->tsize) )/scale;
              iPoint pt(int(p1.x), int(p1.y));
              if (m->tswap) pt.y=m->tsize/scale-pt.y-1;
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
                C.add(tile, img);
              }
              image.set_a(x,y,C.get(tile).safe_get(pt));
            }
          }
        }

        else { // normal map
          m2ms[i]->image_frw(
            image_cache.get(m->file, scale, m->border),
            image, origin, 1.0/scale);
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
          cr->set_color_a(0x80000080);
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
          cnv.bck(pg);
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
  return GObj::FILL_PART;
}

double
GObjMAP::scale(const dPoint & p, const int m){
  return pdist(m2ms[m]->units_frw(p))/sqrt(2.0);
}

double
GObjMAP::scale(const dPoint & p){
  int m = find_map(p);
  if (m<0) return -1;
  return pdist(m2ms[m]->units_frw(p))/sqrt(2.0);
}

void
GObjMAP::dump_maps(const char *file){
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
GObjMAP::refresh(){
  if (!data || !data->size()) return;

  m2ms.clear();
  scales.clear();
  borders.clear();
  map<const g_map *, int> nstatus;

  for (int i=0; i< data->size(); i++){
      // map -> layer conversion

      g_map * m = &(*data)[i];
      boost::shared_ptr<Conv> c(new convs::map2map(*m, ref));

      if (ref.map_proj == m->map_proj){ // the same proj!
        map<dPoint,dPoint> points;
        for (g_map::const_iterator i=m->begin(); i!=m->end(); i++){
          dPoint p1(i->xr, i->yr), p2(p1);
          c->frw(p2);
          points[p1] = p2; // map->scr
        }
        c.reset(new ConvAff(points));
      }
      m2ms.push_back(c);

      // map scaling factor
      dPoint p1(0,0), p2(1000,0), p3(0,1000); // on scr
      c->bck(p1); c->bck(p2); c->bck(p3);
      double sc = min(pdist(p1,p2)/1000, pdist(p1,p3)/1000);
      scales.push_back(sc);

      // border and range
      if (m->border.size()){
        dLine brd = c->line_frw(m->border);
        if ( i == 0 ) myrange = brd.range();
        else myrange = rect_bounding_box(myrange, iRect(brd.range()));
        borders.push_back(brd);
      }
      else{
        myrange=GObj::MAX_RANGE;
        borders.push_back(rect2line(myrange));
      }

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
  cerr << "GObjMAP: Setting map conversions. Range: " << myrange << "\n";
#endif
}
