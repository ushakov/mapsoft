## Shell funcs for westra passes downloading.
## Uses official kml interface of the Westra catalog.

MAPSOFT_WP1=1
[ -n "${MAPSOFT_CRD:-}" ] || . mapsoft_crd.sh

# get url for downloading data from westra passes catalog
# usage:  ll2wp <wgs lonlat geom>
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

# Download kml file for a given wgs latlon range.
# Convert kml to mp and txt form.
download_ll_wp(){
  local geom="$1"
  local name="$2"

  wget "$(ll2wp "$geom")" -O - > "${name}_wp.kml"
  mapsoft_wp_parse "${name}_wp.kml"
}


# Download kml file for a given pulkovo tmerc range.
# Convert kml to mp and txt form.
download_gk_wp(){
  local geom="$1"
  local name="$2"

  ll="$(geom2ll "$geom")"
  download_ll_wp "$ll" "$name"
}


