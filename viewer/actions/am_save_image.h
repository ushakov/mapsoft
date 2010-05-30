#ifndef AM_SAVE_IMAGE_H
#define AM_SAVE_IMAGE_H

#include <sstream>

#include "action_mode.h"
#include "../generic_dialog.h"
#include "../mapview.h"

#include "../../core/loaders/image_r.h"

class SaveImage : public ActionMode {
public:
    SaveImage (Mapview * mapview_) : mapview(mapview_) {
    }

    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "Save Image";
    }

    // Activates this mode.
    virtual void activate() {
      have_points = 0;
    }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() { }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p, const Gdk::ModifierType & state) {
	std::cout << "SAVE IMAGE: " << p << " points: " << have_points << std::endl;
	if (have_points == 0) {
	    one = p;
	    have_points = 1;
	    // make rubber
	    mapview->rubber.clear();
	    mapview->rubber.add_rect(p);
	} else if (have_points == 1) {
	    two = p;
	    Options opt;
	    iRect bb(one, two);
	    opt.put("geom", bb);
	    opt.put("filename", "image.jpg");
	    mapview->rubber.clear();
	    mapview->gend.activate(get_name(), opt,
	      sigc::mem_fun(this, &SaveImage::on_result));
	}
    }

private:
    Mapview   * mapview;
    int		have_points;

    iPoint one, two;

    void on_result(int r, const Options & o) {
	have_points = 0;
        if (r == 0) { // OK
          iRect bb = o.get("geom", iRect(one,two));
          std::string fname=o.get("filename", std::string("image.jpg"));
          if (fname == "") fname = "image.jpg";

	  iImage image = mapview->viewer.get_image(bb);
	  image_r::save(image, fname.c_str(), o);

          g_map ref;
          ref.map_proj  = mapview->reference.map_proj;
          ref.file=fname;
          ref.comm="created by mapsoft_mapview";

          g_point pts[4] = {bb.TLC(), bb.TRC(), bb.BRC(), bb.BLC()};
          convs::map2pt cnv(mapview->reference, Datum("wgs84"), Proj("lonlat"));
          for (int i=0; i<4; i++){
            g_point p = pts[i]; cnv.frw(p);
            ref.push_back(g_refpoint(p, pts[i] - (g_point)bb.TLC()));
            ref.border.push_back(pts[i]);
          }

	  std::ofstream f((fname + ".map").c_str());
          oe::write_map_file(f, ref, Options());
	}
    }
};

#endif /* AM_SAVE_IMAGE_H */
