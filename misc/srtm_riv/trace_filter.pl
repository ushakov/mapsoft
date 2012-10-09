#!/usr/bin/perl -w
use strict;

my $PI = 3.1415926;
my $avr_n = 3;  ## усреднение высоты и рассчет уклона - по n*2+1 точкам

my %dist;

my @data;

sub print_data{

  return if $#data==-1;

  printf "# %8s %9s %7s %7s %4s %7s %6s\n", 'lon', 'lat', 'dist', 'area', 'h0', 'h', 'slope';

  # считаем длину реки
  my $dist=0;
  $dist = $dist{$data[0]->{x}." ".$data[0]->{y}} 
    if exists($dist{$data[0]->{x}." ".$data[0]->{y}});
  my $lat0 = -100;
  my $lon0 = -100;
  foreach (@data){
    if ($lat0 != -100){
      my $dy = ($_->{lat}-$lat0)/180.0*$PI*6380.0;
      my $dx = ($_->{lon}-$lon0)/180.0*$PI*6380.0*cos($_->{lat}/180.0*$PI);
      my $ddist = sqrt($dx*$dx+$dy*$dy);
      $dist+=$ddist;
    }
    $_->{d} = $dist;
    $lat0 = $_->{lat};
    $lon0 = $_->{lon};
    $dist{$_->{x}." ".$_->{y}} = $dist;
  }

  # фильтры высоты:
  my $w = 30;
  my $a = 20;
  my $w1 = 2;
  my $w2 = 3;

  for (my $i = 0; $i<= $#data; $i++){
    $data[$i]->{h0} = $data[$i]->{h};
  }

  
  if ($#data < $w){
    for (my $i = 0; $i<= $#data-$w; $i++){
      my $x1 = $data[$i]->{d};
      my $x2 = $data[$i+$w]->{d};
      my $y1 = $data[$i]->{h};
      my $y2 = $data[$i+$w]->{h};
      for (my $j = $i; $j<= $i+$w; $j++){
        my $x = $data[$j]->{d};
        my $y = $y1 + ($x-$x1)/($x2-$x1)*($y2-$y1) + $a;
        if (($y < $data[$j]->{h})&&($data[$j]->{h} > $y2))
           {$data[$j]->{h}=$y1 + ($x-$x1)/($x2-$x1)*($y2-$y1);}
      }
    }
  }
  
  
  for (my $i = 0; $i<= $#data; $i++){
    my $x0 = $data[$i]->{d};
    my $y0 = $data[$i]->{h};
    my $li = $i-$w1*5; #$li = 0 if $li<0;
    my $ri = $i+$w1*5; #$ri = $#data if $ri>$#data;
    $li =  -$#data if $li < -$#data;
    $ri = 2*$#data if $ri >2*$#data;
    my $sy=0;
    my $s=0;
    for (my $ii = $li; $ii<= $ri; $ii++){
      my ($x,$y);
      if ($ii < 0){
        $x = 2*$data[0]->{d} - $data[-$ii]->{d};
        $y = 2*$data[0]->{h} - $data[-$ii]->{h};
      } elsif ($ii>$#data){
        $x = 2*$data[$#data]->{d} - $data[2*$#data-$ii]->{d};
        $y = 2*$data[$#data]->{h} - $data[2*$#data-$ii]->{h};
      } else {
        $x = $data[$ii]->{d};
        $y = $data[$ii]->{h};
      }
      my $f = exp(-($x-$x0)**2/$w1);
      $s+=$f;
      $sy+=$f*$y;
    }
    $data[$i]->{h} = $sy/$s;
    $data[$i]->{u} = $s;
  }

  for (my $i = 0; $i<= $#data; $i++){
    my $x0 = $data[$i]->{d};
    my $y0 = $data[$i]->{h};
    my $li = $i-$w1*5; $li = 0 if $li<0;
    my $ri = $i+$w1*5; $ri = $#data if $ri>$#data;
    my $sx=0;
    my $sy=0;
    my $sxy=0;
    my $sxx=0;
    my $s=0;
    for (my $ii = $li; $ii<= $ri; $ii++){
      my $x = $data[$ii]->{d};
      my $y = $data[$ii]->{h};
      $s+=1;
      $sx+=$x;
      $sxy+=$x*$y;
      $sxx+=$x*$x;
      $sy+=$y;
    }
  
    $data[$i]->{u} = ($s*$sxx-$sx*$sx)!=0 ? ($s*$sxy-$sx*$sy)/($s*$sxx-$sx*$sx):0;
    printf "%10.6f %9.6f %7.3f %7.2f %4d %7.2f %6.2f\n",
       $data[$i]->{lon}, $data[$i]->{lat}, $data[$i]->{d}, 
       $data[$i]->{a}, $data[$i]->{h0}, $data[$i]->{h}, $data[$i]->{u};
  }
  @data=();
  print "\n";
}

foreach (<STDIN>){
  if (/^\s*$/){
    print_data();
    next;
  }

  my ($y, $x, $a, $d, $h) = split /\s+/;
  push @data, {x=>$x, y=>$y, lat=>$y/1200.0, lon=>$x/1200.0, a=>$a, h=>$h};
}
print_data();
