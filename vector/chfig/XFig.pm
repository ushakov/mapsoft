package XFig;

# Package: XFig.pm Version 0.1
# XFig Parser
# Author: Marcel Rohner
# Date  : 16.5.98
# $Id: XFig.pm,v 1.1 2007/03/15 11:43:01 slazav Exp $

# Copyright (C) 1998  Marcel Rohner
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


# changed by slazav (slazav at narod.ru) to working correctly with arrows

=head1 NAME

XFig - A parser for XFig files

=head1 SYNOPSIS

 use XFig;

 # Generate a parser

 $fig = new XFig;

 # Parse a Fig file

 $fig->parsefile($figname);
 die "Can't read file $figname" if ($fig->{'object'} < 0);

 # Get header information

 $fig->Version ()
 $fig->Orientation ()
 $fig->Justification ()
 $fig->Units ()
 $fig->Papersize ()
 $fig->Magnification ()
 $fig->MultiplePage ()
 $fig->Transparent ()
 $fig->Resolution ()
 $fig->CoordSystem ()

 # Create a new Fig element and add values to it

 $line = new XFig ('polyline');
 $line->{'subtype'} = 1;  # line
 $line->{'points'} = [ [0, 0], [1, 3] ];
 $line->{'comment'} = [ 'This is just a comment', 'that is continued on this line' ];

 # Add an element to a Fig parsed file or compound element

 $fig->add ($line);

 # Loop through all elements in the parsed file and check their types

 for $element ($fig->eachPrimitive()) {
   print "Primitive\n" if $element->isPrimitive ();
   print "Color\n"     if $element->isColor ();
   print "Arc\n"       if $element->isArc ();
   print "Ellipse\n"   if $element->isEllipse ();
   print "Polyline\n"  if $element->isPolyline ();
   print "Spline\n"    if $element->isSpline ();
   print "Text\n"      if $element->isText ();
   print "Compound\n"  if $element->isCompound ();
   print "Closed\n"    if $element->isClosed ();
 }

 # Write to an XFig file

 $fig->writefile ($figname);

=head1 DESCRIPTION

The XFig module parses XFig files and allows access to its elements.
It also allows the creation of new Fig elements, the addtion of
elements to the parsed file object and the dumping to a Fig file.

Fig elements composing a XFig file are represent by hashes, whose keys
mimic very closely the definitions given in the FORMAT file distributed
with XFig.  For instance, a polyline element is a hash with the following
keys:

 'subtype', 'linestyle', 'thickness', 'pencolor', 'fillcolor',
 'depth', 'penstyle', 'areafill', 'styleval', 'joinstyle',
 'capstyle', 'radius', 'forwardarrow', 'backwardarrow', 'points'

$element->{points} is always a reference to a array of coordinates.
Each coordinate is a reference to an array of length 2, containing the
horizontal and vertical coordinates of the point.

=head1 SEE ALSO

 xfig(1), the FORMAT3.2 file.

=head1 COPYRIGHT

Copyright (C) 1998 Marcel Rohner.  This module is released under the
terms of the GNU GPL.

This manual page was written by Rafael Laboissiere <rafael@debian.org>.

=cut

