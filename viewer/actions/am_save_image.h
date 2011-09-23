#ifndef AM_SAVE_IMAGE_H
#define AM_SAVE_IMAGE_H

#include <sstream>
#include "2d/line_utils.h"

#include "action_mode.h"
#include "../generic_dialog.h"

#include "../../core/loaders/image_r.h"

class SaveImage : public ActionMode {
public:
    SaveImage (Mapview * mapview) : ActionMode(mapview) {
    }

    std::string get_name() { return "Save Image"; }

    void activate() { have_points = 0; }

    void abort() { activate(); }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
	if (have_points == 0) {
	    // make rubber
	    one = p;
	    mapview->rubber.clear();
	    mapview->rubber.add_rect(p);
	    have_points = 1;
	} else if (have_points == 1) {
	    two = p;
	    Options opt;
	    iRect bb(one, two);
	    opt.put("geom", bb);
	    opt.put("filename", "image.jpg");
	    mapview->gend.activate(get_name(), opt,
	      sigc::mem_fun(this, &SaveImage::on_result));

	    mapview->rubber.clear();
	    mapview->rubber.add_rect(one,two);
	    have_points = 0;
	}
    }

private:
    int		have_points;

    iPoint one, two;

    void on_result(int r, const Options & o) {
        mapview->rubber.clear();
        if (r != Gtk::RESPONSE_OK) return;
        iRect bb = o.get("geom", iRect(one,two));
        std::string fname=o.get("filename", std::string("image.jpg"));
        if (fname == "") fname = "image.jpg";

	// iImage image = mapview->viewer.get_image(bb);
	iImage image(bb.w, bb.h, 0xFF000000);
        mapview->workplane.draw(image, bb.TLC());
	image_r::save(image, fname.c_str(), o);

        g_map ref;
        ref.map_proj  = mapview->reference.map_proj;
        ref.file=fname;
        ref.comm="created by mapsoft_mapview";

        convs::map2pt cnv(mapview->reference, Datum("wgs84"), Proj("lonlat"));
        dLine pts   = rect2line(bb);
        dLine pts_c(pts);
        cnv.line_frw_p2p(pts_c);
        pts-=bb.TLC();
        for (int i=0; i<pts.size(); i++){
          ref.push_back(g_refpoint(pts_c[i], pts[i]));
          ref.border.push_back(pts[i]);
        }

        std::ofstream f((fname + ".map").c_str());
        oe::write_map_file(f, ref, Options());
    }
};

#endif /* AM_SAVE_IMAGE_H */
