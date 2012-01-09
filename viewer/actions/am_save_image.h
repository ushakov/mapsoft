#ifndef AM_SAVE_IMAGE_H
#define AM_SAVE_IMAGE_H

#include <sstream>
#include "2d/line_utils.h"

#include "action_mode.h"
#include "../widgets.h"

#include "../../core/loaders/image_r.h"

class SaveImage : public ActionMode {
public:
    SaveImage (Mapview * mapview) : ActionMode(mapview) {
      dlg.signal_response().connect(
        sigc::mem_fun (this, &SaveImage::on_result));
      dlg.set_title(get_name());
    }

    std::string get_name() { return "Save Image"; }

    void activate() { dlg.hide_all(); have_points = 0; }

    void abort() { activate(); }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      if (!mapview->have_reference){
        mapview->statusbar.push("No geo-reference", 0);
        return;
      }
      if (have_points == 0) {
        // make rubber
        one = p;
        mapview->rubber.clear();
        mapview->rubber.add_rect(p);
        have_points = 1;
      } else if (have_points == 1) {
        rect=iRect(one, p);
        dlg.set_file("image.jpg");
        dlg.set_size(rect.w, rect.h);
        dlg.show_all();

        mapview->rubber.clear();
        mapview->rubber.add_rect(one,p);
        have_points = 0;
      }
    }

private:
    DlgSaveImg dlg;
    int have_points;

    iPoint one;
    iRect rect;

    void on_result(int r) {
      mapview->rubber.clear();
      if (r != Gtk::RESPONSE_OK) return;

      std::string fname=dlg.get_file();
      if (fname=="") return;

      // iImage image = mapview->viewer.get_image(rect);
      iImage image(rect.w, rect.h, 0xFF000000);
      mapview->workplane.draw(image, rect.TLC());
      image_r::save(image, fname.c_str(), Options());


      if (dlg.get_map()){
        g_map ref;
        if (!mapview->have_reference){
          mapview->statusbar.push("No geo-reference", 0);
          return;
        }
        ref.map_proj  = mapview->reference.map_proj;
        ref.file=fname;
        ref.comm="created by mapsoft_mapview";

        convs::map2pt cnv(mapview->reference, Datum("wgs84"), Proj("lonlat"));
        dLine pts = rect2line(rect);
        dLine pts_c(pts);
        cnv.line_frw_p2p(pts_c);
        pts-=rect.TLC();
        for (int i=0; i<pts.size(); i++){
          ref.push_back(g_refpoint(pts_c[i], pts[i]));
          ref.border.push_back(pts[i]);
        }
        std::ofstream f((fname + ".map").c_str());
        oe::write_map_file(f, ref, Options());
      }

    }
};

#endif /* AM_SAVE_IMAGE_H */
