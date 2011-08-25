#ifndef AM_ADD_TRACK_H
#define AM_ADD_TRACK_H

#include <sstream>

#include "action_mode.h"
#include "../generic_dialog.h"

class AddTrack : public ActionMode {
public:
    AddTrack (Mapview * mapview) : ActionMode(mapview) { }

    // Returns name of the mode as string.
    std::string get_name() {
	return "Add Track";
    }

    // Activates this mode.
    void activate() {
      new_track.clear();
    }

    // Abandons any action in progress and deactivates mode.
    void abort() {
      activate();
    }

    // Sends user click. Coordinates are in workplane's discrete system.
    void handle_click(iPoint p, const Gdk::ModifierType & state) {
         if (!mapview->have_reference){
           mapview->statusbar.push("No geo-reference", 0);
           return;
         }

         if (new_track.size() == 0){
           Options opt = new_track.to_options();
           mapview->gend.activate(get_name(), opt,
             sigc::mem_fun(this, &AddTrack::on_result));
         }

	/// add or remove point
        if (state&Gdk::CONTROL_MASK){
	  std::cout << "remove track point" << std::endl;
          if (new_track.size()>0) new_track.pop_back();
          if (mapview->rubber.size()>0){
            mapview->rubber.pop();
          }
          if (mapview->rubber.size()>0){
            RubberSegment s = mapview->rubber.pop();
            s.flags |= RUBBFL_MOUSE_P2;
            s.p2=iPoint(0,0);
            mapview->rubber.add(s);
          }
        }
        else{
	  std::cout << "add track point: " << p << std::endl;
          if (mapview->rubber.size()>0){
            RubberSegment s = mapview->rubber.pop();
            s.flags &= ~RUBBFL_MOUSE;
            s.p2 = p;
            mapview->rubber.add(s);
          }
          mapview->rubber.add_diag(p);
          g_map map = mapview->reference;
          convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"));

          g_trackpoint pt;
          pt.x = p.x; pt.y=p.y;
  	  cnv.frw(pt);
 	  new_track.push_back(pt);
        }

	std::ostringstream st;
	st << "Add track: "
           << new_track.size() << " points, "
           << new_track.length()/1000 << " km";
	mapview->statusbar.push(st.str(),0);
    }

private:
    g_track  new_track;

    void on_result(int r, const Options & o) {
       if (r == 0) { // OK
          new_track.parse_from_options(o);
          boost::shared_ptr<geo_data> world(new geo_data);
          world->trks.push_back(new_track);
          mapview->add_world(world, new_track.comm, false);
       }
       mapview->statusbar.push("",0);
       new_track.clear();
       mapview->rubber.clear();
    }

};

#endif /* AM_ADD_TRACK_H */
