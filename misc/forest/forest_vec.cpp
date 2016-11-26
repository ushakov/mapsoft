#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <queue>
#include <list>
#include <stdexcept>
#include <cmath>

#include "srtm/srtm3.h"
#include "2d/line.h"
#include "geo/geo_nom.h"

#include "2d/line_utils.h"
#include "2d/line_rectcrop.h"
#include "2d/point_int.h"

#include "geo_io/geofig.h"
#include "geo/geo_data.h"
#include "geo/geo_convs.h"

#include "loaders/image_tiff.h"
#include "loaders/image_jpeg.h"
#include "geo_io/io.h"

// Global Forest Change 2000-2014
// http://earthenginepartners.appspot.com/science-2013-global-forest/download_v1.2.html

using namespace std;

void usage(){
    cerr << "usage: \n"
     " mapsoft_forest2fig <fig_name> <forest_dir>\n";
    exit(0);
}

// найти множество соседних точек, в которых img>0 (не более max точек)
set<iPoint>
plane(iImage & img, const iPoint& p, int max){
  set<iPoint> ret;
  queue<iPoint> q;
  if (!img.safe_get(p)) return ret;
  q.push(p);
  ret.insert(p);
  while (!q.empty()){
    iPoint p1 = q.front();
    q.pop();
    for (int i=0; i<8; i++){
      iPoint p2 = adjacent(p1, i);
      if (img.safe_get(p2)&&(ret.insert(p2).second)) {
        q.push(p2);
      }
    }
    if ((max!=0)&&(ret.size()>max)) break;
  }
  return ret;
}


void smooth_image(iImage & img){
  for (int y=0; y<img.h; y++){
    for (int x=0; x<img.w; x++){
      int sum=img.get(x,y) & 0xFF, n=1;
      for (int i=0;i<8;i++){
        iPoint p=adjacent(iPoint(x,y),i);
        if (p.x>0 && p.x<img.w && p.y>0 && p.y<img.h)
          sum+= img.get(p) & 0xFF;
          n++;
      }
      img.set(x,y, ((sum/n) << 8) + (img.get(x,y) & 0xFF));
    }
  }
  for (int y=0; y<img.h; y++){
    for (int x=0; x<img.w; x++){
      img.set_na(x,y, img.get(x,y)>>8 );
    }
  }
}

//координаты угла единичного квадрата по его номеру
iPoint crn (int k){ k%=4; return iPoint(k/2, (k%3>0)?1:0); }

//направление следующей за углом стороны (единичный вектор)
iPoint dir (int k){ return crn(k+1)-crn(k); }

