#include "list_store_for_wpt.h"
#include "../geo_io/io.h"

class Mapview : public Gtk::Window{
  WPT_List list;
  public:
  Mapview(const g_waypoint_list & w):list(w){
    add(list);
    set_default_size(640,480);
    show_all();
  }
};

int main(int argc, char **argv){

    Gtk::Main kit (argc, argv);
    if (argc<2){
      std::cerr << "use: test1 <wpt file>\n";
      exit(0);
    }
    geo_data file;
    if (!io::in(argv[1], file, Options())){
      std::cerr << "can't read file " << argv[1] << "\n";
      exit(0);
    }
    if (file.wpts.size()<1){
      std::cerr << "no waypoints in file\n";
      exit(0);
    }

    Mapview  win(file.wpts[0]);
    kit.run(win);
}
