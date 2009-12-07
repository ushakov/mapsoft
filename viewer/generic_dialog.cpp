#include "generic_dialog.h"

/*
There are four ways to clear dialog
 - dialog button is pressed
 - dialog window is closed
 - deactivate()
 - activate()

Before clearing on_res callback must be called.
, but only once.

*/

GenericDialog::GenericDialog (){

    add_button (Gtk::Stock::OK,     Gtk::RESPONSE_OK);
    add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    // when dialog is closed Gtk::RESPONSE_NONE response emitted

    signal_response().connect(
      sigc::mem_fun (this, &GenericDialog::on_response));

    table=NULL;
}

void GenericDialog::activate (
      const std::string & title,
      const Options & options,
      const sigc::slot2<void,int,Options> & _on_res){

  deactivate(); // deactivate old dialog

  if (table){ // we can't do it in on_response callback
    get_vbox()->remove(*table);
    delete table;
  }

  on_res = _on_res;
  set_title(title);

  table = new Gtk::Table;
  table->resize(options.size(), 2);

  entries.clear();
  int k = 0;
  for (Options::const_iterator i = options.begin(); i != options.end(); ++i, ++k) {
    Gtk::Label * label = Gtk::manage(new Gtk::Label (i->first, 1.0, 0.5));
    table->attach(*label, 0,1, k,k+1);

    Gtk::Entry * entry = Gtk::manage (new Gtk::Entry);
    table->attach(*entry, 1,2, k,k+1);
    entry->set_editable(true);
    entry->set_text(i->second);
    entries[i->first]=entry;
  }
  get_vbox()->add(*table);

  resize(1,1);
  deiconify();
  show_all();
  show_now();
std::cerr << ">>> SHOW\n";
}

// deactivate is a special case of on_response
void GenericDialog::deactivate () {
  on_response(Gtk::RESPONSE_CANCEL);
}

// run on_res callback, deactivates it, hide dialog
void GenericDialog::on_response (int response) {

  if (response==Gtk::RESPONSE_OK){
    Options options;
    for (std::map<std::string, Gtk::Entry *>::const_iterator
          i=entries.begin(); i!=entries.end();i++){
      options[i->first]  = i->second->get_text();
    }
    on_res(0, options);
  }
  else {
    on_res(1, Options());
  }
  on_res=sigc::slot2<void,int,Options>(); // we don't want to call it any more
  hide_all();
std::cerr << ">>> HIDE\n";
}