dMultiLine get_contours(const iImage & img, int th, iRect range = iRect()){
  dMultiLine ret;
  int count = 0;
  if (range.empty()) range = img.range();
  range = rect_intersect(range, img.range());

  for (int y=range.y-1; y<range.y+range.h; y++){
    for (int x=range.x-1; x<range.x+range.w; x++){

      iPoint p(x,y);
      // пересечения четырех сторон клетки с горизонталями:
      // при подсчетах мы опустим все данные на полметра,
      // чтоб не разбирать кучу случаев с попаданием горизонталей в узлы сетки
      vector<double> pts;

      for (int k=0; k<4; k++){
        iPoint p1 = p+crn(k);
        iPoint p2 = p+crn(k+1);
        int v1=0, v2=0;
        if (point_in_rect(p1, range)) v1=img.get(p1)&0xff;
        if (point_in_rect(p2, range)) v2=img.get(p2)&0xff;
        if (v1==v2) continue;
        double u = double(th+0.5-v1)/double(v2-v1);
        if ((u<0)||(u>1)) continue;
        pts.push_back(k+u);
      }

      // найдем, какие контуры пересекают квадрат дважды,
      // поместим их в список  ret
      double x1,x2;
      for (int i=0; i<(int)pts.size()-1; i+=2){
        x1 = pts[i];
        x2 = pts[i+1];
        dPoint p1=(dPoint(p + crn(int(x1))) + dPoint(dir(int(x1)))*double(x1-int(x1)));
        dPoint p2=(dPoint(p + crn(int(x2))) + dPoint(dir(int(x2)))*double(x2-int(x2)));

        // we found segment p1-p2
        // first try to append it to existing line in ret[h]
        bool done=false;
        for (dMultiLine::iterator l=ret.begin(); l!=ret.end(); l++){
          int e=l->size()-1;
          if (e<=0) continue; // we have no 1pt lines!
          if (pdist((*l)[0], p1) < 1e-8){ l->insert(l->begin(), p2); done=true; break;}
          if (pdist((*l)[0], p2) < 1e-8){ l->insert(l->begin(), p1); done=true; break;}
          if (pdist((*l)[e], p1) < 1e-8){ l->push_back(p2); done=true; break;}
          if (pdist((*l)[e], p2) < 1e-8){ l->push_back(p1); done=true; break;}
        }
        if (!done){ // insert new line into ret
          dLine hor;
          hor.push_back(p1);
          hor.push_back(p2);
          ret.push_back(hor);
        }
        count++;
      }
    }
  }

  // merge contours
  merge(ret, 1e-8);
  return ret;
}
/*

typedef pair<iPoint,char> side_t;
dMultiLine get_contours1(const iImage & img, int th){

  // Classify points, fill crossing mask.
  // Threshold is shifted by 0.5 to avoid corner crossings (v1..v4==0).
  map<side_t, dPoint> cr;    // side -> exact crossing
  map<side_t, side_t> pairs; // side-side pairs

  for (int y=0; y<img.h-1; y++){
    for (int x=0; x<img.w-1; x++){
      double v1 = th+0.5-img.get(x,y);
      double v2 = th+0.5-img.get(x+1,y);
      double v3 = th+0.5-img.get(x+1,y+1);
      double v4 = th+0.5-img.get(x,y+1);
      iPoint ip(x,y);

      // find crossings
      vector<char> sides;
      if (v1*v2<0){
        dPoint p(x+v1/(v2-v1),y);
        cr.insert(pair<iPoint,char>(ip,0), p);
        sides.push_back(0);
      }
      if (v2*v3<0){
        dPoint p(x+1,y+v2/(v3-v2));
        cr.insert(pair<iPoint,char>(ip,1), p);
        sides.push_back(1);
      }
      if (v3*v4<0){
        dPoint p(x+1-v3/(v4-v3),y+1);
        cr.insert(pair<iPoint,char>(ip,2), p);
        sides.push_back(2);
      }
      if (v4*v1<0){
        dPoint p(x,y+1-v4/(v1-v4));
        cr.insert(pair<iPoint,char>(ip,3), p);
        sides.push_back(3);
      }

      // make pairs
      if (sides.size()==0) continue;

      if (sides.size()==2){
        pairs.insert(side_t(ip,sides[0]),side_t(ip,sides[1]));
        pairs.insert(side_t(ip,sides[1]),side_t(ip,sides[0]));
        continue;
      }

      if (sides.size()==4){
        double d1=pdist(sides[0],sides[1]);
        double d2=pdist(sides[0],sides[2]);
        double d3=pdist(sides[0],sides[3]);
        int i1,i2,j1,j2;
        if      (d1<=d2 && d1<=d3){ i1=0; i2=1; j1=2; j2=3; }
        else if (d2<=d1 && d2<=d3){ i1=0; i2=2; j1=1; j2=3; }
        else if (d3<=d1 && d3<=d2){ i1=0; i2=3; j1=1; j2=2; }
        pairs.insert(side_t(ip,sides[i1]),side_t(ip,sides[i2]));
        pairs.insert(side_t(ip,sides[i2]),side_t(ip,sides[i1]));
        pairs.insert(side_t(ip,sides[j1]),side_t(ip,sides[j2]));
        pairs.insert(side_t(ip,sides[j2]),side_t(ip,sides[j1]));
        continue;
      }
      cerr << "Fatal error: wrong number of crossings\n";
      exit(1);
    }
  }
  // boundaries
  for (int x=0; x<img.w; x++){
    double v1 = th-img.get(x,0);
  }
  dMultiLine ret;
  return ret;
}

*/



