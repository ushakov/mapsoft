B<<  --shift_maps <X,Y> >> -- shift map references

B<<  --rescale_maps <k> >> -- rescale map references

B<<  --map_nom_brd >> -- set map borders according to map name

B<<  -s, --skip <wmtao> >> -- skip data, "wmtao" (w - waypoints, m -
maps, t - tracks, a - active log, o - save tracks)

B<<  --gen_n <n> >> -- reduce track points to n

B<<  --gen_e <e> >> -- reduce track points up to accuracy e [meters]
(when gen_n and gen_e both used it means: "remove points while number of
points > n OR accuracy < e"
