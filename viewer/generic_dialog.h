#ifndef GENERIC_DIALOG_H
#define GENERIC_DIALOG_H

#include <gtkmm.h>
#include <string>
#include <boost/shared_ptr.hpp>

#include <utils/generic_accessor.h>

// A Gtk::Dialog showing and allowing to edit the supplied GenericAccessor's content
// There are two responses, 0 is OK, 1 is Cancel.
// The caller can connect to signal_response() and handle them appropriately.

class GenericDialog : public Gtk::Dialog {
public:
    GenericDialog(std::string title, GenericAccessor * accessor_)
	: Gtk::Dialog(title), accessor(accessor_)
    {
	add_button ("OK", 0);
	add_button ("Cancel", 1);

	std::vector<std::string> names = accessor->get_names();
	Gtk::Table *table = Gtk::manage(new Gtk::Table (names.size(), 2));
	get_vbox()->pack_start(*table);
	for (int i = 0; i < names.size(); ++i) {
	    Gtk::Label * label = Gtk::manage(new Gtk::Label (names[i], 1.0, 0.5));
	    table->attach(*label, 0,1, i,i+1);
	
	    Gtk::Entry * entry = Gtk::manage (new Gtk::Entry);
	    table->attach(*entry, 1,2, i,i+1);
	    entry->set_editable(true);
	    entry->set_text(accessor->get(names[i]));
	    entry->signal_activate().connect (sigc::bind (sigc::mem_fun (this, &GenericDialog::cell_changed), names[i], entry));
	}
	show_all();
    }
    void activate (std::string title, boost::shared_ptr<GenericAccessor> _accessor);
private:
    void cell_changed (std::string name, Gtk::Entry * entry);
    void close_clicked (int response);
    void desactivate ();

    bool dialog_active;
    boost::shared_ptr<GenericAccessor> accessor;
    boost::shared_ptr<Gtk::Dialog> dialog;


void GenericDialog::activate (std::string title, boost::shared_ptr<GenericAccessor> _accessor) {
}

void GenericDialog::cell_changed (std::string name, Gtk::Entry * entry) {
    accessor->set(name, entry->get_text());
    entry->set_text(accessor->get(name));
}

void GenericDialog::close_clicked (int response) {
    desactivate();
}


void GenericDialog::desactivate () {
    dialog->hide();
    dialog.reset();
    dialog_active = false;
}

};

#endif /* GENERIC_DIALOG_H */
