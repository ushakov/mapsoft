#include "save_img.h"

DlgSaveImg::DlgSaveImg(): dpi_adj(300, 1, 9999, 50),
                          mag_adj(1, 0, 9999, 0.1){
//  add_button (Gtk::Stock::PRINT, Gtk::RESPONSE_APPLY);
  add_button (Gtk::Stock::SAVE,     Gtk::RESPONSE_OK);
  add_button (Gtk::Stock::CLOSE, Gtk::RESPONSE_CANCEL);

  /****  size ****/

  pagebox = manage(new PageBox());
  pagebox->set_label("Image Size:");

  /****  resolution ****/

  Gtk::Frame *res_frame = manage(new Gtk::Frame("Map Resolution"));

  rb_mpp_screen =  manage(new Gtk::RadioButton("Screen resolution"));
  Gtk::RadioButtonGroup gr1 = rb_mpp_screen->get_group();
  rb_mpp_auto   =  manage(new Gtk::RadioButton(gr1,"Auto detect"));
  rb_mpp_dpi =  manage(new Gtk::RadioButton(gr1,"Set resolution: "));
  rb_mpp_auto->set_active();

  mpp_dpi   = manage(new Gtk::SpinButton(dpi_adj,0,1));
  scr_mag   = manage(new Gtk::SpinButton(mag_adj,0,3));
  mpp_scale = manage(new CBScale);

  Gtk::Label *l_dpi = manage(
    new Gtk::Label("dpi at",  Gtk::ALIGN_CENTER));
  l_dpi->set_padding(3,0);
  Gtk::Label *l_scale1 = manage(
    new Gtk::Label("scale:",  Gtk::ALIGN_LEFT));
  l_scale1->set_padding(3,0);
  Gtk::Label *l_scale2 = manage(
    new Gtk::Label("scale",  Gtk::ALIGN_LEFT));
  l_scale2->set_padding(3,0);

  Gtk::Table *t1 = manage(new Gtk::Table(5,3));
  t1->attach(*rb_mpp_screen, 0, 2, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t1->attach(*l_scale1,      2, 3, 0, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t1->attach(*scr_mag,       3, 4, 0, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t1->attach(*rb_mpp_auto,   0, 2, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t1->attach(*rb_mpp_dpi,    0, 1, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t1->attach(*mpp_dpi,       1, 2, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t1->attach(*l_dpi,         2, 3, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t1->attach(*mpp_scale,     3, 4, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t1->attach(*l_scale2,      4, 5, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  res_frame->add(*t1);

  /**** Map checkbutton ****/

  map = manage(new Gtk::CheckButton("write Ozi map file"));
  map->set_active();

  fig = manage(new Gtk::CheckButton("write Fig file"));
  fig->set_active(false);

  /**** Corner combobox ****/
  Gtk::Label *l_c = manage(
    new Gtk::Label("Select corner: ",  Gtk::ALIGN_RIGHT));
  corner = manage(new CBCorner());

  /*** hint ***/

  hint  = manage(new Gtk::Label);
  hint->set_line_wrap();

  /**** Main table ****/

  Gtk::Table *table = manage(new Gtk::Table(3,6));
            //  widget    l  r  t  b  x       y
  table->attach(*pagebox,   0, 3, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*res_frame, 0, 3, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*map,       0, 3, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*fig,       0, 3, 3, 4, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_c,       0, 1, 4, 5, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*corner,    1, 3, 4, 5, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*hint,      0, 3, 5, 6, Gtk::FILL, Gtk::SHRINK, 3, 3);

  get_vbox()->add(*table);

  rb_mpp_screen->signal_toggled().connect(
      sigc::mem_fun(this, &DlgSaveImg::on_ch));
  rb_mpp_dpi->signal_toggled().connect(
      sigc::mem_fun(this, &DlgSaveImg::on_ch));
  mpp_dpi->signal_changed().connect(
      sigc::mem_fun(this, &DlgSaveImg::on_ch));
  mpp_scale->signal_changed().connect(
      sigc::mem_fun(this, &DlgSaveImg::on_ch));
  scr_mag->signal_changed().connect(
      sigc::mem_fun(this, &DlgSaveImg::on_ch));
  corner->signal_changed().connect(
      sigc::mem_fun(this, &DlgSaveImg::on_ch));
  pagebox->signal_changed().connect(
      sigc::mem_fun(this, &DlgSaveImg::on_ch));

}

void
DlgSaveImg::set_px(const iPoint & p){
  pagebox->set_px(p);
}

iPoint
DlgSaveImg::get_px(){
  return pagebox->get_px();
}

int
DlgSaveImg::get_dpi(){
  return pagebox->get_dpi();
}

double
DlgSaveImg::get_scr_mag() const{
  return scr_mag->get_value();
}

double
DlgSaveImg::get_mpp() const{
  if (rb_mpp_dpi->get_active())
    return mpp_scale->get_active_id() * 2.54 /
           mpp_dpi->get_value() / 100;

  return 1;
}

int
DlgSaveImg::get_corner() const{
  return corner->get_active_id();
}

int
DlgSaveImg::get_mpp_style() const{
  if (rb_mpp_screen->get_active()) return MPP_SCREEN;
  if (rb_mpp_auto->get_active())   return MPP_AUTO;
  if (rb_mpp_dpi->get_active()) return MPP_MANUAL;

  return MPP_SCREEN;
}

void
DlgSaveImg::set_hint(const char * str){
  hint->set_markup(str);
}

void
DlgSaveImg::on_ch(){
  signal_changed_.emit();
}

sigc::signal<void> &
DlgSaveImg::signal_changed(){
  return signal_changed_;
}

