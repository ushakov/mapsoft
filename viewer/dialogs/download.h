#ifndef DIALOGS_DOWNLOAD_H
#define DIALOGS_DOWNLOAD_H

#include <gtkmm.h>

class DlgDownload : public Gtk::Dialog{
  public:

  Gtk::CheckButton *cb_w, *cb_a, *cb_o, *cb_off;
  Gtk::Entry * e_dev;

  DlgDownload();
};


#endif
