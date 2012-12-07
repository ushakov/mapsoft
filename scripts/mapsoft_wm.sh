## shell functions for wikimapia data downloading

### YOU MUST PUT WIKIMAPIA KEY TO $HOME/.mapsoft/wikimapia.key

MAPSOFT_WM=1
[ -n "${MAPSOFT_CRD:-}" ] || . mapsoft_crd.sh

###########

wm2mp(){
  sed -r -e '
  s|<place|\n<place|g
  s|<\?[^>]+>||g
  s|<place id="([0-9]+)"><name>([^<]+)</name>|<track comm="\2">|g
  s|<url>[^<]*</url>||g
  s#<(lat|lon|north|south|east|west)>[^<]*</(lat|lon|north|south|east|west)>##g
  s|<location>[^<]*</location>||g
  s|</?polygon>||g
  s|<point|<pt|g
  s|</place>|</track>|g
  s|</?folder[^>]*>||g
'

}

get_wm(){
  BBOX="$1"
  wget -O - \
"http://api.wikimapia.org/?function=box\
&key=$(cat $HOME/.mapsoft/wikimapia.key)\
&bbox=$1&count=500&format=xml"
}

download_ll_wm(){
  local geom="$1"
  local name="$2"
  echo "$geom" |
  sed 's/\(^[0-9\.]\+\)x\([0-9\.]\+\)\([\+-][0-9\.]\+\)\([\+-][0-9\.]\+\)/\1 \2 \3 \4/' |
  {
    local x1 x2 y1 y2 dx dy
    read dx dy x1 y1
    x2="$(printf -- "${x1#+} + $dx\n" | bc -l)"
    y2="$(printf -- "${y1#+} + $dy\n" | bc -l)"
    get_wm "$x1,$y1,$x2,$y2" | wm2mp > "${name}_wm.xml"
  }
}

download_gk_wm(){
  local geom="$1"
  local name="$2"

  ll="$(geom2ll "$geom")"
  download_ll_wm "$ll" "$name"
}


