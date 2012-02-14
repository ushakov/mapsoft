#ifndef AM_SAVE_IMAGE_H
#define AM_SAVE_IMAGE_H

#include <sstream>
#include "2d/line_utils.h"

#include "action_mode.h"
#include "../dialogs/save_img.h"

#include "../../core/loaders/image_r.h"

class SaveImage : public ActionMode {
public:
    SaveImage (Mapview * mapview) : ActionMode(mapview) {
      dlg.signal_response().connect(
        sigc::mem_fun (this, &SaveImage::on_result));
      dlg.set_title(get_name());
      dlg.signal_changed().connect(
        sigc::mem_fun(this, &SaveImage::on_ch_size));
    }

    std::string get_name() { return "Save Image"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::SAVE_AS; }

    void activate() { dlg.show_all(); have_points = 0; }

    void abort() { dlg.hide_all(); }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      if (!mapview->have_reference){
        mapview->statusbar.push("No geo-reference", 0);
        return;
      }

      mapview->rubber.clear();

      if (state&Gdk::CONTROL_MASK && (have_points == 0)){ // start 2pt area
        one = p;
        mapview->rubber.add_rect(one);
        have_points=1;
      }
      else{
        if (have_points == 1){
          iPoint wh = pabs(p-one);
          one=(p+one-wh)/2.0;
          dlg.set_px(wh);
          have_points=0;
        }
        else{
          one = p;
          mapview->rubber.add_rect(one, one+dlg.get_px());
        }
      }
    }

private:
    DlgSaveImg dlg;
    int have_points;

    iPoint one;
    iRect rect;

    void on_ch_size(void) {
      mapview->rubber.clear();
      mapview->rubber.add_rect(one, one+dlg.get_px());
    }

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
