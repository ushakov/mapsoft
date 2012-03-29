#ifndef WIDGETS_RAINBOW_H
#define WIDGETS_RAINBOW_H

#include <gtkmm.h>
#include "2d/rainbow.h"
#include "2d/image.h"

class Rainbow : public Gtk::Frame{
  iImage img;
  std::vector<Gtk::Label *> labels;
  Gtk::SpinButton *v1e, *v2e;
  Gtk::Adjustment v1a, v2a;
  int dg;

  void on_ch();
  sigc::signal<void> signal_changed_;

  public:
  Rainbow(
    int width, // widget width
    const rainbow_data * data, int size, // rainbow data, data size
    int l, int h, int s, int d // low, high, step, digits for spinboxes
  );
  // Only first and last values from rainbow_data used

  void set(double v1, double v2);
  double get_v1() const;
  double get_v2() const;
  sigc::signal<void> & signal_changed();
};


#endif
