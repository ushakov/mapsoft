#!/usr/bin/perl -w
use strict;

$#ARGV == 0 or die "usage $0 <map name>\n";

my $name = $ARGV[0];

-f "$name.fig" or die "can't find $name.fig\n";
-f "$name.300.png" or die "can't find $name.300.png\n";

open R, "./RANGE" or die "can't open RANGE file\n";
my ($sc,$lon0,$x1,$x2,$y1,$y2,$n) = (0,0,0,0,0,0,0);
foreach(<R>){
  if (/$name\s+1:([\d\.]+)\s+([\d\.]+)\s+([\d\.]+)-([\d\.]+)\s+([\d\.]+)-([\d\.]+)/){
    ($sc,$lon0,$x1,$x2,$y1,$y2,$n) = ($1,$2,$3,$4,$5,$6,1);
  }
}
$n==1 or die "can't find $name in RANGE file\n";
my $w = $x2-$x1;
my $h = $y2-$y1;
my $s = $w*$h;

$x1*=1000; $x2*=1000; 
$y1*=1000; $y2*=1000; 

my $px_per_km = 300.0/2.54 * 100000/$sc;
print "$name 1:$sc lon0=$lon0 x=$x1-$x2 y=$y1-$y2 ($w x $h km, $s km2)\n";

# поля слева и сверху -- 1 км!!!
my $rx = int($px_per_km);
my $ry = int($px_per_km);
my $rw = int($w*$px_per_km)+4;
my $rh = int($h*$px_per_km)+4;
my $rw1=int($rw/2);
my $rh1=int($rh/2);

print "cropping ${rw}x${rh}+${rx}+${ry}, resizing to ${rw1}x${rh1}\n";

#`pngtopnm $name.300.png | pnmcrop -white | pnmcut $rx $ry $rw $rh | pnmscale 0.5 | ppmquant -nofs 256 | pnmtopng > $name.png`;
`pngtopnm $name.300.png | pnmcrop -white | pnmcut $rx $ry $rw $rh | pnmscale 0.5 | ppmquant 256 | pnmtopng > $name.png`;

my $mapfile = qq*OziExplorer Map Data File Version 2.2\r
$name\r
$name.png\r
1 ,Map Code,\r
Pulkovo 1942 (1),,   0.0000,   0.0000,WGS 84\r
Reserved 1\r
Reserved 2\r
Magnetic Variation,,,E\r
Map Projection,Transverse Mercator,PolyCal,No,AutoCalOnly,No,BSBUseWPX,No\r
Point01,xy,    0,    0,in, deg,    ,        ,N,    ,        ,E, grid,   ,$x1,$y2,N\r
Point02,xy, $rw1,    0,in, deg,    ,        ,N,    ,        ,E, grid,   ,$x2,$y2,N\r
Point03,xy, $rw1, $rh1,in, deg,    ,        ,N,    ,        ,E, grid,   ,$x2,$y1,N\r
Point04,xy,    0, $rh1,in, deg,    ,        ,N,    ,        ,W, grid,   ,$x1,$y1,N\r
Point05,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point06,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point07,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point08,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point09,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point10,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point11,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point12,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point13,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point14,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point15,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point16,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point17,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point18,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point19,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point20,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point21,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point22,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point23,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point24,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point25,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point26,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point27,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point28,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point29,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Point30,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N\r
Projection Setup,     0.000000000,    $lon0,     1.000000000,      500000.00,            0.00,,,,,\r
Map Feature = MF ; Map Comment = MC     These follow if they exist\r
Track File = TF      These follow if they exist\r
Moving Map Parameters = MM?    These follow if they exist\r
MM0,Yes\r
MMPNUM,4\r
MMPXY,1,0,0\r
MMPXY,2,$rw1,0\r
MMPXY,3,$rw1,$rh1\r
MMPXY,4,0,$rh1\r
MM1B,0.00000000\r
*;

open M, "> $name.map" or die "can't open $name.map\n";
print M $mapfile;