sub new {
  my $class = shift;
  my $type  = shift;
  my $self = {object => -1};

  # Defaults values
  @$self{'linestyle', 'thickness', 'pencolor', 'fillcolor', 'depth',
         'penstyle', 'areafill', 'styleval', 'capstyle', 'joinstyle'} =
    (0, 1, -1, 7, 0, 0, -1, 0.0, 0, 0, 0);

  @$self{'object', 'color', 'rgb'} = ('color', 32, '#000000') 
    if (defined $type and $type =~ /color/i) ;

  @$self{'object', 'subtype', 'direction', 'angle', 'cx', 'cy', 'rx', 'ry',
         'sx', 'sy', 'ex', 'ey'} = ('ellipse', 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    if (defined $type and $type =~ /ellipse/i);

  @$self{'object', 'subtype', 'radius', 'forwardarrow', 'backwardarrow'} 
         = ('polyline', 1, 0, 0, 0)
    if (defined $type and $type =~ /polyline/i);

  @$self{'object', 'subtype', 'forwardarrow', 'backwardarrow'} =
        ('spline', 1, 0, 0)
    if (defined $type and $type =~ /spline/i);

  @$self{'object', 'subtype', 'color', 'font', 'fontsize', 'angle',
         'fontflags', 'height', 'length', 'x', 'y'} = 
     ('text', 0, -1, 0, 12, 0.0, 0, 0, 0, 0, 0) 
   if (defined $type and $type =~ /text/i);

  @$self{'object', 'subtype', 'direction', 'forwardarrow',
          'backwardarrow', 'centerx', 'centery',
          'x1', 'y1', 'x2', 'y2', 'x3', 'y3'} = ('arc', 0, 0, 0, 0, 0, 0, 
          0, 0, 0, 0, 0, 0) if (defined $type and $type =~ /arc/i);

  bless $self, ref $class || $class;
  return $self;
}

sub parsefile {
  my $self     = shift;
  my $filename = shift;
  my ($line, $ver, @object);
  
  $self->{object} = -1;
  open IN, "<$filename" || return;
  $self->{filename} = $filename;

  $self->parseHead(\*IN);
  $self->parseBody(\*IN);

  close IN;
}

sub parseHead {
  my $self = shift;
  my $fh   = shift;

  # Read Version Information
  $line = <$fh>;
  return unless ($line =~ /^#FIG\s+([\d\.]+)/);
  $self->{version} = $ver = $1;
  undef $self->{parent};

  # Read Header Information
  $self->{orientation}   = nextline($fh) if ($ver >= 3.1);
  $self->{justification} = nextline($fh) if ($ver >= 3.1);
  $self->{units}         = nextline($fh) if ($ver >= 3.1);
  $self->{papersize}     = nextline($fh) if ($ver >= 3.2);
  $self->{magnification} = nextline($fh) if ($ver >= 3.2);
  $self->{multiplepage}  = nextline($fh) if ($ver >= 3.2);
  $self->{transparent}   = nextline($fh) if ($ver >= 3.2);
  $line = nextline($fh);
  # Note: comment lines can appear before the resolution and coordsystem line
  while( $line =~ s%^#%% ){ 
    push @{$self->{comment}}, $line;
    $line = nextline($fh);
  }
  ($self->{resolution}, $self->{coordsystem}) = split ' ', $line;
}

sub parseBody {
  my $self    = shift;
  my $fh      = shift;
  my ($element, @coord, @object, $otype, $npoints);

  $self->{object} = 'compound'; # Compound
  $self->{elements} = [];
  @comment = ();

OBJECT:  while ($line = nextline($fh)) {
    return if ($line =~ /^-6/);

    $element = $self->new();

    @object = split ' ', $line;
    $otype = shift @object;

    # Added by David to handle comments
    # Assumed that they refer to the following element

    do { # Comments
	$line =~ s%^#%%;  # Remove leading #
	push @comment, $line; # Accumulate comments as there may be more than one
        goto OBJECT;
    } if ( $otype eq "#" );

    do { # Color Pseudo Object
      @$element{'object', 'color', 'rgb'} = ('color', @object);
      goto REGISTER;
    } if ($otype == 0);

    do { # Ellipse
      @$element{'object', 'subtype', 'linestyle', 'thickness',
		'pencolor', 'fillcolor', 'depth', 'penstyle', 'areafill',
		'styleval', 'direction', 'angle', 'cx', 'cy', 'rx', 'ry',
		'sx', 'sy', 'ex', 'ey'} = ('ellipse', @object);
      goto REGISTER;
    } if ($otype == 1);

    do { # Polyline
      (@$element{'object', 'subtype', 'linestyle', 'thickness',
		 'pencolor', 'fillcolor', 'depth', 'penstyle', 'areafill',
		 'styleval', 'joinstyle', 'capstyle', 'radius',
		 'forwardarrow', 'backwardarrow'}, $npoints) =
        ('polyline', @object);
      @$element{'flipped', 'file'} = split ' ', nextline($fh)
        if ($element->{subtype} == 5);
      @$element{'forwardarrow_type', 'forwardarrow_style', 
                  'forwardarrow_thickness', 'forwardarrow_width',
                  'forwardarrow_height'} = split ' ', nextline($fh) 
        if ($element->{forwardarrow});     
      @$element{'backwardarrow_type', 'backwardarrow_style', 
                  'backwardarrow_thickness', 'backwardarrow_width',
                  'backwardarrow_height'} = split ' ', nextline($fh) 
        if ($element->{backwardarrow});     


      @coord = ();
      foreach $i (1 .. $npoints) {
         @coord = split(' ', nextline($fh)) unless (@coord);
         push @{$element->{points}}, [shift @coord, shift @coord];
      }
      goto REGISTER;
    } if ($otype == 2);

    do { # Spline
      (@$element{'object', 'subtype', 'linestyle', 'thickness',
                'pencolor', 'fillcolor', 'depth', 'penstyle', 'areafill',
                'styleval', 'capstyle', 'forwardarrow',
                'backwardarrow'}, $npoints) = ('spline', @object);
      @$element{'forwardarrow_type', 'forwardarrow_style', 
                  'forwardarrow_thickness', 'forwardarrow_width',
                  'forwardarrow_height'} = split ' ', nextline($fh) 
        if ($element->{forwardarrow});     
      @$element{'backwardarrow_type', 'backwardarrow_style', 
                  'backwardarrow_thickness', 'backwardarrow_width',
                  'backwardarrow_height'} = split ' ', nextline($fh) 
        if ($element->{backwardarrow});     
      @coord = ();
      foreach $i (1 .. $npoints) {
        @coord = split(' ', nextline($fh)) unless (@coord);
        push @{$element->{points}}, [shift @coord, shift @coord];
      }
      @coord = ();
#      if ($element->{subtype} > 1) {
        for $i (1 .. $npoints) {
          @coord = split(' ', nextline($fh)) unless (@coord);
          push @{$element->{ctrlnumbers}}, (shift @coord);
        }
#      }
      goto REGISTER;
    } if ($otype == 3);

    do { # Text
      my $text;
      (@$element{'object', 'subtype', 'color', 'depth', 'penstyle', 'font',
		 'fontsize', 'angle', 'fontflags', 'height', 'length',
		 'x', 'y'},
       $text) = split ' ', $line, 14;
      $element->{object} = 'text';
      $text =~ m/^(.*)\\001/;
      $element->{text} = $1;
      goto REGISTER;
    } if ($otype == 4);

    do { # Arc
      @$element{'object', 'subtype', 'linestyle', 'thickness',
		'pencolor', 'fillcolor', 'depth', 'penstyle', 'areafill',
		'styleval', 'capstyle', 'direction', 'forwardarrow',
		'backwardarrow', 'centerx', 'centery',
		'x1', 'y1', 'x2', 'y2', 'x3', 'y3'} = ('arc', @object);
      @$element{'forwardarrow_type', 'forwardarrow_style', 
                  'forwardarrow_thickness', 'forwardarrow_width',
                  'forwardarrow_height'} = split ' ', nextline($fh) 
        if ($element->{forwardarrow});     
      @$element{'backwardarrow_type', 'backwardarrow_style', 
                  'backwardarrow_thickness', 'backwardarrow_width',
                  'backwardarrow_height'} = split ' ', nextline($fh) 
        if ($element->{backwardarrow});     
     goto REGISTER;
    } if ($otype == 5);

    do { # Compound
      @$element{'object', 'urx', 'ury', 'llx', 'lly'} = ('compound', @object);
      $element->parseBody($fh);
    } if ($otype == 6);

REGISTER:
    $element->{parent} = $self;
    # Save comments as feature on the element
    $element->{comment} = [ @comment ] if (@comment);
    @comment = ();
    push @{$self->{elements}}, $element;
  }
}

sub writefile {
  my $self = shift;
  my $filename = shift || $self->{filename};

  # allow pipes
  $filename =~ s/^([^|>])/>$1/;

  open OUT, "$filename" || die "Cannot open file $filename";
  $self->{filename} = $filename;

  $self->writeHead(\*OUT);
  foreach (@{$self->{'elements'}}) {
    $_->writeBody(\*OUT);
  }

  close OUT;
}

sub writeHead {
  my $self = shift;
  my $fh   = shift;
  my $ver  = $self->Version();

  print $fh "#FIG ", $ver, "\n";
  print $fh $self->Orientation(), "\n"   if ($ver >= 3.1);
  print $fh $self->Justification(), "\n" if ($ver >= 3.1);
  print $fh $self->Units(), "\n"         if ($ver >= 3.1);
  print $fh $self->Papersize(), "\n"     if ($ver >= 3.2);
  print $fh $self->Magnification(), "\n" if ($ver >= 3.2);
  print $fh $self->MultiplePage(), "\n"  if ($ver >= 3.2);
  print $fh $self->Transparent(), "\n"   if ($ver >= 3.2);

  if ( defined $self->{comment} ) {
    if (ref $self->{comment}) {
    	foreach ( @{ $self->{comment} } ) {
    	  printf $fh "#$_\n";
	}
    } else {
    	printf $fh "#$self->{comment}\n";
    }
  }
  print $fh $self->Resolution(), " ", $self->CoordSystem(), "\n"
                                         if ($ver >= 3.1);
}

sub writeBody {
  my $self = shift;
  my $fh   = shift;
  my $otype = $self->{object};

  if ( defined $self->{comment} ) {
    if (ref $self->{comment}) {
    	foreach ( @{ $self->{comment} } ) {
    	  printf $fh "#$_\n";
	}
    } else {
    	printf $fh "#$self->{comment}\n";
    }
  }

  do { # Color Pseudo Object
    printf $fh "0 %i %s\n", @$self{'color', 'rgb'};
  } if ($otype =~ /color/i);

  do { # Ellipse
    printf $fh "1 %i %i %i %i %i %i %i %i %.3f %i %.3f %i %i %i %i %i %i %i %i\n", 
  @$self{'subtype', 'linestyle', 'thickness', 'pencolor', 'fillcolor',
         'depth', 'penstyle', 'areafill', 'styleval', 'direction',
         'angle', 'cx', 'cy', 'rx', 'ry', 'sx', 'sy', 'ex', 'ey'};
  } if ($otype =~ /ellipse/i);

  do { # Polyline
    $npoints = @{$self->{points}};
    printf $fh "2 %i %i %i %i %i %i %i %i %.3f %i %i %i %i %i %i\n",
      (@$self{'subtype', 'linestyle', 'thickness',
              'pencolor', 'fillcolor', 'depth', 'penstyle', 'areafill',
              'styleval', 'joinstyle', 'capstyle', 'radius'},
       ($self->{forwardarrow})?1:0, ($self->{backwardarrow})?1:0,
       $npoints);
    printf $fh "\t%i %i %.3f %.3f %.3f\n", 
      (@$self{'forwardarrow_type', 'forwardarrow_style', 
                  'forwardarrow_thickness', 'forwardarrow_width',
                  'forwardarrow_height'}) 
      if ($self->{forwardarrow});
    printf $fh "\t%i %i %.3f %.3f %.3f\n", 
      (@$self{'backwardarrow_type', 'backwardarrow_style', 
                  'backwardarrow_thickness', 'backwardarrow_width',
                  'backwardarrow_height'}) 
      if ($self->{backwardarrow});
    printf $fh "\t%i %s\n", @$self{'flipped', 'file'}
      if ($self->{subtype} == 5);
    for $i (0 .. $npoints-1) {
      printf $fh "\t%i %i\n", @{$self->{points}->[$i]};
    }
  } if ($otype =~ /polyline/i);

  do { # Spline
    $npoints = @{$self->{points}};
    printf $fh "3 %i %i %i %i %i %i %i %i %.3f %i %i %i %i\n",
      (@$self{'subtype', 'linestyle', 'thickness',
              'pencolor', 'fillcolor', 'depth', 'penstyle', 'areafill',
              'styleval', 'capstyle'},
       ($self->{forwardarrow})?1:0, ($self->{backwardarrow})?1:0,
       $npoints);
    printf $fh "\t%i %i %.3f %.3f %.3f\n", 
      (@$self{'forwardarrow_type', 'forwardarrow_style', 
                  'forwardarrow_thickness', 'forwardarrow_width',
                  'forwardarrow_height'}) 
      if ($self->{forwardarrow});
    printf $fh "\t%i %i %.3f %.3f %.3f\n", 
      (@$self{'backwardarrow_type', 'backwardarrow_style', 
                  'backwardarrow_thickness', 'backwardarrow_width',
                  'backwardarrow_height'}) 
      if ($self->{backwardarrow});
    for $i (0 .. $npoints-1) {
      printf $fh "\t%i %i\n", @{$self->{points}->[$i]};
    }
    for $i (0 .. $npoints-1) {
      printf $fh "\t%.3f\n", $self->{ctrlnumbers}->[$i];
    }
  } if ($otype =~ /spline/i);

  do { # Text
    printf $fh "4 %i %i %i %i %i %.3f %.3f %i %.3f %.3f %i %i %s\\001\n",
      @$self{'subtype', 'color', 'depth', 'penstyle', 'font', 'fontsize',
             'angle', 'fontflags', 'height', 'length', 'x', 'y', 'text'};
  } if ($otype =~ /text/i);

  do { # Arc
    printf $fh "5 %i %i %i %i %i %i %i %i %.3f %i %i %i %i %.3f %.3f %i %i %i %i %i %i\n",
      (@$self{'subtype', 'linestyle', 'thickness', 'pencolor', 'fillcolor',
	      'depth', 'penstyle', 'areafill', 'styleval', 'capstyle',
	      'direction'},
       ($self->{forwardarrow})?1:0, ($self->{backwardarrow})?1:0,
       @$self{'centerx', 'centery', 'x1', 'y1', 'x2', 'y2', 'x3', 'y3'});
    printf $fh "\t%i %i %.3f %.3f %.3f\n", 
      (@$self{'forwardarrow_type', 'forwardarrow_style', 
                  'forwardarrow_thickness', 'forwardarrow_width',
                  'forwardarrow_height'}) 
      if ($self->{forwardarrow});
    printf $fh "\t%i %i %.3f %.3f %.3f\n", 
      (@$self{'backwardarrow_type', 'backwardarrow_style', 
                  'backwardarrow_thickness', 'backwardarrow_width',
                  'backwardarrow_height'}) 
      if ($self->{backwardarrow});

  } if ($otype =~ /arc/i);

  do { # Compound
    printf $fh "6 %i %i %i %i\n", @$self{'urx', 'ury', 'llx', 'lly'};
    foreach $element (@{$self->{elements}}) {
      $element->writeBody($fh);
    }
    print $fh "-6\n";
  } if ($otype =~ /compound/i);
}

sub Top {
  my $self = shift;
  if ($self->{parent}) {
    return $self->{parent}->Top();
  } else {
    return $self;
  }
}

sub Up {
  my $self = shift;
  if ($self->{parent}) {
    return $self->{parent};
  } else {
    return $self;
  }
}

sub eachPrimitive {
  my $self = shift;
  if ($self->{object} =~ /compound/i) { # Compound
    return grep {$_->isPrimitive()} @{$self->{elements}};
  } else {
    return ($self);
  }
}

sub eachCompound {
  my $self = shift;
  if ($self->{object} =~ /compound/i) { # Compound
    return grep {$_->isCompound()} @{$self->{elements}};
  } else {
    return ();
  }
}

sub isCompound {
  my $self = shift;
  return $self->{object} =~ /compound/i;
}

sub isPrimitive {
  my $self = shift;
  return !$self->isCompound();
}

sub add {
  my $self = shift;
  my $element = shift;

  die "Cannot add element to non-compound" unless ($self->isCompound());
 
  push @{$self->{elements}}, $element;
}

sub delete {
  my $self = shift;
  my $parent = $self->{parent};

  @{$parent->{elements}} = grep { $self ne $_ }
    @{$parent->{elements}};

  undef $self->{parent};
}

sub Version {
  my $self = shift;
  return $self->Top()->{version};
}

sub Orientation {
  my $self = shift;
  return $self->Top()->{orientation};
}

sub Justification {
  my $self = shift;
  return $self->Top()->{justification};
}

sub Units {
  my $self = shift;
  return $self->Top()->{units};
}

sub Papersize {
  my $self = shift;
  return $self->Top()->{papersize};
}

sub Magnification {
  my $self = shift;
  return $self->Top()->{magnification};
}

sub MultiplePage {
  my $self = shift;
  return $self->Top()->{multiplepage};
}

sub Transparent {
  my $self = shift;
  return $self->Top()->{transparent};
}

sub Resolution {
  my $self = shift;
  return $self->Top()->{resolution};
}

sub CoordSystem {
  my $self = shift;
  return $self->Top()->{coordsystem};
}

sub isColor {
  my $self = shift;
  return $self->{object} =~ /color/i;
}

sub isArc {
  my $self = shift;
  return $self->{object} =~ /arc/i;
}

sub isEllipse {
  my $self = shift;
  return $self->{object} =~ /ellipse/i;
}

sub isPolyline {
  my $self = shift;
  return $self->{object} =~ /polyline/i;
}

sub isSpline {
  my $self = shift;
  return $self->{object} =~ /spline/i;
}

sub isText {
  my $self = shift;
  return $self->{object} =~ /text/i;
}

sub isClosed {
  my $self = shift;
  return $self->isPolyline() &&
         (($self->{'subtype'} == 2) || ($self->{'subtype'} == 3));
}

sub nextline {
  my $fh = shift;
  my $line = '#';
  if (eof $fh) { undef $line; return; };
# (Original)  while ( ($line =~ m/^(#.*)$/) && !eof $fh ) { $line = <$fh>; };
  $line = <$fh>;
  chomp $line;
  $line =~ s/\\\\/\\/g;
  return $line;
}
  
1;
