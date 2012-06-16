#include "conv_triv.h"

ConvTriv::ConvTriv(double sc):scale(sc){};

void
ConvTriv::frw(dPoint & p) const {p*=scale;}
void
ConvTriv::bck(dPoint & p) const {p/=scale;}

void
ConvTriv::rescale_src(const double sc){ scale/=sc; }
void
ConvTriv::rescale_dst(const double sc){ scale*=sc; }

