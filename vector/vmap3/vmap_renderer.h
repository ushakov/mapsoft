#ifndef VMAP_RENDERER
#define VMAP_RENDERER

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

typedef enum {
  LABEL_STYLE0,
  LABEL_STYLE1,
  LABEL_STYLE2,
} label_style_t;


struct VMAPRenderer{

  const static double pics_dpi;
  const static char * pics_dir;

  CairoWrapper cr;
  vmap::world * W;
  double dpi, lw1, fs1;
  g_map ref;
  int bgcolor;

  // convert coordinates from meters to pixels
  void pt_m2pt(dPoint & p);

  VMAPRenderer(vmap::world * _W, iImage & img,
    const g_map & ref, const Options & O = Options());

  void unset_dash();
  void set_dash(double d1, double d2);
  void set_dash(double d1, double d2, double d3, double d4);
  void set_cap_round();
  void set_cap_butt();
  void set_cap_square();
  void set_join_miter();
  void set_join_round();


  void render_objects(Conv & cnv, const bool draw_contours=true);
  void render_holes(Conv & cnv);

  // picture-related functions (see vmap_rend_pic.cpp)

  // create pattern from iImage, rescaled
  // according to dpi and pics_dpi values and
  // translated to the image center
  Cairo::RefPtr<Cairo::SurfacePattern>
    get_patt_from_image(const iImage & I);

  // place image in the center of polygons
  void render_im_in_polygons(Conv & cnv, int type, const char * fname);

  // place image in points
  void render_im_in_points(Conv & cnv, int type, const char * fname);

  // path for drawing points
  void mkptpath(Conv & cnv, const dMultiLine & o);

  void  render_polygons(Conv & cnv, int type, int col, double curve_l=0);
  // contoured polygons
  void  render_cnt_polygons(Conv & cnv, int type, int fill_col, int cnt_col,
                        double cnt_th, double curve_l=0);
  // polygons filled with image pattern
  void render_img_polygons(Conv & cnv, int type, const char * fname, double curve_l=0);

  void  render_line(Conv & cnv, int type, int col, double th, double curve_l=0);
  void  render_points(Conv & cnv, int type, int col, double th);




  // paths for bridge sign
  void  mkbrpath1(Conv & cnv, const vmap::object & o);

  void mkbrpath2(Conv & cnv, const vmap::object & o, double th, double side);

  // мосты
  void render_bridge(Conv & cnv, int type, double th1, double th2, double side);

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
  void render_line_gaz(Conv & cnv, int type, int col, double th, double step=40);


  // нарисовать сетку с шагом dx,dy см в координатах Г-К СК1942г
  void render_pulk_grid(double dx, double dy, bool draw_labels);
  // нарисовать подпись сетки в месте пересейчаения
  // границы карты и линий с данным x=c или y=c (horiz=false)
  // todo: merge common code for x and y?
  void render_grid_label(double c, double val, bool horiz);

  // functions for drawing text (see vmap_rend_txt.cpp)

  void set_fig_font(int font, double fs, Cairo::RefPtr<Cairo::Context> C);

  void render_labels(Conv & cnv, label_style_t label_style, double bound=2.5);

  void render_text(const char *text, dPoint pos, double ang=0,
         int color=0, int fig_font=18, int font_size=10, int hdir=0, int vdir=0);


  // functions for drawing contours (see vmap_rend_cnt.cpp)

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
