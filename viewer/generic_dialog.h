#ifndef GENERIC_DIALOG_H
#define GENERIC_DIALOG_H

#include <gtkmm.h>
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

#include "../core/utils/options.h"

// Manager of a Gtk::Dialog showing and allowing to edit the supplied Options' content
// Can register callbacks on ok/cancel events. Only one dialog can be active at a time.

class GenericDialog {
public:
    // Activates the dialog using the options supplied.
    void activate (std::string title, Options const & _options);

    // Gets the (modified by user) options. Can be called at any time, but useful
    // only if signal_result already fired with result == 0.
    Options get_options() { return options; }

    // A void f(int) callback that fires when the dialog's result is available.
    // Result is: 0 -- ok, 1 -- cancel.
    sigc::signal<void, int> & signal_result() {
	return m_signal_result;
    }

    static GenericDialog * get_instance() {
	if (!instance) {
	    instance = new GenericDialog;
	}
	return instance;
    }

    void deactivate ();
    
protected:
    // This should actually be a singleton, so the constructor is protected
    GenericDialog()
    { }

private:
//    void cell_changed (std::string name, Gtk::Entry * entry);
    void on_response (int response);
    bool on_delete (GdkEventAny * e);

    static GenericDialog * instance;

    boost::shared_ptr<Gtk::Dialog> dialog;
    Options options;

    std::map<std::string, Gtk::Entry *> entries;

    sigc::signal<void, int> m_signal_result;
};

#endif /* GENERIC_DIALOG_H */
