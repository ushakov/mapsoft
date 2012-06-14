#include "conv_aff.h"
#include "line_utils.h"

/* Приведение матрицы (N+1)xN к диагональному виду */
#define AN(x,y) a[(x)+(y)*(N+1)]
int mdiag(int N, double *a){
  int i,j,k,l;
  double tmp;

  for (k=0; k<N; ++k){  /* идем по строчкам сверху вниз */
    /* циклически переставляем строчки от k до N-1, чтобы на месте (k,k) встал не ноль */
    for (i=k; i<N; ++i){ if (AN(k,k)!=0) break;
      for (j=0;j<N+1;++j){ /* по всем столбцам */
        tmp=AN(j,k);        /* сохраним верхнее */
        for (l=k; l<N-1; ++l) AN(j,l)=AN(j,l+1); /* сдвинем */
        AN(j,N-1)=tmp; /* бывшее верхнее -- вниз */
      }
    }
    /* если так сделать нельзя, уравнения зависимы */
    if (AN(k,k)==0) return 1;
    /* делим строчку на A(k,k) (от N до k, т.к. в начале -- нули)*/
    for (j=N; j>=k; --j) AN(j,k)=AN(j,k)/AN(k,k);
    /* вычитаем из всех остальных строчек эту, помноженную на A(k,*) */
    for (i=0; i<N; ++i) if (i!=k) for (j=N; j>=k; --j) AN(j,i)-=AN(k,i)*AN(j,k);
  }
  return 0;
}


void ConvAff::bck_recalc(){
  double D = k_frw[0] * k_frw[4] - k_frw[1] * k_frw[3];

  if (D==0) throw "ConvAff: can't calculate matrix for backward conversion.";

  k_bck[0] =   k_frw[4] / D;
  k_bck[1] = - k_frw[1] / D;
  k_bck[2] = (k_frw[1] * k_frw[5] - k_frw[2] * k_frw[4]) / D;

  k_bck[3] = - k_frw[3] / D;
  k_bck[4] =   k_frw[0] / D;
  k_bck[5] = (- k_frw[0] * k_frw[5] + k_frw[3] * k_frw[2]) / D;
}

ConvAff::ConvAff(): k_frw(6), k_bck(6){
  reset();
}

ConvAff::ConvAff(const std::map<dPoint, dPoint> & ref): k_frw(6), k_bck(6){
  set_from_ref(ref);
}

void
ConvAff::set_from_ref(const std::map<dPoint, dPoint> & ref){
/*
  a x + b y + c = X
  d x + e y + f = Y

 Чтобы найти минимум среднеквадратичного отклонения,
 надо решать такую систему:

 d/d(a..f) SUMi(  a xi + b yi + c - Xi )^2 +
    SUMi(  d xi + e yi + f - Yi )^2 = 0

 a Sxx + b Sxy + c Sx - SXx = 0
 a Sxy + b Syy + c Sy - SXy = 0
 a Sx  + b Sy  + c S  - SX  = 0
 d Sxx + e Sxy + f Sx - SYx = 0
 d Sxy + e Syy + f Sy - SYy = 0
 d Sx  + e Sy  + f S  - SY  = 0

 Sxx Sxy Sx 0   0   0  = SXx
 Sxy Syy Sy 0   0   0  = SXy
 Sx  Sy  S  0   0   0  = SX
 0   0   0  Sxx Sxy Sx = SYx
 0   0   0  Sxy Syy Sy = SYy
 0   0   0  Sx  Sy  S  = SY
*/

#define A7(x,y) a[(x)+(y)*7]
  double a[6*7];
  for (int i=0; i<6*7; i++) a[i]=0;

  std::map<dPoint, dPoint>::const_iterator pp;
  for (pp=ref.begin(); pp!=ref.end(); pp++){
    double x  = pp->first.x;
    double y  = pp->first.y;
    double xc = pp->second.x;
    double yc = pp->second.y;

    A7(0,0)+=x*x; A7(3,3)+=x*x;
    A7(1,0)+=x*y; A7(4,3)+=x*y;
    A7(2,0)+=x;   A7(5,3)+=x;
    A7(0,1)+=x*y; A7(3,4)+=x*y;
    A7(1,1)+=y*y; A7(4,4)+=y*y;
    A7(2,1)+=y;   A7(5,4)+=y;
    A7(0,2)+=x;   A7(3,5)+=x;
    A7(1,2)+=y;   A7(4,5)+=y;
    A7(2,2)+=1;   A7(5,5)+=1;

    A7(6,0)+=xc*x; A7(6,1)+=xc*y; A7(6,2)+=xc;
    A7(6,3)+=yc*x; A7(6,4)+=yc*y; A7(6,5)+=yc;
  }
  if (mdiag (6, a) != 0) throw "ConvAff: can't calculate conversion matrix.";

  for (int i=0; i<6; i++) k_frw[i] = A7(6,i);
  bck_recalc();
}

void
ConvAff::frw(dPoint & p) const{
  double x = k_frw[0]*p.x + k_frw[1]*p.y + k_frw[2];
  double y = k_frw[3]*p.x + k_frw[4]*p.y + k_frw[5];
  p.x=x; p.y=y;
}

void
ConvAff::bck(dPoint & p) const{
  double x = k_bck[0]*p.x + k_bck[1]*p.y + k_bck[2];
  double y = k_bck[3]*p.x + k_bck[4]*p.y + k_bck[5];
  p.x=x; p.y=y;
}


void
ConvAff::reset(){
  for (int i=0; i<6; i++) k_frw[i]=k_bck[i]=0;
  k_frw[0] = k_bck[0] = 1.0;
  k_frw[4] = k_bck[4] = 1.0;
}

void
ConvAff::shift(const dPoint & p){
  k_frw[2] += p.x;
  k_frw[5] += p.y;
  bck_recalc();
}

void
ConvAff::scale(const double kx, const double ky){
  for (int i=0; i<3; i++) k_frw[i]*=kx;
  for (int i=3; i<6; i++) k_frw[i]*=ky;
  bck_recalc();
}

void
ConvAff::rescale_src(const double s){
  for (int i=0; i<5; i++){
    if (i!=2) k_frw[i] /= s; // i!=2 && i!=5
  }
  bck_recalc();
}

void
ConvAff::rescale_dst(const double s){
  for (int i=0;i<6;i++) k_frw[i] *= s;
  bck_recalc();
}
