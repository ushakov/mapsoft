#include <stdio.h>
#include <proj_api.h>

int main(){

//  char *pr1[] = {"+proj=latlong", "+ellps=WGS84"};
  char *pr1[] = {"+proj=tmerc",   "+ellps=WGS84", "+lon_0=39",
                 "+scale=1", "+lat_0=0", "+x_0=500000"};
  char *pr2[] = {"+proj=tmerc",   "+ellps=krass", "+lon_0=39",
                 "+scale=1", "+lat_0=0", "+x_0=500000"};


  projPJ P1 = pj_init(6, pr1);
  projPJ P2 = pj_init(6, pr2);


  if (!P1) {
    fprintf(stderr, "can't create projection 1\n");
    exit(1);
  }

  if (!P2) {
    fprintf(stderr, "can't create projection 2\n");
    exit(1);
  }

  double x=39*M_PI/180 + ((double)rand()/RAND_MAX - 0.5) * M_PI/30; // 36..42
  double y=((double)rand()/RAND_MAX - 0.5) * M_PI;         // -90..+90
  double z=((double)rand()/RAND_MAX - 0.5) * 6000;         // +/- 3000m

  printf("%.12f %.12f %f\n",x*180/M_PI,y*180/M_PI,z);

  int i;
  for (i=0; i<1000000; i++){

//    z=0;
    int j = pj_transform(P1,P2, 1, 1, &x, &y, &z);

    if (j) {
      fprintf(stderr, "can't do transform\n");
      exit(1);
    }

//    z=0;
    j = pj_transform(P2,P1, 1, 1, &x, &y, &z);

    if (j) {
      fprintf(stderr, "can't do transform\n");
      exit(1);
    }
  }

  printf("%.12f %.12f %f\n",x*180/M_PI,y*180/M_PI,z);

  pj_free(P1);
  pj_free(P2);

}