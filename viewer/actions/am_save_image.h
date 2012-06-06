#ifndef AM_SAVE_IMAGE_H
#define AM_SAVE_IMAGE_H

#include <sstream>
#include "action_mode.h"
#include "../dialogs/save_img.h"

#include "2d/line_utils.h"
#include "geo_io/io_oe.h"
#include "loaders/image_r.h"



class SaveImage : public ActionMode {
public:
    SaveImage (Mapview * mapview) : ActionMode(mapview) {
      dlg.set_transient_for(*mapview);

      dlg.signal_response().connect(
        sigc::mem_fun (this, &SaveImage::on_result));
      dlg.set_title(get_name());

      fdlg.signal_response().connect(
        sigc::mem_fun (this, &SaveImage::on_fresult));
      fdlg.set_title(get_name());
      fdlg.set_filename("image.jpg");

      dlg.signal_changed().connect(
        sigc::mem_fun(this, &SaveImage::on_ch_size));
      dlg.set_hint(
         "Use <b>Image Size</b> settings to set/inspect pixel size of the image."
         " Use <b>1st mouse button</b> to set image top-left corner or"
         " <b>Ctrl + 1st mouse button</b> to set image area.");
      have_points = -1;
    }

    std::string get_name() { return "Save Image"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::SAVE_AS; }

    void activate() {
      dlg.show_all();
      if (have_points < 0) one = mapview->viewer.get_center();
      have_points = 0;
      on_ch_size();
    }

    void abort() {
      dlg.hide_all();
    }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      dlg.show_all();
      if (!mapview->have_reference){
        mapview->statusbar.push("No geo-reference", 0);
        return;
      }

      if (state&Gdk::CONTROL_MASK && (have_points == 0)){ // start 2pt area
        one = p;
        mapview->rubber.clear();
        mapview->rubber.add_rect(one);
        have_points=1;
      }
      else{
        if (have_points == 1){
          iPoint wh = pabs(p-one);
          one=(p+one-wh)/2.0;
          dlg.set_px(dPoint(wh)/get_mpp_scale());
          have_points=0;
        }
        else{
          one = p;
          on_ch_size();
        }
      }
    }

private:
    DlgSaveImg dlg;
    Gtk::FileSelection fdlg;
    int have_points;

    iPoint one;

    void on_ch_size(void) {
      mapview->rubber.clear();
      dPoint wh = dPoint(dlg.get_px())*get_mpp_scale();
      int c=dlg.get_corner();

      iPoint tlc = one;
      if ((c==1) || (c==2)) tlc.x -=wh.x;
      if ((c==2) || (c==3)) tlc.y -=wh.y;
      mapview->rubber.add_rect(tlc, tlc+iPoint(wh));
    }

    // find visible map in a given point and return mpp
    double get_mpp(const iPoint & p){
      LayerGeoMap *L;
      int i = mapview->find_map(p, &L);
      if (L==NULL) return -1;
      g_map * m = L->get_map(i);
      return  convs::map_mpp(*m, m->map_proj);
    }

    double get_mpp_scale(){
      switch (dlg.get_mpp_style()){
        case MPP_SCREEN:
          return 1.0;
        case MPP_AUTO: {
          //search top-level maps on 5x5 points on the screen and in point one
          // for max mpp
          iPoint p1=mapview->viewer.get_origin();
          iPoint p2=mapview->viewer.get_center();
          int w = 2*(p2.x-p1.x), h = 2*(p2.y-p1.y);
          double scr_mpp = convs::map_mpp(
             mapview->reference, mapview->reference.map_proj);
          double max_mpp = 0;

          iPoint p;
          for (p.y=p1.y; p.y<=p1.y+h; p.y+=h/4){
            for (p.x=p1.x; p.x<=p1.x+w; p.x+=w/4){
              double mpp = get_mpp(p);
              if (mpp>max_mpp) max_mpp = mpp;
            }
          }
          {
             double mpp = get_mpp(one);
             if (mpp>max_mpp) max_mpp = mpp;
          }

          if (max_mpp==0) return 1.0;
          return max_mpp/scr_mpp;
        }
        case MPP_MANUAL:{
          double scr_mpp = convs::map_mpp(
             mapview->reference, mapview->reference.map_proj);
          return dlg.get_mpp()/scr_mpp;
        }
      }
      return 1.0;
    }

    void on_result(int r) {

      if (!mapview->have_reference){
        mapview->statusbar.push("No geo-reference", 0);
        return;
      }

      if (r == Gtk::RESPONSE_CANCEL){
        mapview->rubber.clear();
        dlg.hide_all();
        return;
      }
      if (r == Gtk::RESPONSE_OK){
        fdlg.show_all();
        return;
      }
      if (r == Gtk::RESPONSE_APPLY){
        on_fresult(r);
        return;
      }

    }

    void on_fresult(int r) {
      if (r == Gtk::RESPONSE_CANCEL){
        fdlg.hide_all();
        return;
      }

      std::string fname=fdlg.get_filename();

      g_map mymap = mapview->reference;

      iPoint wh = dlg.get_px();
      double mpp_scale=get_mpp_scale();

      dPoint mytlc(one);
      int c = dlg.get_corner();
      if ((c==1) || (c==2)) mytlc.x -=wh.x;
      if ((c==2) || (c==3)) mytlc.y -=wh.y;

      iImage image(wh.x, wh.y, 0xFFFFFFFF);

      if (mpp_scale == 1.0){
        mapview->workplane.draw(image, mytlc);
      }
      else{
        mytlc/=mpp_scale;
        mymap/=mpp_scale;
        double old_scale = mapview->workplane.get_scale();
        mapview->workplane.set_scale(old_scale/mpp_scale);
        mapview->workplane.draw(image, mytlc);
        mapview->workplane.set_scale(old_scale);
      }

//      if (r == Gtk::RESPONSE_APPLY){
//        print_image(image, dlg.get_dpi());
//        return;
//      }

      if (image_r::save(image, fname.c_str(), Options())>0){
        mapview->dlg_err.call(MapsoftErr() << "Can't save file: " << fname);
        return;
      }

      g_map ref;
      if (dlg.get_map()){
        ref.map_proj  = mapview->reference.map_proj;
        ref.file=fname;
        ref.comm="created by mapsoft_mapview";

        convs::map2pt cnv(mymap,
                          Datum("wgs84"), Proj("lonlat"));
        dLine pts = rect2line(iRect(mytlc, iPoint(mytlc)+wh));
        dLine pts_c(pts);
        cnv.line_frw_p2p(pts_c);
        pts-=mytlc;
        for (int i=0; i<pts.size(); i++){
          ref.push_back(g_refpoint(pts_c[i], pts[i]));
          ref.border.push_back(pts[i]);
        }

        try {oe::write_map_file((fname + ".map").c_str(), ref);}
        catch (MapsoftErr e) {mapview->dlg_err.call(e);}
      }

      fdlg.hide_all();

    }
};

#endif /* AM_SAVE_IMAGE_H */
