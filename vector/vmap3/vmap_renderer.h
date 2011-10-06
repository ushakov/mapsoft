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
  const label_style_t label_style;

  CairoWrapper cr;
  vmap::world * W;
  double dpi, lw1, fs1;
  g_map ref;

  // convert coordinates from meters to pixels
  void pt_m2pt(dPoint & p);

  VMAPRenderer(vmap::world * _W,
    int dpi_=300, int lm=0, int tm=0, int rm=0, int bm=0,
    bool use_aa=true, label_style_t _label_style=LABEL_STYLE0);

  void unset_dash();
  void set_dash(double d1, double d2);
  void set_dash(double d1, double d2, double d3, double d4);
  void set_cap_round();
  void set_cap_butt();
  void set_cap_square();
  void set_join_miter();
  void set_join_round();


  void render_objects(const bool draw_contours=true);
  void render_holes();

  // picture-related functions (see vmap_rend_pic.cpp)

  // create pattern from png-file, rescaled
  // according to dpi and pics_dpi values and
  // translated to the image center
  Cairo::RefPtr<Cairo::SurfacePattern>
  get_patt_from_png(const char * fname);

  void
  paintim(const vmap::object & o,
          const Cairo::RefPtr<Cairo::SurfacePattern> & patt, double ang=0);


  // place image in the center of polygons
  void render_im_in_polygons(int type, const char * fname);

  // place image in points
  void render_im_in_points(int type, const char * fname);

  // path for drawing points
  void mkptpath(const dMultiLine & o);

  void  render_polygons(int type, int col, double curve_l=0);
  // contoured polygons
  void  render_cnt_polygons(int type, int fill_col, int cnt_col,
                        double cnt_th, double curve_l=0);
  // polygons filled with image pattern
  void render_img_polygons(int type, const char * fname, double curve_l=0);

  void  render_line(int type, int col, double th, double curve_l=0);
  void  render_points(int type, int col, double th);




  // paths for bridge sign
  void  mkbrpath1(const vmap::object & o);

  void mkbrpath2(const vmap::object & o, double th, double side);

  // мосты
  void render_bridge(int type, double th1, double th2, double side);

  // лэп
  void render_line_el(int type, int col, double th, double step=40);

  // обрывы
  void render_line_obr(int type, int col, double th);

  // заборы
  void render_line_zab(int type, int col, double th);

  // газопроводы
  void render_line_gaz(int type, int col, double th, double step=40);

  void save_image(const char * png, const char * map);


  // нарисовать сетку с шагом dx,dy см в координатах Г-К СК1942г
  void render_pulk_grid(double dx, double dy, bool draw_labels);
  // нарисовать подпись сетки в месте пересейчаения
  // границы карты и линий с данным x=c или y=c (horiz=false)
  // todo: merge common code for x and y?
  void render_grid_label(double c, double val, bool horiz);

  // functions for drawing text (see vmap_rend_txt.cpp)

  void set_fig_font(int font, double fs, Cairo::RefPtr<Cairo::Context> C);

  void erase_under_text(Cairo::RefPtr<Cairo::ImageSurface> bw_surface,
                   int dark_th, int search_dist);

  void render_labels(double bound=2.5, int dark_th = 170, int search_dist=6);

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
