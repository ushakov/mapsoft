#include "../jeeps/gps.h"
#include <stdlib.h>
#include <string.h>

char *port = "usb:";

main(){
  int nw, nt, i;
  GPS_PWay *wpt, *wpt1;
  GPS_PTrack *trk;

  if (GPS_Init(port) < 0){
    fprintf(stderr, "Can't init\n");
    return;
  }
		
  if((nw=GPS_Command_Get_Waypoint(port, &wpt, NULL))<0) {
    fprintf(stderr, "can't get waypoints\n");
    return;
  }

  if((nt=GPS_Command_Get_Track(port, &trk))<0) {
    fprintf(stderr, "can't get track\n");
    return;
  }

  GPS_Fmt_Print_Waypoint(wpt, nw, stdout);
  GPS_Fmt_Print_Track(trk, nt, stdout);

/*
   wpt1 = (GPS_PWay *) calloc(1, sizeof(GPS_PWay));
   wpt1[0] = GPS_Way_New();
   strcpy(wpt1[0]->ident,"lower");
   strcpy(wpt1[0]->cmnt,"COMMENTCOMMENTCOMMENTCOMMENTCOMMENT");
   wpt1[0]->wpt_class = 0;
   wpt1[0]->lat = 1.234;
   wpt1[0]->lon = 1.234;
   GPS_Command_Send_Waypoint(port, wpt1, 1, NULL);
*/
}
