#include "gps.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>

char *port = "/dev/ttyUSB0";

main(){
  int i;
  GPS_PPvt_Data pvt;
  gpsdevh * gh;

  if (GPS_Init(port) < 0){
    fprintf(stderr, "Can't init\n");
    return;
  }

  if(GPS_Command_Pvt_On(port, &gh)<0) {
    fprintf(stderr, "can't turn on pvt mode\n");
    return;
  }


  for (i=0; i<1000; i++){
    if (GPS_Command_Pvt_Get(&gh, &pvt)<0){
      fprintf(stderr, "can't get pvt mode\n");
      return;
    }
    printf("\nPVT:\n");
    printf(" alt: %.2f\n",             pvt->alt);
    printf(" epe: %.2f\n",             pvt->epe);
    printf(" eph: %.2f\n",             pvt->eph);
    printf(" epv: %.2f\n",             pvt->epv);
    printf(" fix: %d\n",               pvt->fix);
    printf(" tow: %.10f\n",            pvt->tow);
    printf(" lat: %.10f\n",            pvt->lat);
    printf(" lon: %.10f\n",            pvt->lon);
    printf(" east:     %.2f\n",        pvt->east);
    printf(" north:    %.2f\n",        pvt->north);
    printf(" up:       %.2f\n",        pvt->up);
    printf(" msl_hght: %.2f\n",        pvt->msl_hght);
    printf(" leap_scnds: %d\n",        pvt->leap_scnds);
    printf(" wn_days:    %d\n",        pvt->wn_days);
  }

  if (GPS_Command_Pvt_Off(port, &gh)<0) {
    fprintf(stderr, "can't turn off pvt mode\n");
    return;
  }

}
