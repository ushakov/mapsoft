## shell functions for westra passes downloading
## official kml interface

MAPSOFT_WP1=1
[ -n "${MAPSOFT_CRD:-}" ] || . mapsoft_crd.sh

# get url for downloading data from westra passes catalog
# usage:  geom2wp <wgs lonlat geom> (mp|txt|wpt)
ll2wp(){
  local geom="$1"

  echo "$geom" |
  sed 's/\(^[0-9\.]\+\)x\([0-9\.]\+\)\([\+-][0-9\.]\+\)\([\+-][0-9\.]\+\)/\1 \2 \3 \4/' |
  {
    local x1 x2 y1 y2 dx dy
    read dx dy x1 y1
    x2="$(printf -- "${x1#+} + $dx\n" | bc -l)"
    y2="$(printf -- "${y1#+} + $dy\n" | bc -l)"
    printf "http://westra.ru/passes/kml/passes.php?BBOX=%f,%f,%f,%f" $x1 $y1 $x2 $y2
  }
}

# download mp, wpt, txt files for a given wgs latlon range
# remove old-style comments
download_ll_wp(){
  local geom="$1"
  local name="$2"

  date="$(date +"%F %T")"
  wget "$(ll2wp "$geom")" -O - > "${name}_wp.kml"
  mapsoft_wp_parse "${name}_wp.kml"
}


# download mp, wpt, txt files for a given pulkovo tmerc range
# fix old style comments, add Source=westra_passes
download_gk_wp(){
  local geom="$1"
  local name="$2"

  ll="$(geom2ll "$geom")"
  download_ll_wp "$ll" "$name"
}


