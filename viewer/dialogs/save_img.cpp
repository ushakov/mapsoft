#include "save_img.h"

DlgSaveImg::DlgSaveImg(): file_d("Save image to file"),
                          dpi_adj(300, 0, 9999, 50){
  add_button (Gtk::Stock::OK,     Gtk::RESPONSE_OK);
  add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  signal_response().connect(
      sigc::hide(sigc::mem_fun(this, &DlgSaveImg::hide_all)));

  /**** 1st table - file settings ****/

  // file label
  Gtk::Label *l_file = manage(
    new Gtk::Label("File name (jpg, tiff or png):",  Gtk::ALIGN_RIGHT));
  l_file->set_padding(3,0);

  // file entry
  file = manage(new Gtk::Entry());
  file->set_text("image.jpg");

  // File selection dialog
  file_d.signal_response().connect(
        sigc::mem_fun (this, &DlgSaveImg::on_file_ch));
  file_d.set_filename(get_file());

  // File selection button
  Gtk::Button *file_b= manage(new Gtk::Button("Select..."));
  file_b->signal_clicked().connect(
    sigc::mem_fun (&file_d, &Gtk::FileSelection::show));
  file_b->signal_hide().connect(
    sigc::mem_fun (&file_d, &Gtk::FileSelection::hide));

  // Map checkbutton
  map = manage(new Gtk::CheckButton("write Ozi map file"));
  map->set_active();

  /****  size ****/

  pagebox = manage(new PageBox());
  pagebox->set_label("Image Size:");

  /****  resolution ****/

  Gtk::Frame *res_frame = manage(new Gtk::Frame("Map Resolution"));

  rb_mpp_screen =  manage(new Gtk::RadioButton("Screen resolution"));
  Gtk::RadioButtonGroup gr1 = rb_mpp_screen->get_group();
  rb_mpp_auto   =  manage(new Gtk::RadioButton(gr1,"Auto detect"));
  rb_mpp_dpi =  manage(new Gtk::RadioButton(gr1,"Set resolution: "));

  mpp_dpi   = manage(new Gtk::SpinButton(dpi_adj));
  mpp_scale = manage(new CBScale);

  Gtk::Label *l_dpi = manage(
    new Gtk::Label("dpi at",  Gtk::ALIGN_CENTER));
  l_dpi->set_padding(3,0);
  Gtk::Label *l_scale = manage(
    new Gtk::Label("scale",  Gtk::ALIGN_LEFT));
  l_scale->set_padding(3,0);

  Gtk::Table *t1 = manage(new Gtk::Table(5,3));
  t1->attach(*rb_mpp_screen, 0, 5, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t1->attach(*rb_mpp_auto,   0, 5, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t1->attach(*rb_mpp_dpi,    0, 1, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t1->attach(*mpp_dpi,       1, 2, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t1->attach(*l_dpi,         2, 3, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t1->attach(*mpp_scale,     3, 4, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t1->attach(*l_scale,       4, 5, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  res_frame->add(*t1);

  /*** hint ***/
  hint  = manage(new Gtk::Label);
  hint->set_line_wrap();

  /**** Main table ****/
  Gtk::Table *table = manage(new Gtk::Table(3,5));
            //  widget    l  r  t  b  x       y
  table->attach(*l_file,  0, 1, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*file,    1, 2, 0, 1, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*file_b,  2, 3, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*map,     0, 3, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*pagebox,   0, 3, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*res_frame, 0, 3, 3, 4, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*hint,    0, 3, 4, 5, Gtk::FILL, Gtk::SHRINK, 3, 3);

  get_vbox()->add(*table);

  rb_mpp_screen->signal_toggled().connect(
      sigc::mem_fun(this, &DlgSaveImg::on_ch));
  rb_mpp_dpi->signal_toggled().connect(
      sigc::mem_fun(this, &DlgSaveImg::on_ch));
  mpp_dpi->signal_changed().connect(
      sigc::mem_fun(this, &DlgSaveImg::on_ch));
  mpp_scale->signal_changed().connect(
      sigc::mem_fun(this, &DlgSaveImg::on_ch));
  pagebox->signal_changed().connect(
      sigc::mem_fun(this, &DlgSaveImg::on_ch));

}

std::string
DlgSaveImg::get_file() const{
  return file->get_text();
}

void
DlgSaveImg::set_file(const std::string & f){
  file->set_text(f);
}

void
DlgSaveImg::on_file_ch(int response){
  if (response==Gtk::RESPONSE_OK)
    file->set_text(file_d.get_filename());
  file_d.hide();
}


void
DlgSaveImg::set_px(const iPoint & p){
  pagebox->set_px(p);
}

iPoint
DlgSaveImg::get_px(){
  return pagebox->get_px();
}

bool
DlgSaveImg::get_map() const{
  return map->get_active();
}
double
DlgSaveImg::get_mpp(){
  if (rb_mpp_dpi->get_active())
    return mpp_scale->get_active_id() * 2.54 /
           mpp_dpi->get_value() / 100;

  return 1;
}

int
DlgSaveImg::get_mpp_style(){
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

