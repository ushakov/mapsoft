#include "comboboxes.h"

CBProj::CBProj(){
  const pair_t data_list[] = {
    pair_t(Proj("lonlat"), "Lon, Lat"),
    pair_t(Proj("tmerc"),  "Gauss-Kruger"),
  };
  set_values(data_list,
    data_list+sizeof(data_list)/sizeof(pair_t));
}

CBDatum::CBDatum(){
  const pair_t data_list[] = {
    pair_t(Datum("wgs84"), "WGS84"),
    pair_t(Datum("pulk"),  "Pulkovo 1942"),
  };
  set_values(data_list,
    data_list+sizeof(data_list)/sizeof(pair_t));
}

CBScale::CBScale(){
  const pair_t data_list[] = {
      pair_t( 50000, " 1:50'000"),
      pair_t(100000, "1:100'000"),
      pair_t(200000, "1:200'000"),
      pair_t(500000, "1:500'000")
  };
  set_values(data_list,
    data_list+sizeof(data_list)/sizeof(pair_t));
}

CBUnit::CBUnit(){
  const pair_t data_list[] = {
      pair_t( 0, "px"),
      pair_t( 1, "cm"),
      pair_t( 2, "in")
  };
  set_values(data_list,
    data_list+sizeof(data_list)/sizeof(pair_t));
}

CBPage::CBPage(){
  const pair_t data_list[] = {
  pair_t(iPoint(0,0), ""),
//    pair_t(iPoint(841,1189), "A0"),
//    pair_t(iPoint(594,841), "A1"),
//    pair_t(iPoint(420,594), "A2"),
  pair_t(iPoint(297,420), "A3"),
  pair_t(iPoint(210,297), "A4"),
  pair_t(iPoint(148,210), "A5")
//    pair_t(iPoint(105,148), "A6"),
//    pair_t(iPoint(74,105), "A7"),
//    pair_t(iPoint(52,74), "A8"),
//    pair_t(iPoint(37,52), "A9"),
//    pair_t(iPoint(26,37), "A10")
  };
  set_values(data_list,
    data_list+sizeof(data_list)/sizeof(pair_t));
}

CBCorner::CBCorner(){
  const pair_t data_list[] = {
    pair_t(0, "top-left"),
    pair_t(1, "top-right"),
    pair_t(2, "bottom-right"),
    pair_t(3, "bottom-left")
  };
  set_values(data_list,
    data_list+sizeof(data_list)/sizeof(pair_t));
}

CBSrtmW::CBSrtmW(){
  const pair_t data_list[] = {
    pair_t(1201, "3 sec"),
    pair_t(3601, "1 sec"),
  };
  set_values(data_list,
    data_list+sizeof(data_list)/sizeof(pair_t));
}
