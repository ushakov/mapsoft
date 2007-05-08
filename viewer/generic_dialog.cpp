#include <string>
#include <sigc++/sigc++.h>
#include "generic_dialog.h"


void GenericDialog::activate (std::string title, boost::shared_ptr<GenericAccessor> _accessor) {
    if (!_accessor) return;
    if (dialog_active) {
	desactivate();
    }
    dialog.reset (new Gtk::Dialog (title));
    dialog->add_button ("Close", 0);
    dialog->signal_response().connect (sigc::mem_fun (this, &GenericDialog::close_clicked));

    accessor = _accessor;
    std::vector<std::string> names = accessor->get_names();
    Gtk::Table *table = Gtk::manage(new Gtk::Table (names.size(), 2));
    dialog->get_vbox()->pack_start(*table);
    for (int i = 0; i < names.size(); ++i) {
	Gtk::Label * label = Gtk::manage(new Gtk::Label (names[i], 1.0, 0.5));
	table->attach(*label, 0,1, i,i+1);
	
	Gtk::Entry * entry = Gtk::manage (new Gtk::Entry);
	table->attach(*entry, 1,2, i,i+1);
	entry->set_editable(true);
	entry->set_text(accessor->get(names[i]));
	entry->signal_activate().connect (sigc::bind (sigc::mem_fun (this, &GenericDialog::cell_changed), names[i], entry));
    }
    dialog->show_all();
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