/// Ближайшая точка на отрезке
dPoint find_nearest(const dPoint & p,
                 const dPoint & p1, const dPoint & p2){

  double  ll = pdist(p1,p2);
  if (ll==0) return p1; // отрезок нулевой длины

  dPoint vec = Point<double>(p2-p1)/ll;

  double l1 = pdist(p,p1);
  double l2 = pdist(p,p2);
  dPoint ret = l1<l2 ? p1:p2;

  double prl = pscal(dPoint(p-p1), vec);

  if ((prl>=0)&&(prl<=ll)){ // проекция попала на отрезок
    ret = Point<double>(p1) + vec * prl;
  }
  return ret;
}


// разделить слишком бьольшие многоугольники на части (не больше max точек)
void cut_poly(dMultiLine & ml, const int max=20000){
  dMultiLine::iterator l = ml.begin();
  while (l!=ml.end()){
    // line is short, skip it
    if (l->size()<max) {l++; continue;}

    // line is too long
    dRect range1 = l->range();
    range1.h/=2; range1.w/=2;

    dRect range2 = range1; range2.x+=range2.w;
    dRect range3 = range1; range3.y+=range3.h;
    dRect range4 = range2; range4.y+=range4.h;

    dMultiLine mlc1 = rect_crop_ml(range1, *l, true);
    dMultiLine mlc2 = rect_crop_ml(range2, *l, true);
    dMultiLine mlc3 = rect_crop_ml(range3, *l, true);
    dMultiLine mlc4 = rect_crop_ml(range4, *l, true);

    ml.insert(ml.end(), mlc1.begin(), mlc1.end());
    ml.insert(ml.end(), mlc2.begin(), mlc2.end());
    ml.insert(ml.end(), mlc3.begin(), mlc3.end());
    ml.insert(ml.end(), mlc4.begin(), mlc4.end());
    l=ml.erase(l);
  }
}



void img2fig(const iImage & img, convs::map2map & cnv, fig::fig_world & F, const char * fig_mask, int split=1){
  for (int y=0;y<split;y++){
    for (int x=0;x<split;x++){
      iRect range = img.range();
      range.w = range.w/split + 1;
      range.h = range.h/split + 1;
      range.x += range.w*x;
      range.y += range.h*y;
      cerr << "\nrange: " << range << "\n";

      cerr << "cnt: ";
      dMultiLine cnt = get_contours(img, 50, range);
      cerr << cnt.size();

      cerr << "gen: ";
      generalize(cnt,0.1);
      cerr << cnt.size();

      cerr << "join: ";
      join_polygons1(cnt);
      cerr << cnt.size();

      cerr << "cut: ";
      cut_poly(cnt, 10000);
      cerr << cnt.size();

      cerr << "fig:\n";
      for(dMultiLine::iterator iv = cnt.begin(); iv!=cnt.end(); iv++){
        if (iv->size()<3) continue;
        dLine l = cnv.line_frw(*iv);
        fig::fig_object o = fig::make_object(fig_mask);
        o.insert(o.end(), l.begin(), l.end());
        F.push_back(o);
      }
    }
  }
}

