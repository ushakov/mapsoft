## shell functions for glims data downloading

MAPSOFT_GL=1
[ -n "${MAPSOFT_CRD:-}" ] || . mapsoft_crd.sh

###########

gml2mp(){
sed -n -e '
1{
i\
[IMG ID]\r\
ID=0\r\
Name=\r\
Elevation=M\r\
Preprocess=F\r\
CodePage=1251\r\
LblCoding=9\r\
TreSize=511\r\
TreMargin=0.000000\r\
RgnLimit=127\r\
POIIndex=N\r\
Levels=4\r\
Level0=22\r\
Level1=21\r\
Level2=19\r\
Level3=17\r\
Zoom0=0\r\
Zoom1=1\r\
Zoom2=2\r\
Zoom3=3\r\
MG=Y\r\
PolygonEvaluate=Y\r\
Transparent=Y\r\
[END-IMG ID]\r
}
/^[[:space:]]*<gml:posList/{
  s/^[[:space:]]*<gml:posList[^>]*>\([^<>]*\)<[^>]*>[[:space:]]*$/\1/
  s/\([0-9\.-]\+\)[[:space:]]\+\([0-9\.-]\+\)[[:space:]]\+/(\2,\1),/g
  s/,$//
  s/^/Data0=/
  i\
\
[POLYGON]\r\
Type=0x4d\r\
Levels=1\r
  a\
[END]\r
  p
}
'
}

get_gl(){
  TYPENAME="$1"
  BBOX="$2"
  wget -O - \
"http://glims.colorado.edu/cgi-bin/glims_ogc?\
service=WFS&\
request=GetFeature&\
VERSION=1.1.0&\
TYPENAME=$TYPENAME&\
BBOX=$BBOX"
}


download_gl(){
  local geom="$1"
  geom2ll "$geom" |
  sed 's/\(^[0-9\.]\+\)x\([0-9\.]\+\)\([\+-][0-9\.]\+\)\([\+-][0-9\.]\+\)/\1 \2 \3 \4/' |
  {
    local x1 x2 y1 y2 dx dy
    read dx dy x1 y1
    x2="$(printf -- "${x1#+} + $dx\n" | bc -l)"
    y2="$(printf -- "${y1#+} + $dy\n" | bc -l)"
    get_gl GLIMS_Glacier_Polygons "$x1,$y1,$x2,$y2" | gml2mp > "${name}_gl.mp"
  }
}
