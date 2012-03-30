### OBSOLETE

. shell-config
. shell-error
. shell-quote

[ -n "${MAPSOFT_CRD:-}" ] || . mapsoft_crd.sh
[ -n "${MAPSOFT_WP:-}" ] || . mapsoft_wp.sh
[ -n "${MAPSOFT_GL:-}" ] || . mapsoft_gl.sh

get_map_par(){
  local cnf="./.maps"
  local sep="[[:space:]]*=[[:space:]]*"
  local name="$1"
  local par="$2"
  local def="$3"

  local ret="$(shell_config_get "$cnf" "$par:$name" "$sep")"
  [ -n "$ret" ] ||
    ret="$(shell_config_get "$cnf" "$par" "$sep")"
  [ -n "$ret" ] ||
    ret="$def"
  printf "%s" "$ret"
}
