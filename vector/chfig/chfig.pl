#!/usr/bin/perl -w 
use XFig;
use Data::Dumper;
$Data::Dump::Purity = 1;
my $PI = 3.1415926;

# This program reads all elements from FIG file
# using XFig.pm from http://www.ife.ee.ethz.ch/~rohner/fig2sty/
# and change them according to sub convert_element()

die "usage: $0 <infile.fig> <outfile.fig>\n"
  if ($#ARGV!=1);
my ($infile, $outfile) = @ARGV[0..2];

my $f1 = new XFig; # source

$f1->parsefile($infile);
die "Can't read file $infile\n" if ($f1->{'object'} ne 'compound');

######################################
sub convert_element{
   my $el = shift;
   if (($el->{object} =~ /polyline/) || ($el->{object} =~ /spline/)){
#     print $el->{forwardarrow}[4]."\n";

     # river
     if (($el->{thickness} >= 3)&&($el->{pencolor} == 33)&&($el->{depth} == 85)){ 
        my $el2=copy_element($el);

        $el2->{pencolor} = 3;
        $el2->{depth} = 84;
        $el2->{thickness} -= 2;
        $el->{parent}->add($el2);
     }

     # dashed road
     if (($el->{thickness} == 1)&&($el->{pencolor} == 35)&&($el->{depth} == 80)){ 
        $el->{pencolor} = 0;
        my $el2=copy_element($el);

        $el2->{pencolor} = 7;
        $el2->{depth} = 79;
        $el2->{capstyle} = 2;
        $el2->{linestyle} = 2;
        $el2->{styleval} = 8.0;
 
        $el->{parent}->add($el2);
     }

     # white road
     if (($el->{thickness} == 3)&&($el->{pencolor} == 35)&&($el->{depth} == 80)){ 
        $el->{pencolor} = 0;

        my $el2=copy_element($el);
        $el2->{pencolor} = 7;
        $el2->{depth} = 79;
        $el2->{thickness} = 1;
        $el->{parent}->add($el2);
     }

     # red road
     if (($el->{thickness} == 4)&&($el->{pencolor} == 35)&&($el->{depth} == 80)){ 
        $el->{pencolor} = 0;

        my $el2=copy_element($el);
        $el2->{pencolor} = 27;
        $el2->{depth} = 79;
        $el2->{thickness} = 2;
        $el->{parent}->add($el2);
     }

     # highway
     if (($el->{thickness} == 7)&&($el->{pencolor} == 35)&&($el->{depth} == 80)){ 
        $el->{pencolor} = 0;

        my $el2=copy_element($el);
        $el2->{pencolor} = 27;
        $el2->{depth} = 79;
        $el2->{thickness} = 5;
        $el->{parent}->add($el2);

        my $el3=copy_element($el);
        $el3->{pencolor} = 0;
        $el3->{depth} = 78;
        $el3->{thickness} = 1;
        $el->{parent}->add($el3);
     }

     # gas
     if (($el->{thickness} == 2)&&($el->{pencolor} == 18)&&($el->{depth} == 80)){ 
        $el->{pencolor} = 0;

        my $el2=copy_element($el);
        $el2->{pencolor} = 0;
        $el2->{depth} = 80;
        $el2->{thickness} = 5;
        $el2->{linestyle} = 2;
        $el2->{styleval} = 42.0;
        $el2->{capstyle} = 1;
        $el->{parent}->add($el2);

        my $el3=copy_element($el);
        $el3->{pencolor} = 7;
        $el3->{depth} = 79;
        $el3->{thickness} = 3;
        $el3->{linestyle} = 2;
        $el3->{styleval} = 42.0;
        $el3->{capstyle} = 1;
        $el->{parent}->add($el3);
     }

     # bridge
     if (($el->{thickness} > 0)&&($el->{pencolor} == 7)&&($el->{depth} == 77)&&
        ($#{$el->{points}}==1)){ 

	my $x1 = $el->{points}->[0][0];
	my $x2 = $el->{points}->[1][0];
	my $y1 = $el->{points}->[0][1];
	my $y2 = $el->{points}->[1][1];
        my $l = sqrt(1.0*($x2-$x1)**2 + 1.0*($y2-$y1)**2);
	my $a = atan2($y2-$y1, $x2-$x1);
        my $w = ($el->{thickness}+1.0)*15/2.0;
	my $ll = 45.0;  # длина "стрелок"
	
        my $el2=copy_element($el);  
	my $el3=copy_element($el);
	
        $el2->{pencolor} = 0;
        $el2->{depth} = 76;
        $el2->{thickness} = 2;
        $el3->{pencolor} = 0;
        $el3->{depth} = 76;
        $el3->{thickness} = 2;
	
	$el2->{points}->[0][0] = $x1 - $w*sin($a) - $ll*sin($a + $PI/4.0);
	$el2->{points}->[1][0] = $x1 - $w*sin($a);
	$el2->{points}->[2][0] = $x2 - $w*sin($a);
	$el2->{points}->[3][0] = $x2 - $w*sin($a) - $ll*sin($a - $PI/4.0);

	$el3->{points}->[0][0] = $x1 + $w*sin($a) + $ll*sin($a - $PI/4.0);
	$el3->{points}->[1][0] = $x1 + $w*sin($a);
	$el3->{points}->[2][0] = $x2 + $w*sin($a);
	$el3->{points}->[3][0] = $x2 + $w*sin($a) + $ll*sin($a + $PI/4.0);

	$el2->{points}->[0][1] = $y1 + $w*cos($a) + $ll*cos($a + $PI/4.0);
	$el2->{points}->[1][1] = $y1 + $w*cos($a);
	$el2->{points}->[2][1] = $y2 + $w*cos($a);
	$el2->{points}->[3][1] = $y2 + $w*cos($a) + $ll*cos($a - $PI/4.0);

	$el3->{points}->[0][1] = $y1 - $w*cos($a) - $ll*cos($a - $PI/4.0);
	$el3->{points}->[1][1] = $y1 - $w*cos($a);
	$el3->{points}->[2][1] = $y2 - $w*cos($a);
	$el3->{points}->[3][1] = $y2 - $w*cos($a) - $ll*cos($a + $PI/4.0);
	
	$el->{pencolor} = 7;
        $el->{fillcolor} = 7;
        $el->{depth} = 77;
        $el->{thickness} = 0;
        $el->{areafill} = 20;

	$el->{points}->[0][0] = $x1 - $w*sin($a);
	$el->{points}->[1][0] = $x2 - $w*sin($a);
	$el->{points}->[2][0] = $x2 + $w*sin($a);
	$el->{points}->[3][0] = $x1 + $w*sin($a);
	$el->{points}->[0][1] = $y1 + $w*cos($a);
	$el->{points}->[1][1] = $y2 + $w*cos($a);
	$el->{points}->[2][1] = $y2 - $w*cos($a);
	$el->{points}->[3][1] = $y1 - $w*cos($a);

        $el->{parent}->add($el2);
        $el->{parent}->add($el3);
     }



   }
}
######################################
sub copy_element{
  my $el = shift; 
  my $parent = $el->{parent};
  $el->{parent}=0;

  my $el2 = eval Dumper($el);
  $el->{parent} = $parent;
  $el2->{parent} = $parent;

  return $el2;
}


sub get_all_elements{ 
  my $el = shift;
  if ($el->{'object'}  !~ /compound/i) {return;}
  foreach $element (@{$el->{elements}}) {
   if ($element->{'object'} =~ /compound/i)
     {get_all_elements($element)}
   else {convert_element($element)}
  }
}

get_all_elements($f1);
$f1->writefile ($outfile);
