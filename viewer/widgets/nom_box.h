#ifndef WIDGETS_NOM_BOX_H
#define WIDGETS_NOM_BOX_H

#include "comboboxes.h"
#include <geo/geo_convs.h>

/*
  Nomenclatorial map name selection widget.
  See "show point information" in mapview for example.
*/

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
  CBScale rscale;
  convs::pt2pt cnv;
  sigc::signal<void, dPoint> signal_jump_;
};

#endif
