#ifndef MTHREAD_VIEWER
#define MTHREAD_VIEWER

#include "simple_viewer.h"
#include <map>

///\addtogroup gred
///@{
///\defgroup mthread_viewer
///@{

/**
Многопотоковый вьюер. Не используется и не поддерживается!
По expose_event нужная область быстро закрашивается фоновым цветом,
после этого запускается отдельный поток, готовящий картинку из
"медленного" объекта. По мере готовности картинка дорисовывается.

Недостатки MThreadViewer:

- плитки большого размера могут рисоваться долго. Красивее рисовать
  все небольшими плитками одинакового размера раз в секунду
- не подходит для объектов, которые кэшируют информацию, относящуюся
  к запрошенным плиткам (а такие объекты мы и собираемся дальше делать)

Тупиковый объект, вряд ли его надо использовать.
*/
class MThreadViewer : public SimpleViewer {
  public:

    MThreadViewer(GObj * o);
    ~MThreadViewer();

    void updater(const iRect & r);
    void on_done_signal();
    void draw(const iRect & r);
    virtual void redraw();

  private:
    std::map<iPoint,iImage> done_cache;
    Glib::Dispatcher        done_signal;
    Glib::Mutex            *mutex;
    bool stop_drawing;
};

#endif
