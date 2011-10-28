#CPPFLAGS = -g

all: srtm2pnm geth rivers one_river arrows rtext

srtm2pnm: srtm2pnm.cpp srtm.cpp point.cpp
geth: geth.cpp srtm.cpp point.cpp

one_river: one_river.cpp srtm.cpp point.cpp map.cpp
arrows: arrows.cpp srtm.cpp point.cpp map.cpp
rivers: rivers.cpp srtm.cpp point.cpp map.cpp
rtext: rtext.cpp font.cpp srtm.cpp point.cpp map.cpp


