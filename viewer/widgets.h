#ifndef WIDGETS_H
#define WIDGETS_H

#include <gtkmm.h>
#include <geo/geo_convs.h>

class CoordBox : public Gtk::Frame {
public:
  CoordBox();
  CoordBox(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

  void set_ll(const dPoint &p);
  dPoint get_ll();
  sigc::signal<void> & signal_changed();
  sigc::signal<void, dPoint> & signal_jump();

private:
  Gtk::Entry coords;
  Datum datum;
  Proj proj;
  Gtk::ComboBoxText proj_cb, datum_cb;
  sigc::signal<void> signal_changed_;
  sigc::signal<void, dPoint> signal_jump_;
  dPoint old_pt; // to fix incorrect values

  dPoint get_xy();
  void init();
  void on_conv();
  void on_change();
  void on_jump();

};

class NomBox :  public Gtk::Frame {
public:
  NomBox();
  NomBox(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

  std::string get_nom() const;
  dPoint get_ll() const;
  int get_rscale();

  void set_ll(const dPoint &p);
  sigc::signal<void, dPoint> & signal_jump();

private:
  void init();

  void set_nom(const std::string &nom);
  void on_change_rscale();
  void move(int dx, int dy);

  Gtk::Entry nom;
  dPoint pt;
  Gtk::ComboBoxText rscale;
  convs::pt2pt cnv;
  std::map<std::string, int> rscales;
  sigc::signal<void, dPoint> signal_jump_;
};

#endif