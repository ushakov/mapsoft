#ifndef AM_SAVE_IMAGE_H
#define AM_SAVE_IMAGE_H

#include <sstream>
#include "action_mode.h"
#include "../dialogs/save_img.h"

#include "geo_io/geofig.h"
#include "2d/line_utils.h"
#include "geo_io/io_oe.h"
#include "loaders/image_r.h"



class SaveImage : public ActionMode {
public:

    SaveImage (Mapview * mapview) : ActionMode(mapview) {
      /* save image dialog */
      dlg.set_transient_for(*mapview);
      dlg.set_title(get_name());
      dlg.signal_response().connect(
        sigc::mem_fun (this, &SaveImage::on_result));
      dlg.signal_changed().connect(
        sigc::mem_fun(this, &SaveImage::ch_rubber));
      dlg.set_hint(
         "Use <b>Image Size</b> settings to set/inspect pixel size of the image."
         " Use <b>1st mouse button</b> to set image top-left corner or"
         " <b>Ctrl + 1st mouse button</b> to set image area.");

      /* file selection dialog */
      fdlg.set_transient_for(*mapview);
      fdlg.set_title(get_name());
      fdlg.set_filename("image.jpg");
      fdlg.signal_response().connect(
        sigc::mem_fun (this, &SaveImage::on_fresult));
      have_points = -1;

      /* change point on viewer rescaling. Mayby its better to keep
         points in wgs coords instead... */
      mapview->viewer.signal_on_rescale().connect(
        sigc::mem_fun (this, &SaveImage::on_rescale));
    }

    std::string get_name() { return "Save Image"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::SAVE_AS; }

    void activate() {
      dlg.show_all();
      if (have_points < 0) one = mapview->viewer.get_center();
      have_points = 0;
      ch_rubber();
    }

    void abort() {
      dlg.hide_all();
    }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      dlg.show_all();

      /* no reference*/
      if (!mapview->have_reference){
        mapview->spanel.message("No geo-reference");
        return;
      }

      /* start rubber for 2pt area selection */
      if (state&Gdk::CONTROL_MASK && (have_points == 0)){ 
        one = p;
        mapview->rubber.clear();
        mapview->rubber.add_rect(one);
        have_points=1;
        return;
      }

      /* finish 2pt area selection */
      if (have_points == 1){ // finish 2pt area
        iPoint wh = pabs(p-one); //< size
        one=(p+one-wh)/2.0;      //< top-left corner
        // convert to correct corner:
        int c=dlg.get_corner();
        if ((c==1) || (c==2)) one.x +=wh.x;
        if ((c==2) || (c==3)) one.y +=wh.y;
        // set new size to dialog (ch_rubber called by signal):
        dlg.set_px(dPoint(wh)/get_mpp_scale());
        have_points=0;
        return;
      }

      /* single click - move area with current size */
      one = p;
      ch_rubber();
    }

