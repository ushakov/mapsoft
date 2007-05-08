#ifndef GENERIC_DIALOG_H
#define GENERIC_DIALOG_H

#include <gtkmm.h>
#include <string>
#include <boost/shared_ptr.hpp>

#include <utils/generic_accessor.h>

// Persistent object that shows dialogs on demand.
// For now can show only one dialog at a time.
class GenericDialog {
public:
    GenericDialog() : dialog_active(false) {}
    void activate (std::string title, boost::shared_ptr<GenericAccessor> _accessor);
private:
    void cell_changed (std::string name, Gtk::Entry * entry);
    void close_clicked (int response);
    void desactivate ();

    bool dialog_active;
    boost::shared_ptr<GenericAccessor> accessor;
    boost::shared_ptr<Gtk::Dialog> dialog;
};

#endif /* GENERIC_DIALOG_H */
