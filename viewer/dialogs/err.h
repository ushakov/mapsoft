#ifndef DIALOGS_ERR_H
#define DIALOGS_ERR_H

#include <gtkmm.h>
#include "utils/err.h"

// dialog for error messages
class DlgErr : public Gtk::MessageDialog{
  public:
    DlgErr();
    void call(const MapsoftErr & e);
};

#endif
