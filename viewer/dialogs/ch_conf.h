#ifndef DIALOGS_CH_CONF_H
#define DIALOGS_CH_CONF_H

#include <gtkmm.h>

// dialog for file changed confirmation
class DlgChConf : public Gtk::MessageDialog{
  public:
    DlgChConf();
    void call(const sigc::slot<void> & slot);

  private:
    sigc::signal<void> signal_ok_;
    sigc::slot<void> current_slot;
    void on_ok();
};

#endif
