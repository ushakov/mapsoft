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
VMAPRenderer::render_labels(label_style_t label_style, double bound){
  cr->save();

  zn::zn_conv zc(W->style);

  for (int pass=1; pass<=2; pass++){

    // no need for first pass
    if ((pass==1) && (label_style==LABEL_STYLE0)) continue;

    for (vmap::world::const_iterator o=W->begin(); o!=W->end(); o++){
      std::map<int, zn::zn>::const_iterator z = zc.find_type(o->type);
      if (z==zc.znaki.end()) continue;
      if (!z->second.label_type) continue;

      if (pass==1){
        cr->set_line_width(bound*lw1);
        cr->set_line_join(Cairo::LINE_JOIN_ROUND);
        if (label_style==LABEL_STYLE1){ // lighten everything below text
          cr->set_source_rgba(1,1,1,0.5);
        }
        else if (label_style==LABEL_STYLE2){ // erase everything below text
          cr->set_operator(Cairo::OPERATOR_SOURCE);
          cr->set_source_rgba(1,1,1,0);
        }
      }
      else { // pass 2 -- draw text
        cr->set_color(z->second.txt.pen_color);
      }

      set_fig_font(z->second.txt.font, z->second.txt.font_size, cr);

      dRect ext=cr->get_text_extents(o->text);

      for (std::list<vmap::lpos>::const_iterator
            l=o->labels.begin(); l!=o->labels.end(); l++){
        dPoint p(l->pos);
        cr->save();
        cr->move_to(p);
        if (!l->hor) cr->rotate(l->ang);
        if (l->dir == 1) cr->Cairo::Context::rel_move_to(-ext.w/2, 0);
        if (l->dir == 2) cr->Cairo::Context::rel_move_to(-ext.w, 0);
        if (pass == 1){
          cr->text_path(o->text);
          cr->stroke();
        }
        else {
          cr->show_text(o->text); // draw text
        }
        cr->restore();
      }
    }
  }
  cr->restore();
  if (label_style==LABEL_STYLE2) render_holes();
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
