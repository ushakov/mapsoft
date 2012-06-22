#include "nav.h"
#include "jeeps/gpscom.h"
#include "geo_io/io.h"

using namespace std;

DlgNav::DlgNav(): thread(NULL){

  signal_response().connect(
    sigc::hide(sigc::mem_fun(this, &DlgNav::hide_all)));

  signal_update.connect(
    sigc::mem_fun(this, &DlgNav::on_update));

  sw_upd  = manage(new Gtk::CheckButton("Turn on navigation mode"));
  sw_goto = manage(new Gtk::CheckButton("Auto goto"));
  sw_trk  = manage(new Gtk::CheckButton("Write track"));
  device  = manage(new Gtk::Entry());
  crd     = manage(new CoordBox);
  state_icon = manage(new Gtk::Image());

  state_icon->set_tooltip_text("Connection state");
  state_icon->set_size_request(20,16);

  Gtk::Label * devl = manage(new Gtk::Label("Device:", Gtk::ALIGN_RIGHT));

  Gtk::Table * t =   manage(new Gtk::Table(3,5));

          //  widget    l  r  t  b  x       y
  t->attach(*sw_upd,     0, 3, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*sw_goto,    0, 3, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
//  t->attach(*sw_trk,  0, 2, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*devl,       0, 1, 3, 4, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*device,     1, 2, 3, 4, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*state_icon, 2, 3, 3, 4, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*crd,        0, 3, 4, 5, Gtk::FILL, Gtk::SHRINK, 3, 3);

  get_vbox()->pack_start (*t, false, true);
  add_button (Gtk::Stock::CLOSE, Gtk::RESPONSE_CLOSE);

  string port = io::gps_detect();
  device->set_text(port);

  sw_upd->signal_toggled().connect(
    sigc::mem_fun(this, &DlgNav::on_sw_update));

  device->signal_changed().connect(
    sigc::mem_fun(this, &DlgNav::on_sw_update));

  crd->signal_jump().connect(
    sigc::mem_fun(&signal_goto_, &sigc::signal<void, dPoint>::emit));

  if (!Glib::thread_supported()) Glib::thread_init();

  pvt = new(GPS_OPvt_Data);
}

DlgNav::~DlgNav(){
  delete(pvt);
  updater_needed = false;
  thread->join(); // waiting for our thread to exit
}

void
DlgNav::updater(){

  string port = device->get_text();
  if (port==""){
    port = io::gps_detect();
    device->set_text(port);
  }

  if (GPS_Init(port.c_str()) < 0){
    cerr << "nav: can't init GPS: " << port << endl;
    state_icon->set(Gtk::Stock::NO,Gtk::ICON_SIZE_MENU);
    throw Glib::Thread::Exit();
  }
  if (GPS_Command_Pvt_On(port.c_str(), &gh)<0 || !gh) {
    state_icon->set(Gtk::Stock::NO,Gtk::ICON_SIZE_MENU);
    cerr << "nav: can't turn on pvt mode for " << port << endl;
    throw Glib::Thread::Exit();
  }
  updater_needed=true;
  while (updater_needed){
    if (GPS_Command_Pvt_Get(&gh, &pvt)>=0){
      state_icon->set(Gtk::Stock::YES,Gtk::ICON_SIZE_MENU);
      signal_update.emit();
    }
    else{
      state_icon->set(Gtk::Stock::NO,Gtk::ICON_SIZE_MENU);
      cerr << "nav: can't get pvt data" << endl;
      sleep(1);
    }
  }

  if (GPS_Command_Pvt_Off(port.c_str(), &gh)<0) {
    cerr << "nav: can't turn off pvt mode for " << port << endl;
  }
  state_icon->clear();
  throw Glib::Thread::Exit();
}

void
DlgNav::on_update(){
  dPoint p(pvt->lon, pvt->lat);
  crd->set_ll(p);
  signal_changed_.emit(p);
  if (sw_goto->get_active()) signal_goto_.emit(p);
}

void
DlgNav::on_sw_update(){

  // stop running thread:
  if (thread){
    updater_needed = false;
    thread->join();
    thread=NULL;
  }

  if (sw_upd->get_active()){
    // do nothing if wrong device selected
    if (io::check_file(device->get_text().c_str()) != 1 ||
        device->get_text() == "/dev/tty"){
      state_icon->set(Gtk::Stock::NO,Gtk::ICON_SIZE_MENU);
      return;
    }

    thread = Glib::Thread::create(
      sigc::mem_fun(*this, &DlgNav::updater), true);
  }
  else
    state_icon->clear();

}
