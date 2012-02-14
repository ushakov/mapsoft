#include "save_img.h"

DlgSaveImg::DlgSaveImg(): file_d("Save image to file"),
    page_marg_adj(0.0, 0.0, 99.0) {
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

  // Image size label
  Gtk::Label *l_size = manage(new Gtk::Label("Image Size:", Gtk::ALIGN_RIGHT));
  l_size->set_padding(3,0);

  // Image size entry
  size = manage(new Gtk::Label("", Gtk::ALIGN_LEFT));


  // Table
  Gtk::Table *table1 = manage(new Gtk::Table(3,3));
            //  widget    l  r  t  b  x       y
  table1->attach(*l_file,  0, 1, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table1->attach(*file,    1, 2, 0, 1, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK, 3, 3);
  table1->attach(*file_b,  2, 3, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table1->attach(*map,     0, 3, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table1->attach(*l_size,  0, 1, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table1->attach(*size,    1, 3, 2, 3, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK, 3, 3);

  get_vbox()->add(*table1);


  /**** 2nd frame - resolution ****/

  rb_res_screen =  manage(new Gtk::RadioButton("Screen resolution"));
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

  Gtk::Table *table2 = manage(new Gtk::Table(5,3));
            //  widget    l  r  t  b  x       y
  table2->attach(*rb_res_screen, 0, 5, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table2->attach(*rb_res_dpi,    0, 1, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table2->attach(*e_res_dpi,     1, 2, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table2->attach(*l_dpi,         2, 3, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table2->attach(*e_res_scale,   3, 4, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table2->attach(*l_scale,       4, 5, 1, 2, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK, 3, 3);
  table2->attach(*rb_res_mpp,    0, 1, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table2->attach(*e_res_mpp,     1, 2, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table2->attach(*l_mpp,         2, 5, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);

  Gtk::Frame *frame2 = manage(new Gtk::Frame("Resolution"));
  frame2->add(*table2);
  get_vbox()->add(*frame2);


  /**** 3rd frame - size ****/

  rb_size_int =  manage(new Gtk::RadioButton("Select by mouse"));
  Gtk::RadioButtonGroup gr2 = rb_size_int->get_group();
  rb_size_man  =  manage(new Gtk::RadioButton(gr2,""));
  rb_size_page =  manage(new Gtk::RadioButton(gr2,""));

  e_size_x = manage(new Gtk::Entry());
  e_size_y = manage(new Gtk::Entry());

  // Page combobox
  cb_page.set_active_id(iPoint(210,297));
  page_landsc = manage(new Gtk::CheckButton("Landscape"));
  page_marg = manage(new Gtk::SpinButton(page_marg_adj,0,1));

  Gtk::Label *l_size1 = manage(
    new Gtk::Label("x",  Gtk::ALIGN_CENTER));
  l_size1->set_padding(3,0);
  Gtk::Label *l_size2 = manage(
    new Gtk::Label("points",  Gtk::ALIGN_LEFT));
  l_size2->set_padding(3,0);
  Gtk::Label *l_page1 = manage(
    new Gtk::Label("Page: ",  Gtk::ALIGN_LEFT));
  l_page1->set_padding(3,0);
  Gtk::Label *l_page2 = manage(
    new Gtk::Label("Margins [mm]: ",  Gtk::ALIGN_LEFT));
  l_page2->set_padding(3,0);

  // Table
  Gtk::Table *table3 = manage(new Gtk::Table(6,3));
            //  widget    l  r  t  b  x       y
  table3->attach(*rb_size_int,  0, 6, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table3->attach(*rb_size_man,  0, 1, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table3->attach(*e_size_x,     1, 2, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table3->attach(*l_size1,      2, 3, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table3->attach(*e_size_y,     3, 4, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table3->attach(*l_size2,      4, 6, 1, 2, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK, 3, 3);
  table3->attach(*rb_size_page, 0, 1, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table3->attach(*l_page1,      1, 2, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table3->attach(cb_page,       2, 3, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table3->attach(*page_landsc,  3, 4, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table3->attach(*l_page2,      4, 5, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table3->attach(*page_marg,    5, 6, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);

  Gtk::Frame *frame3 = manage(new Gtk::Frame("Size"));
  frame3->add(*table3);
  get_vbox()->add(*frame3);

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
DlgSaveImg::set_size(const int w, const int h){
  std::ostringstream st;
  st << "<b>" << w << "</b> x <b>" << h << "</b> px";
  size->set_markup(st.str());
}

bool
DlgSaveImg::get_map() const{
  return map->get_active();
}
