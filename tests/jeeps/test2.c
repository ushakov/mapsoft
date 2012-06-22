#include "gps.h"
#include <stdlib.h>
#include <time.h>
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

/*  GPS_Fmt_Print_Waypoint(wpt, nw, stdout);
  GPS_Fmt_Print_Track(trk, nt, stdout);*/


for (i=0; i<nw; i++){
  printf("\nWaypoint:\n");
  printf(" ident: %s\n",             wpt[i]->ident);
  printf(" lat: %.10f\n",            wpt[i]->lat);
  printf(" lon: %.10f\n",            wpt[i]->lon);
  printf(" cmnt: %s\n",              wpt[i]->cmnt);
  printf(" dst: %x\n",               wpt[i]->dst);
  printf(" smbl: %d\n",              wpt[i]->smbl);
  printf(" dspl: %d\n",              wpt[i]->dspl);
  printf(" wpt_ident: %s\n",         wpt[i]->wpt_ident);
  printf(" lnk_ident: %s\n",         wpt[i]->lnk_ident);
  printf(" subclass: %s\n",          wpt[i]->subclass);
  printf(" colour: %d\n",            wpt[i]->colour);
  printf(" cc: %s\n",                wpt[i]->cc);
  printf(" wpt_class: %d\n",         wpt[i]->wpt_class);
  printf(" alt: %.2f\n",             wpt[i]->alt);
  printf(" city: %s\n",              wpt[i]->city);
  printf(" state: %s\n",             wpt[i]->state);
  printf(" name: %s\n",              wpt[i]->name);
  printf(" facility: %s\n",          wpt[i]->facility);
  printf(" addr: %s\n",              wpt[i]->addr);
  printf(" cross_road: %s\n",        wpt[i]->cross_road);
  printf(" attr: %d\n",              wpt[i]->attr);
  printf(" dpth: %f\n",              wpt[i]->dpth);
  printf(" idx: %d\n",               wpt[i]->idx);
  printf(" prot: %d\n",              wpt[i]->prot);
  printf(" isrte: %d\n",             wpt[i]->isrte);
  printf(" rte_prot: %d\n",          wpt[i]->rte_prot);
  printf(" rte_num: %d\n",           wpt[i]->rte_num);
  printf(" rte_cmnt: %s\n",          wpt[i]->rte_cmnt);
  printf(" rte_ident: %s\n",         wpt[i]->rte_ident);
  printf(" islink: %d\n",            wpt[i]->islink);
  printf(" rte_link_class: %d\n",    wpt[i]->rte_link_class);
  printf(" rte_link_subclass: %s\n", wpt[i]->rte_link_subclass);
  printf(" rte_link_ident: %s\n",    wpt[i]->rte_link_ident);
  printf(" Time_populated: %d\n",    wpt[i]->Time_populated);
  printf(" Time: %s\n",              ctime(&wpt[i]->Time));
}
for (i=0; i<nt; i++){
  printf("\nTrack:\n");
  printf(" ident: %s\n",             trk[i]->trk_ident);
  printf(" lat: %.10f\n",            trk[i]->lat);
  printf(" lon: %.10f\n",            trk[i]->lon);
  printf(" Time: %s\n",              ctime(&trk[i]->Time));
  printf(" alt: %.2f\n",             trk[i]->alt);
  printf(" dpth: %f\n",              trk[i]->dpth);
  printf(" heartrate: %d\n",         trk[i]->heartrate);
  printf(" tnew: %d\n",         trk[i]->tnew);
  printf(" ishdr: %d\n",         trk[i]->ishdr);
  printf(" dspl: %d\n",         trk[i]->dspl);
  printf(" colour: %d\n",         trk[i]->colour);
}
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
