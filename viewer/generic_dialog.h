#ifndef GENERIC_DIALOG_H
#define GENERIC_DIALOG_H

#include <gtkmm.h>
#include <string>
#include <map>
#include <sigc++/sigc++.h>

#include "../core/utils/options.h"

// Manager of a Gtk::Dialog showing and allowing to edit the supplied Options' content
// Can register callbacks on ok/cancel events. Only one dialog can be active at a time.

class GenericDialog : public Gtk::Dialog{
public:
    GenericDialog();

    // Activates the dialog using the options supplied.
    void activate (
      const std::string & title,
      const Options & _options,
      const sigc::slot2<void, int, Options> & _on_res);

    void deactivate ();

private:
//    void cell_changed (std::string name, Gtk::Entry * entry);
    void on_response (int response);
    bool on_delete (GdkEventAny * e);

    std::map<std::string, Gtk::Entry *> entries;
    Gtk::Table table;

    sigc::slot2<void, int, Options> on_res;
};

#endif /* GENERIC_DIALOG_H */
