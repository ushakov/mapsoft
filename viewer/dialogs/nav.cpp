#include "nav.h"
#include "jeeps/gpscom.h"
#include "geo_io/io.h"

#include <sstream>
#include <iomanip>

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
  Gtk::Label * devl = manage(new Gtk::Label("Device:", Gtk::ALIGN_RIGHT));
  devl->set_padding(3,0);

  state_btn = manage(new Gtk::Button());
  state_btn->set_tooltip_text("Press to autodetect GPS device");
  state_btn->set_size_request(16,26);

  Gtk::Image * img = manage(new Gtk::Image);
  state_btn->set_image(*img);

  Gtk::HSeparator * sep = manage(new Gtk::HSeparator());

  alt   = manage(new Gtk::Entry);
  time  = manage(new Gtk::Entry);
  crd   = manage(new CoordBox);
  Gtk::Label *altl  = manage(new Gtk::Label("Altitude:",      Gtk::ALIGN_RIGHT));
  Gtk::Label *timel = manage(new Gtk::Label("Time (UTC):", Gtk::ALIGN_RIGHT));
  altl->set_padding(3,0);
  timel->set_padding(3,0);


  Gtk::Table * t =   manage(new Gtk::Table(3,8));

          //  widget    l  r  t  b  x       y
  t->attach(*sw_upd,     0, 3, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*sw_goto,    0, 3, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
//  t->attach(*sw_trk,  0, 2, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*devl,       0, 1, 3, 4, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*device,     1, 2, 3, 4, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*state_btn,  2, 3, 3, 4, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*sep,        0, 3, 4, 5, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*altl,       0, 1, 5, 6, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*alt,        1, 3, 5, 6, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*timel,      0, 1, 6, 7, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*time,       1, 3, 6, 7, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*crd,        0, 3, 7, 8, Gtk::FILL, Gtk::SHRINK, 3, 3);

  get_vbox()->pack_start (*t, false, true);
  add_button (Gtk::Stock::CLOSE, Gtk::RESPONSE_CLOSE);

  string port = io::gps_detect();
  device->set_text(port);

  sw_upd->signal_toggled().connect(
    sigc::mem_fun(this, &DlgNav::restart_updater));

  device->signal_changed().connect(
    sigc::mem_fun(this, &DlgNav::restart_updater));

  crd->signal_jump().connect(
    sigc::mem_fun(&signal_goto_, &sigc::signal<void, dPoint>::emit));

  state_btn->signal_clicked().connect(
    sigc::mem_fun(this, &DlgNav::auto_dev));

  if (!Glib::thread_supported()) Glib::thread_init();

  pvt = new(GPS_OPvt_Data);
}

DlgNav::~DlgNav(){
  stop_updater();
  delete(pvt);
}

void
DlgNav::updater(){

  string port=device->get_text();
  if (port=="") port = io::gps_detect();

  if (GPS_Init(port.c_str()) < 0){
    cerr << "nav: can't init GPS: " << port << endl;
      updater_res=-1;
      signal_update.emit();
      throw Glib::Thread::Exit();
  }
  if (GPS_Command_Pvt_On(port.c_str(), &gh)<0 || !gh) {
    cerr << "nav: can't turn on pvt mode for " << port << endl;
    updater_res=-1;
    signal_update.emit();
    throw Glib::Thread::Exit();
  }
  updater_needed=true;
  while (updater_needed){
    int res=GPS_Command_Pvt_Get(&gh, &pvt);
    if (res>=0){
      updater_res=1;
      signal_update.emit();
    }
    else{
      updater_res=-1;
      signal_update.emit();
      cerr << "nav: can't get pvt data" << endl;
      sleep(1);
    }
  }

  if (GPS_Command_Pvt_Off(port.c_str(), &gh)<0) {
    cerr << "nav: can't turn off pvt mode for " << port << endl;
  }
  throw Glib::Thread::Exit();
}

void
DlgNav::stop_updater(){
  if (thread){
    updater_needed = false;
    thread->join();
    thread=NULL;
  }
  set_state(0);
}

void
DlgNav::restart_updater(){
  stop_updater();

  if (!sw_upd->get_active()) return;

  // do nothing if wrong device selected
  string port=device->get_text();
  if (io::check_file(port.c_str()) != 1 ||
      port == "/dev/tty"){
    set_state(-1);
    return;
  }

  thread = Glib::Thread::create(
    sigc::mem_fun(*this, &DlgNav::updater), true);
}

void
DlgNav::auto_dev(){
  string port=io::gps_detect();
  if (port!=device->get_text()){ // we need this check because if value is the same
                                 // restart_updater() will not run!
    stop_updater();
    device->set_text(io::gps_detect()); //this send us to restart_updater();
  }
}

void
DlgNav::set_state(int v){
  Gtk::Image * img=(Gtk::Image *)state_btn->get_image();
  switch (v){
    case -1: img->set(Gtk::Stock::NO, Gtk::ICON_SIZE_MENU); break;
    case  0: img->clear(); break;
    case  1: img->set(Gtk::Stock::YES,Gtk::ICON_SIZE_MENU); break;
  }
}

void
DlgNav::on_update(){

  if (!thread) return; // signal can come after thread exit

  set_state(updater_res);
  if (updater_res!=1) return;

  dPoint p(pvt->lon, pvt->lat);
  Time t(631051200 + pvt->wn_days*86400.0 + pvt->tow - pvt->leap_scnds);
  double h = pvt->alt - pvt->msl_hght;

  crd->set_ll(p);
  time->set_text(boost::lexical_cast<std::string>(t));
  std::ostringstream s;
  s.setf(std::ios::fixed);
  s << std::setprecision(1) << h << " m";
  alt->set_text(s.str());

  signal_changed_.emit(p);
  if (sw_goto->get_active()) signal_goto_.emit(p);
}

