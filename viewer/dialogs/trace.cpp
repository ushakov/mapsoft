#include "trace.h"

Options
DlgTrace::get_opt() const{
  Options o;
  if (dtr->get_active()) o.put<bool>("down", true);
  if (utr->get_active()) o.put<bool>("down", false);
  if (pst->get_active()) o.put<bool>("point_start", true);
  if (lst->get_active()) o.put<bool>("point_start", false);
  o.put<double>("ht", ht->get_value());
  o.put<double>("at", at->get_value());
  o.put<double>("ma", ma->get_value());
  return o;
}

void
DlgTrace::set_opt(const Options & o){

  if (o.get<bool>("down", true))
       dtr->set_active();
  else utr->set_active();

  if (o.get<bool>("point_start", true))
       pst->set_active();
  else lst->set_active();

  ht->set_value(o.get<double>("ht", 200));
  at->set_value(o.get<double>("at", 10000));
  ma->set_value(o.get<double>("ma", 5));
}

DlgTrace::DlgTrace(): ht_adj(200  ,0,9999,  10,   100),
                      at_adj(10000,0,99999, 1000, 10000),
                      ma_adj(5,    0,999,   0.1,  1){

  // dialog buttons
  add_button (Gtk::Stock::CLOSE,  Gtk::RESPONSE_OK);

  // radiobuttons
  dtr =  manage(new Gtk::RadioButton("Rivers"));
  Gtk::RadioButtonGroup gr1 = dtr->get_group();
  utr =  manage(new Gtk::RadioButton(gr1, "Mountains"));
  dtr->set_active();

  // radiobuttons
  pst =  manage(new Gtk::RadioButton("Start from point"));
  Gtk::RadioButtonGroup gr2 = pst->get_group();
  lst =  manage(new Gtk::RadioButton(gr2, "Start from line"));
  pst->set_active();

  // Thresholds
  ht = manage(new Gtk::SpinButton(ht_adj,0,0));
  at = manage(new Gtk::SpinButton(at_adj,0,0));
  Gtk::Label * htl  = manage(
    new Gtk::Label("Height threshold, m", Gtk::ALIGN_LEFT));
  Gtk::Label * atl  = manage(
    new Gtk::Label("Area threshold, pt", Gtk::ALIGN_LEFT));

  set_tooltip_text(
    "Thresholds: max height and area of bumps on a traced river "
    "or a mountain range. Increasing these values slows "
    "the calculation but allows you to go through large flat areas, "
    "bridges on a river, etc. "
    "For tracing mountains larger value of Height threshold is usually "
    "needed because mountain ranges never go monotonically to the summit.\n\n"

    "You can also change area of shown lines (try 0.5 km2 for mountains "
    "and 5 km2 for rivers).\n\n"

    "Rivers/Mountains selector just invertes the algorythm "
    "to trace either rivers or mountain ranges.\n\n"

    "Line/Point start allows you to choose the starting point "
    "in a different ways. If you use Point, program goes "
    "down (up for mountains) from the point to a nearest local minimum "
    "(maximun), but not more then 5 steps, and starts tracing there. "
    "This is better for mountain tracing."
    "If you use Line, you draw a line segment across a river. Tracing starts "
    "in the minimum of this segment on the right side of it, "
    "and never crosses it."
  );

  // max area
  Gtk::Label * mal  = manage(
    new Gtk::Label("Show lines with area, km^2", Gtk::ALIGN_LEFT));
  ma = manage(new Gtk::SpinButton(ma_adj,0,2));

  Gtk::Table *t = manage(new Gtk::Table(2,7));
  t->attach(*htl,      0, 1, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*ht,       1, 2, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*atl,      0, 1, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*at,       1, 2, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*mal,      0, 1, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*ma,       1, 2, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*dtr,      0, 2, 3, 4, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*utr,      0, 2, 4, 5, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*pst,      0, 2, 5, 6, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*lst,      0, 2, 6, 7, Gtk::FILL, Gtk::SHRINK, 3, 3);
  get_vbox()->add(*t);
}

