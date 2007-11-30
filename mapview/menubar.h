#ifndef MENUBAR_H
#define MENUBAR_H

#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include <boost/lexical_cast.hpp>

#include <vector>
#include <iostream>
#include <string>

class MenuBar{

  Glib::RefPtr<Gtk::ActionGroup> actions;
  Glib::RefPtr<Gtk::UIManager> ui_manager;
  Glib::ustring ui;

  public:

  MenuBar(){
    actions = Gtk::ActionGroup::create();
    ui_manager = Gtk::UIManager::create();
  }

  // �������� ����� � ����.
  void add_item(const std::string & addr,  // ����� ���� "File/Save" ��� "Data/Create/Track"...
                const Gtk::AccelKey & accel_key, 
                const Gtk::Action::SlotActivate & slot){

    std::vector<std::string> splitted;

    // ���������� � ui - ������ � ����� �������� ������ ���������
    Glib::ustring::size_type ui_i1=0, ui_i2=ui.size(), ui_i3; 
    // ���������� � ������ - ������ � ����� �����
    std::string::size_type i1 = 0, i2 = 0; 
    
    // ������� ���������
    int level = 0;
    std::string menu_name="";
    do { // �������� �����
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

      if (i2 == addr.size()) { // ��������� ����� - menuitem
        ui_i3 = ui.find( level_str+"<menuitem action='"+menu_name+"'/>", ui_i1);
        if (ui_i3 < ui_i2) {
          std::cerr << addr << " -- already exists!\n";
          return;
        }

        ui.insert(ui_i2, level_str+"<menuitem action='"+menu_name+"'/>");
        actions->add(Gtk::Action::create(menu_name, name), accel_key, slot);
        break;
      } else {
        // ����, ���� �� ����� ����� ����
        ui_i1 = ui.find( level_str+"<menu action='"+menu_name, ui_i1);
        if (ui_i1 < ui_i2) { // ����
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

  // ������� ������� � �������� ����
  std::string get_xml(){
    std::string str = ui;
    std::string::size_type i1, i2;
    int level;

    // ������� �����+'<' �� ������� ������+'<'
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
    return "<ui>\n<menubar name='MenuBar'>\n" + str + "</menubar>\n</ui>\n";
  }

  // ������� Gtk::Widget  
  Gtk::Widget * get_widget(){
    ui_manager->insert_action_group(actions);
// ������, ��� ��� �����, ���� �� ��� ���, ���� �� - ��������� � Gtk::Window
//    add_accel_group(ui_manager->get_accel_group());
    ui_manager->add_ui_from_string(get_xml());
    return ui_manager->get_widget("/MenuBar");
  }
};

#endif
