#include "mthread_viewer.h"
#include "simple_rubb_viewer.h"

class MThreadRubbViewer : public MThreadViewer, public SimpleRubbViewer {
  public:
  MThreadRubbViewer(GPlane * pl1, GPlane * pl2) :
        MThreadViewer(pl1,pl2), SimpleRubbViewer(pl1){}
};

