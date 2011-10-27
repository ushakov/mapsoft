#ifndef WIDGETS_H
#define WIDGETS_H

#include <gtkmm.h>
#include <geo/geo_convs.h>

/********************************************************************/

template <typename ID>
class SimpleComboCols : public Gtk::TreeModelColumnRecord{
public:
  SimpleComboCols() {
    add(id);
    add(name);
  }
  Gtk::TreeModelColumn<ID> id;
  Gtk::TreeModelColumn<Glib::ustring> name;
};

template <typename ID>
class SimpleCombo : public Gtk::ComboBox {
public:
  SimpleCombo(){ }
  SimpleCombo(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) : 
    Gtk::ComboBox(cobject){ }

  void
  set_values(std::pair<ID, std::string> *ib, std::pair<ID, std::string> *ie){
    list = Gtk::ListStore::create(columns);
    std::pair<ID, std::string> *i;
    for (i=ib; i!=ie; i++){
      Gtk::TreeModel::Row row = *(list->append());
      row[columns.id]   = i->first;
      row[columns.name] = i->second;
    }
    set_model(list);
    clear();
    pack_start(columns.name);
    set_active(0);
  }

  ID get_active_id(){
    Gtk::TreeModel::iterator iter = get_active();
    if(iter){
      Gtk::TreeModel::Row row = *iter;
      return row[columns.id];
    }
    else return ID();
  }

  void set_active_id(ID id){
    Gtk::TreeNodeChildren::iterator i;
    for (i = list->children().begin(); i != list->children().end(); i++){
      if ((*i)[columns.id] != id) continue;
      set_active(i);
      return;
    }
    set_active(-1);
  }

private:
  SimpleComboCols<ID> columns;
  Glib::RefPtr<Gtk::ListStore> list;
};

/********************************************************************/

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
  SimpleCombo<Proj>  proj_cb;
  SimpleCombo<Datum> datum_cb;
  sigc::signal<void> signal_changed_;
  sigc::signal<void, dPoint> signal_jump_;
  dPoint old_pt; // to fix incorrect values

  dPoint get_xy();
  void init();
  void on_conv();
  void on_change();
  void on_jump();

};

/********************************************************************/

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
  SimpleCombo<int> rscale;
  convs::pt2pt cnv;
  sigc::signal<void, dPoint> signal_jump_;
};

/********************************************************************/
// dialog for Add/Edit Waypoint actions
class DlgWpt : public Gtk::Dialog{
    CoordBox * coord;
    Gtk::ColorButton *fg, *bg;
    Gtk::Entry *name, *comm, *alt;
    Gtk::SpinButton *fs, *ps;
    Gtk::Adjustment fs_adj, ps_adj;

  public:
    DlgWpt();

    void dlg2wpt(g_waypoint * wpt) const;
    void wpt2dlg(const g_waypoint * wpt);

    sigc::signal<void, dPoint> signal_jump();

    void set_ll(dPoint p);
};

/********************************************************************/
// dialog for Add/Edit Track actions
class DlgTrk : public Gtk::Dialog{
    Gtk::ColorButton *fg;
    Gtk::Entry *comm;
    Gtk::SpinButton *width;
    Gtk::Label *info;
    Gtk::Adjustment width_adj;
    Gtk::Label *hint;

  public:
    DlgTrk();

    void dlg2trk(g_track * trk) const;
    void trk2dlg(const g_track * trk);
    void set_info(const g_track * trk);
    void set_hint(const char * str);
};

/********************************************************************/

// dialog for Add/Edit Track actions
class DlgShowPt : public Gtk::Dialog{
    CoordBox *coord;
    NomBox   *nom;

    void jump(const dPoint p);
    sigc::signal<void, dPoint> signal_jump_;

  public:
    DlgShowPt();

    sigc::signal<void, dPoint> & signal_jump();
    void show_all(dPoint & pt);
};

/********************************************************************/
class DlgDownload : public Gtk::Dialog{
  public:

  Gtk::CheckButton *cb_w, *cb_a, *cb_o, *cb_off;
  Gtk::Entry * e_dev;

  DlgDownload();
};

/********************************************************************/
class DlgDeleteWpt : public Gtk::Dialog{
    Gtk::Label * label;
  public:
    DlgDeleteWpt();
    void set_name(const std::string & name);
};

/********************************************************************/
// dialog for Edit trackpoint action
class DlgTrkPt : public Gtk::Dialog{
    CoordBox * coord;
    Gtk::Entry *time, *alt;
    Gtk::CheckButton *start;

  public:
    DlgTrkPt();

    void dlg2tpt(g_trackpoint * tpt) const;
    void tpt2dlg(const g_trackpoint * tpt);

    sigc::signal<void, dPoint> signal_jump();
};

/********************************************************************/
// dialog for Filter track action
class DlgTrkFilter : public Gtk::Dialog{
    Gtk::CheckButton *cb_acc, *cb_num;
    Gtk::SpinButton *acc, *num;
    Gtk::Adjustment acc_adj, num_adj;
    Gtk::Label *info;
    void   toggle_acc();
    void   toggle_num();

  public:
    DlgTrkFilter();

    double get_acc() const;
    int    get_num() const;
    void set_info(const g_track * trk);

};

/********************************************************************/
// dialog for Mark track action
class DlgMarkTrk : public Gtk::Dialog{
    Gtk::CheckButton *rev;
    Gtk::SpinButton *dist;
    Gtk::Adjustment dist_adj;

  public:
    DlgMarkTrk();

    bool    get_rev() const;
    void    set_rev(const bool r);
    double  get_dist() const;
};

/********************************************************************/
// dialog for Map actions
class DlgMap : public Gtk::Dialog{
    Gtk::Entry *comm, *file;
  public:
    DlgMap();
    void dlg2map(g_map * map) const;
    void map2dlg(const g_map * map);
};








#endif