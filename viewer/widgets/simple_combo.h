#ifndef WIDGETS_SIMPLE_COMBO_H
#define WIDGETS_SIMPLE_COMBO_H

#include <gtkmm.h>

/*
  ComboBox with entries consists of two parts:
  - template type ID
  - string text for the interface
*/

template <typename ID>
class SimpleComboCols : public Gtk::TreeModelColumnRecord{
public:
  SimpleComboCols() {
    add(id);
    add(name);
  }
  Gtk::TreeModelColumn<ID> id;
  Gtk::TreeModelColumn<Glib::ustring> name;
};

template <typename ID>
class SimpleCombo : public Gtk::ComboBox {
public:
  SimpleCombo(){ }
  SimpleCombo(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) :
    Gtk::ComboBox(cobject){ }

  void
  set_values(std::pair<ID, std::string> *ib, std::pair<ID, std::string> *ie){
    list = Gtk::ListStore::create(columns);
    std::pair<ID, std::string> *i;
    for (i=ib; i!=ie; i++){
      Gtk::TreeModel::Row row = *(list->append());
      row[columns.id]   = i->first;
      row[columns.name] = i->second;
    }
    set_model(list);
    clear();
    pack_start(columns.name);
    set_active(0);
  }

  ID get_active_id(){
    Gtk::TreeModel::iterator iter = get_active();
    if(iter){
      Gtk::TreeModel::Row row = *iter;
      return row[columns.id];
    }
    else return ID();
  }

  void set_active_id(ID id){
    Gtk::TreeNodeChildren::iterator i;
    for (i = list->children().begin(); i != list->children().end(); i++){
      if ((*i)[columns.id] != id) continue;
      set_active(i);
      return;
    }
    set_active(-1);
  }

private:
  SimpleComboCols<ID> columns;
  Glib::RefPtr<Gtk::ListStore> list;
};

#endif
