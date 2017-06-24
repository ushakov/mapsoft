#include <stdio.h>
#include <proj_api.h>

int main(){

  char *pr1[] = {"+proj=latlong", "+ellps=WGS84", "+k_0=10"};
  char *pr2[] = {"+proj=tmerc",   "+ellps=krass", "+lon_0=39",
                 "+scale=1", "+lat_0=0", "+x_0=500000"};

  projPJ P1 = pj_init(sizeof(pr1)/sizeof(*pr1), pr1);
  projPJ P2 = pj_init(sizeof(pr2)/sizeof(*pr2), pr2);


  if (!P1) {
    fprintf(stderr, "can't create projection 1\n");
    exit(1);
  }

  if (!P2) {
    fprintf(stderr, "can't create projection 2\n");
    exit(1);
  }

  int i;
  for (i=0; i<10; i++){

    double x=39*M_PI/180 + ((double)rand()/RAND_MAX - 0.5) * M_PI/30; // 36..42
    double y=((double)rand()/RAND_MAX) * M_PI/2;         //  0..+90
    double z=((double)rand()/RAND_MAX - 0.5) * 6000;     // +/- 3000m

    printf("%f %f %f -> ",x*180/M_PI,y*180/M_PI,z);

//    int j = 0;
    int j = pj_transform(P1,P2, 1, 1, &x, &y, &z);

    printf("%f %f %f\n",x,y,z);

    if (j) {
      fprintf(stderr, "can't do transform\n");
      exit(1);
    }
  }

  pj_free(P1);
  pj_free(P2);

}