private:
    DlgSaveImg dlg;
    Gtk::FileSelection fdlg;
    int have_points;

    iPoint one;

    /* This function is called when selected area is changed
       by changing dialog settings, or mouse selection.
     */
    void ch_rubber(void) {
      mapview->rubber.clear();
      dPoint wh = dPoint(dlg.get_px())*get_mpp_scale();
      int c=dlg.get_corner();
      iPoint tlc = one;
      if ((c==1) || (c==2)) tlc.x -=wh.x;
      if ((c==2) || (c==3)) tlc.y -=wh.y;
      mapview->rubber.add_rect(tlc, tlc+iPoint(wh));
    }

    void on_rescale(double sc) {
      one=dPoint(one)*sc;
    }

    // find visible map in a given point and return scale
    double get_sc(const iPoint & p){
      GObjMAP *L;
      int i = mapview->panel_maps.find_map(p, &L);
      if (L==NULL) return -1;
      return L->scale(p, i);
    }

    double get_scr_mpp(const iPoint & p){
      dPoint pp[3] = {p, p + iPoint(1000,0), p + iPoint(0,1000)};
      for (int i=0; i<3; i++){
        mapview->get_cnv()->frw(pp[i]); // convert to wgs
        pp[i].x *= cos(pp[i].y /180*M_PI);
        pp[i] *= 6380000/180*M_PI;
      }
      return std::max(pdist(pp[1],pp[0]), pdist(pp[2],pp[0]))/1000;
    }

    /* get rescaling factor */
    double get_mpp_scale(){
      switch (dlg.get_mpp_style()){
        case MPP_SCREEN:
          return 1./dlg.get_scr_mag();
        case MPP_AUTO: {
          //search top-level maps on 5x5 points on the screen and in point one
          // for max mpp
          iPoint p1=mapview->viewer.get_origin();
          iPoint p2=mapview->viewer.get_center();
          int w = 2*(p2.x-p1.x), h = 2*(p2.y-p1.y);

          double max_sc = 0;

          iPoint p;
          for (p.y=p1.y; p.y<=p1.y+h; p.y+=h/4){
            for (p.x=p1.x; p.x<=p1.x+w; p.x+=w/4){
              double sc = get_sc(p);
              if (sc>max_sc) max_sc = sc;
            }
          }
          {
             double sc = get_sc(one);
             if (sc>max_sc) max_sc = sc;
          }

          if (max_sc==0) return 1.0/dlg.get_scr_mag();
          return max_sc / dlg.get_scr_mag();
        }
        case MPP_MANUAL:{
          return dlg.get_mpp()/get_scr_mpp(one);
        }
      }
      return 1.0;
    }

    /* On dialog response: close on cancel, show file
       selection dialog on ok. */
    void on_result(int r) {
      if (r == Gtk::RESPONSE_CANCEL){
        mapview->rubber.clear();
        dlg.hide_all();
        return;
      }
      if (r == Gtk::RESPONSE_OK){
        fdlg.show_all();
        return;
      }
/* // this is reserved for print button
      if (r == Gtk::RESPONSE_APPLY){
        on_fresult(r);
        return;
      }
*/
    }

    /* On save button pressed in file selection dialog */
    void on_fresult(int r) {
      if (r == Gtk::RESPONSE_CANCEL){
        fdlg.hide_all();
        return;
      }

      std::string fname=fdlg.get_filename();

      iPoint wh = dlg.get_px();
      double sc=get_mpp_scale();

std::cerr << "MPP SCALE " << sc << "\n";
      dPoint mytlc(one);
      int c = dlg.get_corner();
      if ((c==1) || (c==2)) mytlc.x -= wh.x*sc;
      if ((c==2) || (c==3)) mytlc.y -= wh.y*sc;

      iImage image(wh.x, wh.y, 0xFFFFFFFF);

      if (sc == 1.0){
        mapview->main_gobj.draw(image, mytlc);
      }
      else{
        mytlc/=sc;
        mapview->main_gobj.rescale(1/sc);
//        mapview->main_gobj.refresh();
        mapview->main_gobj.draw(image, mytlc);
        mapview->main_gobj.rescale(sc);
//        mapview->main_gobj.refresh();
      }

//      if (r == Gtk::RESPONSE_APPLY){
//        print_image(image, dlg.get_dpi());
//        return;
//      }

      // write image file
      if (image_r::save(image, fname.c_str())>0){
        mapview->dlg_err.call(MapsoftErr() << "Can't save file: " << fname);
        return;
      }

      // Write map and fig files
      if (dlg.get_map() || dlg.get_fig()){
        g_map ref = *mapview->main_gobj.get_ref();
        ref.clear(); ref.border.clear();

        // remove path from fname
        int pos = fname.rfind("/");
        if (pos>0 && pos<fname.length()-1)
          ref.file = fname.substr(pos+1, fname.length()-pos);
        else ref.file=fname;

        ref.comm="created by mapsoft_mapview";

        dLine pts = rect2line(iRect(mytlc, iPoint(mytlc)+wh));
        dLine pts_c(pts);
        pts_c*=sc;
        mapview->get_cnv()->line_frw_p2p(pts_c);
        pts-=mytlc;
        for (int i=0; i<pts.size(); i++){
          ref.push_back(g_refpoint(pts_c[i], pts[i]));
          ref.border.push_back(pts[i]);
        }

        if (dlg.get_map()){
          try {oe::write_map_file((fname + ".map").c_str(), ref);}
          catch (MapsoftErr e) {mapview->dlg_err.call(e);}
        }
        if (dlg.get_fig()){
          fig::fig_world W;
          ref = ref * 2.54 / 300 * fig::cm2fig;
          fig::set_ref(W, ref, Options());
          fig::fig_object o = fig::make_object("2 5 0 1 0 -1 500 -1 -1 0.000 0 0 -1 0 0 *");
          for (g_map::iterator i=ref.begin(); i!=ref.end(); i++){
            o.push_back(iPoint(int(i->xr), int(i->yr)));
          }
          o.push_back(iPoint(int(ref[0].xr), int(ref[0].yr)));
          o.image_file = fname;
          o.comment.push_back(string("MAP ") + fname);
          W.push_back(o);
          try {fig::write((fname + ".fig").c_str(), W);}
          catch (MapsoftErr e) {mapview->dlg_err.call(e);}
        }
      }
      fdlg.hide_all();
    }
};

#endif /* AM_SAVE_IMAGE_H */
