#include <string>
#include <sigc++/sigc++.h>
#include "generic_dialog.h"
#include "../core/utils/options.h"

GenericDialog::GenericDialog (){

    add_button (Gtk::Stock::OK, 0);
    add_button (Gtk::Stock::CANCEL, 1);

    signal_response().connect(
      sigc::mem_fun (this, &GenericDialog::on_response));
    signal_delete_event().connect(
      sigc::mem_fun (this, &GenericDialog::on_delete));
    get_vbox()->pack_start(table);
}


void GenericDialog::activate (
      const std::string & title,
      const Options & _options,
      const sigc::slot1<void,int> & _on_res){


  options = _options;
  on_res = _on_res;
  set_title(title);

  table.resize(options.size(), 2);
  entries.clear();
  int k = 0;
  for (Options::iterator i = options.begin(); i != options.end(); ++i, ++k) {
    Gtk::Label * label = Gtk::manage(new Gtk::Label (i->first, 1.0, 0.5));
    table.attach(*label, 0,1, k,k+1);

    Gtk::Entry * entry = Gtk::manage (new Gtk::Entry);
    table.attach(*entry, 1,2, k,k+1);
    entry->set_editable(true);
    entry->set_text(i->second);
    entries[i->first]=entry;
  }
  show_all();
}

void GenericDialog::on_response (int response) {
  for (std::map<std::string, Gtk::Entry *>::const_iterator i=entries.begin(); i!=entries.end();i++){
    options[i->first]  = i->second->get_text();
  }
  on_res(response);
  entries.clear();
  hide_all();
}

bool GenericDialog::on_delete (GdkEventAny * e) {
  deactivate();
}

void GenericDialog::deactivate () {
  on_res(0);
  entries.clear();
  hide_all();
}
