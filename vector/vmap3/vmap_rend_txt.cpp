#include "vmap_renderer.h"

void
VMAPRenderer::set_fig_font(int font, double fs, Cairo::RefPtr<Cairo::Context> C){
  std::string       face;
  Cairo::FontSlant  slant;
  Cairo::FontWeight weight;
  switch(font){
    case 0:
      face="times";
      slant=Cairo::FONT_SLANT_NORMAL;
      weight=Cairo::FONT_WEIGHT_NORMAL;
      break;
    case 1:
      face="times";
      slant=Cairo::FONT_SLANT_ITALIC;
      weight=Cairo::FONT_WEIGHT_NORMAL;
      break;
    case 2:
      face="times";
      slant=Cairo::FONT_SLANT_NORMAL;
      weight=Cairo::FONT_WEIGHT_BOLD;
      break;
    case 3:
      face="times";
      slant=Cairo::FONT_SLANT_ITALIC;
      weight=Cairo::FONT_WEIGHT_BOLD;
      break;
    case 16:
      face="sans";
      slant=Cairo::FONT_SLANT_NORMAL;
      weight=Cairo::FONT_WEIGHT_NORMAL;
      break;
    case 17:
      face="sans";
      slant=Cairo::FONT_SLANT_OBLIQUE;
      weight=Cairo::FONT_WEIGHT_NORMAL;
      break;
    case 18:
      face="sans";
      slant=Cairo::FONT_SLANT_NORMAL;
      weight=Cairo::FONT_WEIGHT_BOLD;
      break;
    case 19:
      face="sans";
      slant=Cairo::FONT_SLANT_OBLIQUE;
      weight=Cairo::FONT_WEIGHT_BOLD;
      break;
    default:
      std::cerr << "warning: unsupported fig font: " << font << "\n";
      face="sans";
      slant=Cairo::FONT_SLANT_NORMAL;
      weight=Cairo::FONT_WEIGHT_NORMAL;
  }
  if (face=="times") fs/=0.85;
  C->set_font_size(fs*fs1);
  C->set_font_face(
    Cairo::ToyFontFace::create(face, slant, weight));
}

void
VMAPRenderer::erase_under_text(Cairo::RefPtr<Cairo::ImageSurface> bw_surface,
                               int dark_th, int search_dist){

  // data and s indentifiers used in COL macro!
  unsigned char *data=cr.get_im_surface()->get_data();
  int w=cr.get_im_surface()->get_width();
  int h=cr.get_im_surface()->get_height();
  int s=cr.get_im_surface()->get_stride();
#define COL(x,y)  ((data[s*(y) + 4*(x) + 2] << 16)\
                 + (data[s*(y) + 4*(x) + 1] << 8)\
                 +  data[s*(y) + 4*(x) + 0])
#define ADJ(x,y,i)  COL( x+ ((i==1)?1:0) - ((i==3)?1:0), y + ((i==2)?1:0) - ((i==0)?1:0))
#define DIST2(x,y) ((x)*(x) + (y)*(y))
#define IS_DARK(x,y) (data[s*(y) + 4*(x) + 2]\
                 +  data[s*(y) + 4*(x) + 1]\
                 +  data[s*(y) + 4*(x) + 0] < 3*dark_th)
#define MIN(x,y) (x<y? x:y)
#define MAX(x,y) (x>y? x:y)

  // bw_data and bws indentifires used in IS_TEXT macro!
  unsigned char *bw_data=bw_surface->get_data();
  int bws=bw_surface->get_stride();

  unsigned char *data_e=NULL;
  if (label_style==LABEL_STYLE3)
    data_e=cr_e.get_im_surface()->get_data();

