#ifndef MENUBAR_H
#define MENUBAR_H

//#include <gtkmm.h>
//#include <sigc++/sigc++.h>

#include <boost/lexical_cast.hpp>

#include <vector>
#include <iostream>
#include <string>

class MenuBar{
//  Glib::RefPtr<Gtk::ActionGroup> actions;
//  Glib::ustring ui;
  std::string ui;
  int maxitem;

  std::vector<std::string> addrs;

  public:

  MenuBar(){
//    actions = Gtk::ActionGroup::create();
    maxitem = 0;
  }

  // добавить пункт в меню.
  void add_item(const std::string & addr/*,  // адрес вида "File/Save" или "Data/Create/Track"...
                const AccelKey& accel_key, 
                const Action::SlotActivate& slot*/){

    std::vector<std::string> splitted;

    // координаты в ui - начало и конец текущего уровня вложности
//    Glib::ustring::size_type ui_i1=0, ui_i2=ui.size(); 
    std::string::size_type ui_i1=0, ui_i2=ui.size(), ui_i3; 
    // координаты в адресе - начало и конец имени
    std::string::size_type i1 = 0, i2 = 0; 
    
    // уровень вложности
    int level = 0;
    std::string menu_name="";
    do { // разберем адрес
      std::string level_str = boost::lexical_cast<std::string>(level);
      i2 = addr.find("/", i1);
      if (i2 == std::string::npos) i2 = addr.size();
      std::string name = addr.substr(i1, i2-i1);

      if (name == "") {
        std::cerr << addr << " -- empty action name!\n";
        return;
      }
      if (menu_name!="") menu_name+="-";
      menu_name+=name;

      if (i2 == addr.size()) { // последний пункт - menuitem
        ui_i3 = ui.find( level_str+"<menuitem action='"+menu_name+"'/>", ui_i1);
        if (ui_i3 < ui_i2) {
          std::cerr << addr << " -- already exists!\n";
          return;
        }

        ui.insert(ui_i2, level_str+"<menuitem action='"+menu_name+"'/>");
//        actions->add(Gtk::Action::create("n" + boost::lexical_cast<std::string>(addrs.size()), name),
//        accel_key, slot);
        break;
      } else {
        // ищем, есть ли такой пункт меню
        ui_i1 = ui.find( level_str+"<menu action='"+menu_name, ui_i1);
//        if (ui_i1 != Glib::ustring::npos) { // есть
        if (ui_i1 < ui_i2) { // есть
          ui_i3 = ui.find( level_str+"</menu>", ui_i1);
          if (ui_i3 > ui_i2) { 
            std::cerr << "unclosed <menu action=" << menu_name << ">\n";
            return;
          }
          ui_i1 += 17+menu_name.size();
          ui_i2 = ui_i3;
        } else {
          ui_i1 = ui_i2;
          ui.insert(ui_i1, level_str+"<menu action='"+menu_name+"'>"+level_str+"</menu>");
          ui_i1 += 17+menu_name.size();
          ui_i2 = ui_i1;
        }
      }

      i1=i2+1;
      level++;
    }while (true);

  }


  std::string get_xml(){

    std::string str = ui;
    std::string::size_type i1, i2;
    int level;
    
    // заменим число+'<' на перевод строки+'<'
    for (i1 = 0; i1<str.size(); i1 = str.find_first_of("0123456789", i1)){
      i2 = str.find_first_not_of("0123456789", i1);
      if (i2 == std::string::npos) break;
      if (str[i2] != '<') {i1 = i2; continue;}
      int level = atoi(str.substr(i1, i2 - i1).c_str());
      std::string subst = "\n";
      subst.resize(1+level, ' ');
      str.replace(i1, i2 - i1, subst);
      i1 = i2;
    }
    return str + "\n";
  }

/*  // создать Gtk::Widget  
  Glib::RefPtr<Gtk::Widget> get_widget(){
    Glib::RefPtr<Gtk::UIManager> ui_manager = Gtk::UIManager::create();
    ui_manager->insert_action_group(actions);
    add_accel_group(ui_manager->get_accel_group());

    ui_manager->add_ui_from_string(ui);
    return ui_manager->get_widget("/MenuBar");
  }

    // разбор адреса
    std::vector<std::string> splitted;
    std::string::size_type i1 = 0, i2 = 0;
    std::string delim = "/";
    do {
      i2 = addr.find(delim, i1);
      if (i2 == std::string::npos) i2 = addr.size();
      splitted.push_back(addr.substr(i1, i2-i1));
      i1=i2+delim.size();
    }while (i2!=addr.size());

*/
};

#endif
