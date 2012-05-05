package fig_grid;

use strict;
use warnings;

# изготовление сетки fig

sub create{
  my $p=shift;
  my %p=%{$p};

  if (!exists($p{minx}))   {$p{minx}=0;}  # x-range
  if (!exists($p{maxx}))   {$p{maxx}=10;}
  if (!exists($p{miny}))   {$p{miny}=0;}  # y-range
  if (!exists($p{maxy}))   {$p{maxy}=10;}
  if (!exists($p{xsc}))    {$p{xsc}=1;}   # user units -> cm
  if (!exists($p{ysc}))    {$p{ysc}=1;}
  if (!exists($p{dx}))     {$p{dx}=1;}    # steps for grid lines
  if (!exists($p{dy}))     {$p{dy}=1;}
  if (!exists($p{dxb}))    {$p{dxb}=5;}   # steps for bold lines 
  if (!exists($p{dyb}))    {$p{dyb}=5;}
  if (!exists($p{fsize}))  {$p{fsize}=10;}   # font size
  if (!exists($p{fdepth})) {$p{fdepth}=120;} # font depth
  if (!exists($p{ldepth})) {$p{ldepth}=110;} # line depth
  if (!exists($p{fcolor})) {$p{fcolor}=15;}  # font color
  if (!exists($p{lcolor})) {$p{lcolor}=11;}  # line color

  my $cm2fig = 449.943757;
  my $fn2cm  = 0.045;

  my $maxy = $p{maxy};
  my $miny = $p{miny};
  my $maxx = $p{maxx};
  my $minx = $p{minx};

  my $ret="";

  #################
  # fig-файл

  $ret.=qq*#FIG 3.2  Produced by fig_grid.pm
Landscape
Center
Metric
A4
375.00
Single
-2
1200 2
*;

  # рисуем сетку

  for (my $x = $minx; $x <=$maxx; $x+=$p{dx}){
    my $d = 1;
    if ($x%$p{dxb}==0) {$d=2;}
    $ret.="2 1 0 $d $p{lcolor} $p{lcolor} $p{ldepth} -1 -1 0.000 0 0 7 0 0 2\n";
    $ret.=sprintf "  %d %d %d %d\n",
      int(($x-$minx)*$p{xsc}*$cm2fig),
      0,
      int(($x-$minx)*$p{xsc}*$cm2fig),
      int(($maxy-$miny)*$p{ysc}*$cm2fig);
    $ret.=sprintf "4 1 $p{fcolor} $p{fdepth} -1 18 $p{fsize} 0.0000 4 105 300 %d %d %d\\001\n",
      int(($x-$minx)*$p{xsc}*$cm2fig),
      int((($maxy-$miny)*$p{ysc}+$p{fsize}*$fn2cm)*$cm2fig),
      int($x);
  }

  for (my $y = $miny; $y <=$maxy; $y+=$p{dy}){
    my $d =1;
    if ($y%$p{dyb}==0) {$d=2;}
    $ret.="2 1 0 $d $p{lcolor} $p{lcolor} $p{ldepth} -1 -1 0.000 0 0 7 0 0 2\n";
    $ret.=sprintf "  %d %d %d %d\n",
      int(($maxx-$minx)*$p{xsc}*$cm2fig),
      int(($maxy-$y)*$p{ysc}*$cm2fig),
      0,
      int(($maxy-$y)*$p{ysc}*$cm2fig);
    $ret.=sprintf "4 2 $p{fcolor} $p{fdepth} -1 18 $p{fsize} 0.0000 4 105 300 %d %d %d\\001\n", 
      int((-$p{fsize}*$fn2cm/3)*$cm2fig),
      int((($maxy-$y)*$p{ysc}+$p{fsize}*$fn2cm/3)*$cm2fig),
      int($y);
  }

  foreach my $e (@_){
    next unless exists $e->{type};
    if ($e->{type} eq "box"){
      my $color=exists($e->{color})? $e->{color} : 1;
      my $depth=exists($e->{depth})? $e->{depth} : 125;
      my $x1=exists($e->{x})? $e->{x} : 0;
      my $y1=exists($e->{y})? $e->{y} : 0;
      my $x2=(exists($e->{w})? $e->{w} : 1)+$x1;
      my $y2=(exists($e->{h})? $e->{h} : 1)+$y1;
      $x1=int(($x1-$minx)*$p{xsc}*$cm2fig);
      $y1=int(($maxy-$y1)*$p{ysc}*$cm2fig);
      $x2=int(($x2-$minx)*$p{xsc}*$cm2fig);
      $y2=int(($maxy-$y2)*$p{ysc}*$cm2fig);

      $ret.="2 2 0 0 $color $color $depth -1 20 0.000 0 0 7 0 0 5\n";
      $ret.="  $x1 $y1  $x2 $y1  $x2 $y2  $x1 $y2  $x1 $y1\n";
      next;
    }
    if ($e->{type} eq "text"){
      my $color=exists($e->{color})? $e->{color} : 1;
      my $depth=exists($e->{depth})? $e->{depth} : 50;
      my $size=exists($e->{size})? $e->{size}    : 10;
      my $align=exists($e->{align})? $e->{align} : 0;
      my $a=exists($e->{a})? $e->{a} : 0.0;
      my $x=exists($e->{x})? $e->{x} : 0;
      my $y=exists($e->{y})? $e->{y} : 0;
      my $text=exists($e->{text})? $e->{text} : "<text>";
      $x=int(($x-$minx)*$p{xsc}*$cm2fig);
      $y=int(($maxy-$y)*$p{ysc}*$cm2fig);
      $ret.="4 $align $color $depth -1 18 $size $a 4 105 90 $x $y $text\\001\n";
      next;
    }
    if ($e->{type} eq "pt"){
      my $color=exists($e->{color})? $e->{color} : 1;
      my $depth=exists($e->{depth})? $e->{depth} : 60;
      my $size=exists($e->{size})? $e->{size}    : 3;
      my $x=exists($e->{x})? $e->{x} : 0;
      my $y=exists($e->{y})? $e->{y} : 0;
      $x=int(($x-$minx)*$p{xsc}*$cm2fig);
      $y=int(($maxy-$y)*$p{ysc}*$cm2fig);
      $ret.="2 1 0 $size $color 7 $depth -1 -1 0.000 0 1 -1 0 0 1\n";
      $ret.="  $x $y\n";
      next;
    }

  }

  return $ret;
}

1;