main(int argc, char** argv){
  if (argc < 3) usage();

  std::string fig_name = argv[1];
  std::string forest_dir = argv[2];

  fig::fig_world F;
  if (!fig::read(fig_name.c_str(), F)) {
    std::cerr << "Can't open file, exiting.\n";
    exit(1);
  }
  g_map fig_ref = fig::get_ref(F);
  dRect range = fig_ref.range(); // fig range (lonlat)

//  dRect range = convs::nom_to_range(name);
//  convs::pt2wgs cnv(Datum("pulk"), Proj("lonlat"));
//  range = cnv.bb_frw(range);

  int deg2pt = 4000;
  int tsize  = 40000;
  iRect irange = range*deg2pt;
  iRect img_range = irange - irange.TLC();
  iRect tiles = tiles_on_rect(irange, tsize);

  int w=irange.w, h=irange.h;

  // load all data
  iImage tc2000(w,h);
  iImage gain(w,h);
  iImage loss(w,h);
  iImage mask(w,h);

  for (int yt = tiles.y; yt<tiles.y+tiles.h; yt++){
    for (int xt = tiles.x; xt<tiles.x+tiles.w; xt++){
      ostringstream crd;
      crd << "_" << setw(2) << setfill('0') << abs((yt+1)*10) << (yt+1<0?'S':'N')
          << "_" << setw(3) << setfill('0') << abs(xt*10) << (xt<0?'W':'E');
      iRect load = irange-iPoint(xt,yt)*tsize;
      load.y = tsize-load.y-load.h;
      string n_tc2000 = forest_dir + "/" + "Hansen_GFC2015_treecover2000" + crd.str() + ".tif";
      string n_loss   = forest_dir + "/" + "Hansen_GFC2015_loss" + crd.str() + ".tif";
      string n_gain   = forest_dir + "/" + "Hansen_GFC2015_gain" + crd.str() + ".tif";
      string n_mask   = forest_dir + "/" + "Hansen_GFC2015_datamask" + crd.str() + ".tif";
      image_tiff::load(n_tc2000.c_str(), load, tc2000, tc2000.range());
      image_tiff::load(n_loss.c_str(), load, loss, loss.range());
      image_tiff::load(n_gain.c_str(), load, gain, gain.range());
      image_tiff::load(n_mask.c_str(), load, mask, mask.range());
    }
  }


  // changes
  iImage change(w,h,0);
  for (int y = 0; y<h; y++){
    for (int x = 0; x<tc2000.w; x++){
      int g = gain.get(x,y) & 0xFF;
      int l = loss.get(x,y) & 0xFF;
      if (g || l) change.set(x,y,1);
    }
  }
  gain=iImage();
  loss=iImage();

  iImage v_fld(w, h,0);
  iImage v_new(w, h,0);
  iImage v_old(w, h,0);

  // classify changes
  for (int y = 0; y<h; y++){
    for (int x = 0; x<w; x++){

      // update mask and tc2000
      if ((mask.get(x,y)&0xFF)==2) mask.set_na(x,y,100);
      else mask.set(x,y,0);
      tc2000.set_na(x,y,tc2000.get(x,y) & 0xFF);

      if ((tc2000.get(x,y)&0xFF)>0 && (tc2000.get(x,y)&0xFF)<50)
        v_fld.set_na(x,y,100);

      // fined changed area
      set<iPoint> v = plane(change, iPoint(x,y),0);
      if (v.size()<5) continue;

      // parameters: old forest percentage + gain/loss percentage
      double fc=0, gc=0, cc=v.size();
      for(set<iPoint>::const_iterator i = v.begin(); i!=v.end(); i++){
        fc += (tc2000.get(*i) & 0xff)/100.0/cc;
        gc += (gain.get(*i) & 0xff)/cc;
        change.safe_set(*i,0);
      }

      // fill images for building vector data
      for(set<iPoint>::const_iterator i = v.begin(); i!=v.end(); i++){
        if (fc<0.1){
          v_fld.set_na(*i,100);
          tc2000.set_na(*i, 0);
        }
        else if (gc<0.1){
          v_new.set_na(*i,100);
          tc2000.set_na(*i,100);
        }
        else{
          v_old.set_na(*i,100);
          tc2000.set_na(*i,100);
        }
      }
    }
  }

  // smooth images
  smooth_image(v_fld);
  smooth_image(v_new);
  smooth_image(v_old);
  smooth_image(mask);

  g_map ref;
  ref.map_proj = Proj("lonlat");
  ref.push_back(g_refpoint(range.TLC(), img_range.BLC()));
  ref.push_back(g_refpoint(range.BLC(), img_range.TLC()));
  ref.push_back(g_refpoint(range.TRC(), img_range.BRC()));
  convs::map2map cnv(ref,fig_ref);

  img2fig(mask, cnv, F, "2 3 0 1 5269247 11 85 -1 20 0.000 0 0 -1 0 0 0");
  img2fig(v_fld,  cnv, F, "2 3 0 0 11206570 7 98 -1 43 0.000 0 0 -1 0 0 0", 5);
  img2fig(v_new,  cnv, F, "2 3 0 0 12  7  98 -1 43 0.000 0 0 -1 0 0 0", 5);
  img2fig(v_old,  cnv, F, "2 3 0 0 12 11206570  98 -1 43 0.000 0 0 -1 0 0 0", 5);
  img2fig(tc2000, cnv, F, "2 3 0 0 12 11206570 100 -1 20 0.000 0 1 -1 0 0 0", 10);


  fig::write(fig_name.c_str(), F);
  exit(0);
}
