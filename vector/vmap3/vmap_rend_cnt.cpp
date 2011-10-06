#include "vmap_renderer.h"

// functions for drawing contours

// создание контура -- набор точек на расстоянии dist друг от друга
// вокруг областей с цветом col. Цвет в точках = col
std::list<iPoint>
VMAPRenderer::make_cnt(int col, double dist){
  dist*=lw1;

#define COL(x,y)  ((data[s*(y) + 4*(x) + 2] << 16)\
               + (data[s*(y) + 4*(x) + 1] << 8)\
               +  data[s*(y) + 4*(x) + 0])
#define PCOL(p)  COL((p).x,(p).y)
#define ADJ(p,i)  COL( (p).x + ((i==1)?1:0) - ((i==3)?1:0),\
                       (p).y + ((i==2)?1:0) - ((i==0)?1:0))

  // data and s indentifires used in COL macro!
  unsigned char *data=cr.get_im_surface()->get_data();
  int w=cr.get_im_surface()->get_width();
  int h=cr.get_im_surface()->get_height();
  int s=cr.get_im_surface()->get_stride();
  std::list<iPoint> points;
  std::list<iPoint> ret;

  // walk through all non-border points
  iPoint p;
  for (p.y=1; p.y<h-1; p.y++){
    for (p.x=1; p.x<w-1; p.x++){
      bool mk_pt=false;
      if (PCOL(p) != col) continue;
      for (int k=0;k<4;k++){
        if (ADJ(p,k) != col) {
          mk_pt=true;
          break;
        }
      }
      if (!mk_pt) continue;


      std::list<iPoint>::iterator i=points.begin();
      while (i!=points.end()){
        if (pdist(p - *i) < dist) {
          mk_pt=false;
          break;
        }
	// We scan the field from top to bottom. Therefore we will
	// never use points that are higher than (current y - dist)
	// again.
        if (abs(p.y - i->y) > dist) {
          i=points.erase(i);
        }
        else {
          i++;
        }
      }
      if (!mk_pt) continue;
      points.push_back(p);
      ret.push_back(p);
    }
  }
  return ret;
}

void
VMAPRenderer::filter_cnt(std::list<iPoint> & cnt, int col){
  // data and s indentifires used in COL macro!
  unsigned char *data=cr.get_im_surface()->get_data();
  int w=cr.get_im_surface()->get_width();
  int h=cr.get_im_surface()->get_height();
  int s=cr.get_im_surface()->get_stride();
  std::list<iPoint>::iterator p=cnt.begin();
  while (p != cnt.end()){
    if (p->x >= w || p->x < 0 || p->y >= h || p->y < 0) {
      p = cnt.erase(p);
    } else if (PCOL(*p)!=col) {
      p = cnt.erase(p);
    } else {
      p++;
    }
  }
}

// рисование уже готового контура
void
VMAPRenderer::draw_cnt(const std::list<iPoint> & cnt, int c, double th){
  cr->save();
  cr->begin_new_path();
  cr->set_line_width(th*lw1);
  cr->set_color(c);
  cr->set_line_cap(Cairo::LINE_CAP_ROUND);

  for ( std::list<iPoint>::const_iterator p=cnt.begin(); p!=cnt.end(); p++){
    cr->move_to(*p);
    cr->rel_line_to(dPoint());
  }
  cr->stroke();
  cr->restore();
}
