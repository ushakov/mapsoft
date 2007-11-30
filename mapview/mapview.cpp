#include "mapview.h"

#include "viewer_am.h"
#include "file_list_am.h"
#include "wpts_list_am.h"
#include "trks_list_am.h"
#include "maps_list_am.h"

int main(int argc, char **argv){

    Gtk::Main kit (argc, argv);

    boost::shared_ptr<Mapview>  mapview;
    mapview.reset(new Mapview());

    boost::shared_ptr<ViewerAM>    viewer_am;    // ActionManager ��� viewer'�
    boost::shared_ptr<FileListAM>  file_list_am; // ActionManager ��� file_list'�
    boost::shared_ptr<WPTSListAM>  wpts_list_am; // ActionManager ��� wpts_list'�
    boost::shared_ptr<TRKSListAM>  trks_list_am; // ActionManager ��� trks_list'�
    boost::shared_ptr<MAPSListAM>  maps_list_am; // ActionManager ��� maps_list'�

    viewer_am.reset(new ViewerAM(mapview->viewer, mapview));
    file_list_am.reset(new FileListAM(mapview->file_list, mapview));
    wpts_list_am.reset(new WPTSListAM(mapview->wpts_list, mapview));
    trks_list_am.reset(new TRKSListAM(mapview->trks_list, mapview));
    maps_list_am.reset(new MAPSListAM(mapview->maps_list, mapview));

    kit.run(*mapview);
}
