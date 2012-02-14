#include "save_img.h"

DlgSaveImg::DlgSaveImg(): file_d("Save image to file"){
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

  /**** 2nd frame - resolution ****/

  Gtk::Frame *res_frame = manage(new Gtk::Frame("Map resolution"));

  rb_res_screen =  manage(new Gtk::RadioButton("Screen resolution"));
/*
  Gtk::RadioButtonGroup gr1 = rb_res_screen->get_group();
  rb_res_dpi =  manage(new Gtk::RadioButton(gr1,""));
  rb_res_mpp =  manage(new Gtk::RadioButton(gr1,""));

  e_res_dpi   = manage(new Gtk::Entry);
  e_res_scale = manage(new Gtk::Entry);
  e_res_mpp   = manage(new Gtk::Entry);
  Gtk::Label *l_dpi = manage(
    new Gtk::Label("dpi @",  Gtk::ALIGN_CENTER));
  l_dpi->set_padding(3,0);
  Gtk::Label *l_scale = manage(
    new Gtk::Label("scale",  Gtk::ALIGN_LEFT));
  l_scale->set_padding(3,0);
  Gtk::Label *l_mpp = manage(
    new Gtk::Label("mpp",  Gtk::ALIGN_LEFT));
  l_mpp->set_padding(3,0);

  table = manage(new Gtk::Table(5,3));
            //  widget    l  r  t  b  x       y
  table->attach(*rb_res_screen, 0, 5, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*rb_res_dpi,    0, 1, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*e_res_dpi,     1, 2, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_dpi,         2, 3, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*e_res_scale,   3, 4, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_scale,       4, 5, 1, 2, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*rb_res_mpp,    0, 1, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*e_res_mpp,     1, 2, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_mpp,         2, 5, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
*/
  res_frame->add(*rb_res_screen);

  /**** 3rd frame - size ****/

  pagebox = manage(new PageBox());
  pagebox->set_label("Image size:");


  // Table
  Gtk::Table *table = manage(new Gtk::Table(3,4));
            //  widget    l  r  t  b  x       y
  table->attach(*l_file,  0, 1, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*file,    1, 2, 0, 1, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*file_b,  2, 3, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*map,     0, 3, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*res_frame, 0, 3, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*pagebox,   0, 3, 3, 4, Gtk::FILL, Gtk::SHRINK, 3, 3);

  get_vbox()->add(*table);

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

sigc::signal<void> &
DlgSaveImg::signal_changed(){
  return pagebox->signal_changed();
}
