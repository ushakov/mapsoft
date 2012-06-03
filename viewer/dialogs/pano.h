#ifndef DIALOGS_PANO_H
#define DIALOGS_PANO_H

#include <gtkmm.h>
#include "gred/dthread_viewer.h"
#include "srtm/srtm3.h"
#include "layers/layer_pano.h"
#include "../widgets/rainbow.h"

// dialog for show point action
class DlgPano : public Gtk::Dialog{
    LayerPano layer_pano;
    DThreadViewer *viewer;
    Rainbow *rb;

    void on_ch();
  public:
    DlgPano(srtm3 * s);
    void show_all(const dPoint & pt);
    void set_dir(const dPoint & pt);
};

#endif