#define IS_TEXT(x,y)  ((bw_data[bws*(y) + (x)/8] >> ((x)%8))&1)\


  if (label_style==LABEL_STYLE2){
    // walk through all points
    for (int y=0; y<h; y++){
      for (int x=0; x<w; x++){
        if (!IS_TEXT(x,y)) continue;

        if (!IS_DARK(x,y)) continue;
        // find nearest point with light color:
        int r = search_dist;
        int dd = 2*search_dist*search_dist+1;
        int yym=y, xxm=x;
        for (int yy = MAX(0, y-r); yy < MIN(h, y+r+1); yy++){
          for (int xx = MAX(0, x-r); xx < MIN(w, x+r+1); xx++){
            if (IS_DARK(xx,yy)) continue;
            if ((y-yy)*(y-yy) + (x-xx)*(x-xx) < dd){
              dd = (y-yy)*(y-yy) + (x-xx)*(x-xx);
              yym=yy; xxm=xx;
            }
          }
        }
        if ((xxm==x) && (yym==y))
          memset(data + s*y + 4*x, 0xFF, 3);
        else
          memcpy(data + s*y + 4*x, data + s*yym + 4*xxm, 3);
      }
    }
  }
  else if (label_style==LABEL_STYLE1){
    cr->set_source_rgba(1,1,1,0.5);
    cr->mask(bw_surface,0,0);
  }
  else if (label_style==LABEL_STYLE3){
    Cairo::RefPtr<Cairo::SurfacePattern> patt =
      Cairo::SurfacePattern::create(cr_e.get_im_surface());
    cr->set_source(patt);
    cr->mask(bw_surface,0,0);
  }
}

void
VMAPRenderer::render_labels(double bound, int dark_th, int search_dist){
  cr->save();

  zn::zn_conv zc(W->style);

  Cairo::RefPtr<Cairo::ImageSurface> bw_surface;
  Cairo::RefPtr<Cairo::Context> bw_cr;

  if ((label_style==LABEL_STYLE1) ||
      (label_style==LABEL_STYLE2) ||
      (label_style==LABEL_STYLE3)){
    bw_surface = Cairo::ImageSurface::create(
      Cairo::FORMAT_A1, cr.get_im_surface()->get_width(),
                        cr.get_im_surface()->get_height());
    bw_cr = Cairo::Context::create(bw_surface);
    bw_cr->set_line_width(bound*lw1);
    bw_cr->set_line_join(Cairo::LINE_JOIN_ROUND);
  }

  for (int pass=1; pass<=2; pass++){
    for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
      std::map<int, zn::zn>::const_iterator z = zc.find_type(o->type);
      if (z==zc.znaki.end()) continue;
      if (!z->second.label_type) continue;

      if (pass==2) cr->set_color(z->second.txt.pen_color);

      Cairo::RefPtr<Cairo::Context> cur_cr = (pass==1)? bw_cr:cr;
      if (!cur_cr) continue;

      set_fig_font(z->second.txt.font, z->second.txt.font_size, cur_cr);

      Cairo::TextExtents ext;
      cur_cr->get_text_extents (o->text, ext);

      for (std::list<vmap::lpos>::const_iterator l=o->labels.begin(); l!=o->labels.end(); l++){
        dPoint p(l->pos);
        cur_cr->save();
        cur_cr->move_to(p.x, p.y);
        if (!l->hor) cur_cr->rotate(l->ang);
        if (l->dir == 1) cur_cr->rel_move_to(-ext.width/2, 0);
        if (l->dir == 2) cur_cr->rel_move_to(-ext.width, 0);
        if (pass == 1){
          cur_cr->text_path(o->text);
          cur_cr->stroke();
        }
        else {
          cur_cr->show_text(o->text); // draw text
        }
        cur_cr->restore();
      }
    }
    if ((pass==1)&&(bw_surface))
      erase_under_text(bw_surface, dark_th, search_dist);
  }
  cr->restore();
}

void
VMAPRenderer::render_text(const char *text, dPoint pos, double ang,
       int color, int fig_font, int font_size, int hdir, int vdir){
  cr->save();
  cr->set_color(color);
  set_fig_font(fig_font, font_size, cr);

  dRect ext = cr->get_text_extents(text);

  if (pos.x<0) pos.x=cr.get_im_surface()->get_width()+pos.x;
  if (pos.y<0) pos.y=cr.get_im_surface()->get_height()+pos.y;
  cr->move_to(pos);
  cr->rotate(ang);
  if (hdir == 1) cr->Cairo::Context::rel_move_to(-ext.w/2, 0.0);
  if (hdir == 2) cr->Cairo::Context::rel_move_to(-ext.w, 0.0);
  if (vdir == 1) cr->Cairo::Context::rel_move_to(0.0, ext.h/2);
  if (vdir == 2) cr->Cairo::Context::rel_move_to(0.0, ext.h);

  cr->reset_clip();

  cr->show_text(text);
  cr->restore();
}
