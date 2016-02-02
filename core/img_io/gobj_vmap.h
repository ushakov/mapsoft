#ifndef GOBJ_VMAP
#define GOBJ_VMAP

#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <cstring>
#include <utils/cairo_wrapper.h>

#include <2d/line_utils.h>
#include "2d/line_dist.h"
#include <geo/geo_convs.h>
#include <geo/geo_nom.h>
#include <geo_io/io_oe.h>
#include "vmap/zn.h"
#include "vmap/vmap.h"

#include "img_io/gobj_geo.h"

#define LABEL_STYLE0 0
#define LABEL_STYLE1 1
#define LABEL_STYLE2 2

class GObjVMAP : public GObjGeo{
private:

  CairoWrapper cr;
  vmap::world * W;
  zn::zn_conv zc;


  double       pics_dpi;
  double       dpi, lw1;
  int          bgcolor;
  bool         cntrs, use_aa;
  int          label_style;
  double       label_marg;
  double       grid_step;
  bool         transp;
  int          grid_labels;
  dPoint       origin;

public:

  /***/
  GObjVMAP(vmap::world * _W, const Options & O = Options());

  int draw(iImage &img, const iPoint &origin);
  vmap::world * get_data() const {return W;}

  // convert coordinates from meters to pixels
  void pt_m2pt(dPoint & p);

  void render_objects();
  void render_holes(Conv & cnv);

  // place image in the center of polygons
  // polygons filled with image pattern
  void render_img_polygons(int type, double curve_l=0);

  // place image in points
  void render_im_in_points(int type);

  void  render_polygons(int type, int col, double curve_l=0);
  // contoured polygons
  void  render_cnt_polygons(int type, int fill_col, int cnt_col,
                        double cnt_th, double curve_l=0);

  void  render_line(int type, int col, double th, double curve_l=0);
  void  render_points(int type, int col, double th);

  // paths for bridge sign
  void mkbrpath1(const vmap::object & o);
  void mkbrpath2(const vmap::object & o, double th, double side);
  // мосты
  void render_bridge(int type, double th1, double th2, double side);
  // лэп
  void render_line_el(Conv & cnv, int type, int col, double th, double step=40);
  // обрывы
  void render_line_obr(Conv & cnv, int type, int col, double th);
  // заборы
  void render_line_zab(Conv & cnv, int type, int col, double th);
  // вал
  void render_line_val(Conv & cnv, int type, int col, double th, 
                       double width, double step);
  // газопроводы
  void render_line_gaz(Conv & cnv, int type, int col,
                       double th, double step=40);


  // нарисовать сетку с шагом dx,dy см в координатах Г-К СК1942г
  void render_pulk_grid(double dx, double dy, bool draw_labels, const g_map & ref);
  // нарисовать подпись сетки в месте пересейчаения
  // границы карты и линий с данным x=c или y=c (horiz=false)
  // todo: merge common code for x and y?
  void render_grid_label(double c, double val, bool horiz, const dLine & border);

  void render_labels();

  // functions for drawing contours

  // создание контура -- набор точек на расстоянии dist друг от друга
  // вокруг областей с цветом col. Цвет в точках = col
  std::list<iPoint> make_cnt(int c, double dist);

  // Фильтрация контура - удаляем точки, попавшие в области с
  // цветом, отличным от col
  // Прогоняется после изменения картинки, чтобы удалить контуры,
  // которые были чем-то перекрыты.
  void filter_cnt(std::list<iPoint> & cnt, int c);

  // рисование уже готового контура
  void draw_cnt(const std::list<iPoint> & cnt, int c, double th);

};


#endif
