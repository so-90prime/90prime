# Steward Observatory Autoguider Software
# Written: GDS 3/24/00

# ggw - 6/22/01
#	Load the necessary shared libraries.
# 	The syntax is: load library package ?interp?.
#	where library is the shared library, package corresponds to
#	the package_Init c procedure called to initialize the package.
#	Note: the first letter of package in the c routine must
#	be upper case and the remaining should be lower case, however
#	the Tcl load command automatically sets the case so that
#	Package=pAcKage=package. ?interp? is an optional interpreter.
#
#	One could imagine having more than one single package in a single
#	c program (which gets compiled and linked into the .so library),
#	for instance Addition_Init, Multiplication_Init, Trigonometry_Init, 
#	etc.  But to be efficient one only needs to load the package which
#	initializes the routines you will use. 
#	
load ./addnums.so Addition

# Set version number
set version 1.0

# Set window title
wm title . "Steward Observatory Autoguider System (vers. $version)"

# Initialize display
wm withdraw .
wm geometry . +0+0
wm deiconify .

# Define fonts
font create bf8 -family "MS Sans Serif" -weight bold -size 8
font create bf8u -family "MS Sans Serif" -weight bold -size 8 -underline true

# Define images
#image create photo acqimage -file c:/gary/guider/software/amher.gif
image create photo acqimage -file /home/ggwilli/90prime/guider/gui/mw512.gif
label .aimage -image acqimage
image create photo guideimage
label .gimage -image guideimage

# Variables -------------------------------------------------------------------

# Set acquisition area variables
set xs 512
set ys 512
set xoff 45
set yoff 25
set xhs [expr $xs / 2]
set yhs [expr $ys / 2]
set xac [expr $xhs + $xoff]
set yac [expr $yhs + $yoff]
set bs 61
set bsp $bs
set bhs [expr $bs / 2]
set xp $xac
set yp $yac
set xm $xp
set ym $yp
set acquired 0

# Set boundaries of real image
set xps [image width acqimage]
set yps [image height acqimage]
set xlo [expr $xac - $xps / 2]
set xhi [expr $xac + $xps / 2]
set ylo [expr $yac - $yps / 2]
set yhi [expr $yac + $yps / 2]
set xc [expr $xps / 2]
set yc [expr $yps / 2]

# Set guide area variables
set gs 101
set ghs [expr $gs / 2]
set xgc [expr $xoff + $xs + $gs / 2 + 5]
set ygc [expr $yoff + $gs / 2 + 1]

# Exposure time
set exptime 1
set expsel 1

# Guide parameters
set filter .6
set deltime 0
set movelim 1.0

# Communications channel
set netflag 0
set serflag 1

# Centroid output
# ggw - 6/22/01
#	Rather than setting xbar and ybar they are now calculated in
#	the addnums routine and because they are global variables in that
#	routine and in Tcl they can be used after the call to addnums.
#	However it appears they must be initilized to for instance 0.0.
#	The return from addnums (and oaddnums) is set to a dummy
#	variable for now.
#
#set xbar 252.5432
#set ybar 251.5678
#set xbar [sums 5.23 30.14]
#set ybar [osums 100.31 -46.77]
#set xbar 0.0
#set ybar 0.0
set num1 -16.38
set num2 22.43
set dum1 [sums $num1 $num2]
set dum2 [osums $num1 $num2]
#
set xdel -1.2534
set ydel 1.6789
set xcorr 0.1543
set ycorr -0.2655
set xrms 1.3456
set yrms 0.4567

# Image info
set fwhm 1.8901
set imagescale 0.1234
set starflux 14324.3
set backflux 323.5

# Camera info
set ccdtemp -65.2
set dewtemp 35.3
set camstat "IDLE"

# Guide or no-guide 1/0
set guidestat 0
set blinkflag 0

# Define overall canvas -------------------------------------------------------

canvas .layout -width [expr $xoff + $xs + $gs + 245] -height [expr $yoff + $ys + 100]
.layout configure -cursor crosshair

# Create bounding box for acquisition image
.layout create rectangle $xoff $yoff [expr $xoff + $xs] [expr $yoff + $ys] \
               -outline white -width 2 -fill black

# Create bounding box for guide image
.layout create rectangle [expr $xgc - $ghs] [expr $ygc - $ghs] \
                [expr $xgc + $ghs] [expr $ygc + $ghs] \
                -outline white -tag guidebox -width 4 -fill black

# Define widgets --------------------------------------------------------------

# label for acquisition area
label .acqlab -text "Acquisition" -font bf8u -anchor w

# label for guide area
label .guidelab -text "Guiding" -font bf8u -anchor w

# acquisition box slider
scale .box -from 21 -to 101 -variable bs -resolution 1 -tickinterval 80 \
      -orient horizontal -length 300 -showvalue yes -command { acqboxsize }

# label for acq. slider
label .lbox -text "Acq. box size:"

# acquire button
button .acquire -width 8 -height 2 -text "Acquire" -background red -font bf8 -command { acquire }

# start/stop button
button .startstop -width 8 -height 2 -text "Guide" -background red -font bf8 -command { startstop }

# x coord in acq. image
entry .xc -width 4 -state disabled -justify center -text xc

# y coord in acq. image
entry .yc -width 4 -state disabled -justify center -text yc

# message
entry .message -width 70 -font bf8 -state disabled -background #FFFFAA -text msg

# exposure time slider
scale .expslide -from 10 -to 0 -tickinterval 2 -variable expsel -resolution 0.5 \
      -digits 3 -length 210 -showvalue no -command exptimeset

# exposure time entry
entry .exposure -width 5 -state normal -justify r -textvariable exptime

# exposure time slider label
label .explabel -font bf8u -text "Exposure (s)"

# NSEW guide indicators
radiobutton .nind -activeforeground red -foreground white
radiobutton .sind -activeforeground red -foreground white
radiobutton .eind -activeforeground red -foreground white
radiobutton .wind -activeforeground red -foreground white
.nind select
.sind select
.eind select
.wind select
label .nindlab -text "N"
label .sindlab -text "S"
label .eindlab -text "E"
label .windlab -text "W"

# UpDownLeftRight movement buttons
button .moveu -font bf8 -width 2 -text "U" -state disabled -command { guide "n" }
button .moved -font bf8 -width 2 -text "D" -state disabled -command { guide "s" }
button .movel -font bf8 -width 2 -text "L" -state disabled -command { guide "e" }
button .mover -font bf8 -width 2 -text "R" -state disabled -command { guide "w" }

# Centroid display
label .centhead -text "Centroiding" -anchor c -font bf8u
label .xhead -text "x" -anchor c
label .yhead -text "y" -anchor c
label .centlab -text "Centroid (pix):" -anchor w
label .xcent -textvariable xbar -anchor e -width 5
label .ycent -textvariable ybar -anchor e -width 5
label .dellab -text "Offset (pix):" -anchor w
label .xdel -textvariable xdel -anchor e -width 5
label .ydel -textvariable ydel -anchor e -width 5
label .corrlab -text "Correction (RA,Dec ''):" -anchor w
label .xcorr -textvariable xcorr -anchor e -width 5
label .ycorr -textvariable ycorr -anchor e -width 5
label .rmslab -text "Guide rms (''):" -anchor w
label .xrms -textvariable xrms -anchor e -width 5
label .yrms -textvariable yrms -anchor e -width 5

# Image characteristics
label .imhead -text "Image" -anchor c -font bf8u
label .fwhmlab -text "FWHM (''):" -anchor w
label .fwhm -textvariable fwhm -anchor e -width 5
label .starfluxlab -text "Star flux:" -anchor w
label .starflux -textvariable starflux -anchor e -width 5
label .backfluxlab -text "Background flux:" -anchor w
label .backflux -textvariable backflux -anchor e -width 5

label .imagescalelab -text "Scale (''/pix):" -anchor w
label .imagescale -textvariable imagescale -anchor e -width 5

# Camera status
label .camhead -text "Camera" -anchor c -font bf8u
label .ccdtemplab -text "CCD Temp (C)" -anchor w
label .ccdtemp -textvariable ccdtemp -anchor e -width 5
label .dewtemplab -text "Dewar Temp (C)" -anchor w
label .dewtemp -textvariable dewtemp -anchor e -width 5
label .camstatlab -text "Status" -anchor w
label .camstat -textvariable camstat -anchor e -width 10

# Menubar ---------------------------------------------------------------------

menu .menubar -type menubar
. configure -menu .menubar
.menubar add cascade -label File -menu .menubar.file -underline 0
  menu .menubar.file -tearoff no
  .menubar.file add command -label "Save As..." -command { }
  .menubar.file add separator
  .menubar.file add command -label Minify -command { wm iconify . }
  .menubar.file add command -label Exit -command { exit }
.menubar add cascade -label "Edit" -menu .menubar.edit -underline 0
  menu .menubar.edit -tearoff no
  .menubar.edit add command -label "Edit guide.tcl" \
     -command { exec c:\\progra~1\\access~1\\wordpad.exe c:\\gary\\guider\\software\\guide.tcl }
.menubar add cascade -label "Setup" -menu .menubar.setup -underline 0
  menu .menubar.setup -tearoff no
  .menubar.setup add command -label "Set xy <--> RA/Dec transformation" -command { transform }
  .menubar.setup add command -label "Show RA/Dec Axes" -command { }
  .menubar.setup add separator
  .menubar.setup add command -label "Initialize camera" -command { }
.menubar add cascade -label Communications -menu .menubar.talk -underline 0
  menu .menubar.talk -tearoff no
  .menubar.talk add checkbutton -label Network -variable netflag
  .menubar.talk add checkbutton -label Serial -variable serflag
.menubar add cascade -label Parameters -menu .menubar.parm -underline 0
  menu .menubar.parm -tearoff no
  set editparms .parmedit
  .menubar.parm add command -label "Modify guide parameters" -command { editparms .editparms }

# Place widgets ---------------------------------------------------------------

pack .layout
place configure .acqlab -x $xoff -y [expr $yoff - 2] -anchor sw
place configure .guidelab -x [expr $xgc - $ghs] -y [expr $yoff - 2] -anchor sw
place configure .box -x $xac -y [expr $yoff + $ys + 35] -anchor c
place configure .lbox -x [expr $xac - 160] -y [expr $yoff + $ys + 35] -anchor e
place configure .acquire -x [expr $xs + $gs + 140] -y [expr $ys + $yoff + 69] -anchor c
place configure .startstop -x [expr $xs + $gs + 230] -y [expr $ys + $yoff + 69] -anchor c
place configure .xc -x $xac -y 10 -anchor c
place configure .yc -x 5 -y $yac -anchor w
place configure .message -x $xoff -y [expr $ys + $yoff + 80] -anchor w

set cx [expr $xgc - $ghs + 50]
set cy [expr $ygc + $ghs + 155]
place configure .explabel -x $cx -y $cy -anchor n
place configure .exposure -x $cx -y [expr $cy + 20] -anchor n
place configure .expslide -x $cx -y [expr $cy + 45] -anchor n

set cx [expr $cx + 5]
set cy [expr $cy - 137]
place configure .nind -x $cx -y $cy -anchor c
place configure .sind -x $cx -y [expr $cy + 60] -anchor c
place configure .eind -x [expr $cx - 30] -y [expr $cy + 30] -anchor c
place configure .wind -x [expr $cx + 30] -y [expr $cy + 30] -anchor c

set cx [expr $cx - 5]
place configure .nindlab -x $cx -y [expr $cy + 17] -anchor c
place configure .sindlab -x $cx -y [expr $cy + 40] -anchor c
place configure .eindlab -x [expr $cx - 14] -y [expr $cy + 31] -anchor c
place configure .windlab -x [expr $cx + 13] -y [expr $cy + 31] -anchor c

set cx [expr $cx + 2]
set cy [expr $cy + 87]
place configure .moveu -x $cx -y $cy -anchor c
place configure .moved -x $cx -y [expr $cy + 30] -anchor c
place configure .movel -x [expr $cx - 33] -y [expr $cy + 15] -anchor c
place configure .mover -x [expr $cx + 33] -y [expr $cy + 15] -anchor c

set cx [expr $xgc + $ghs + 190]
set cy [expr $ygc - $ghs - 13]
place configure .centhead -x [expr $cx - 180] -y $cy -anchor w
place configure .xhead -x [expr $cx - 20] -y $cy -anchor c
place configure .yhead -x [expr $cx + 30] -y $cy -anchor c
place configure .centlab -x [expr $cx - 180] -y [expr $cy + 20] -anchor w
place configure .xcent -x $cx -y [expr $cy + 20] -anchor e
place configure .ycent -x [expr $cx + 50] -y [expr $cy + 20] -anchor e
place configure .dellab -x [expr $cx - 180] -y [expr $cy + 40] -anchor w
place configure .xdel -x $cx -y [expr $cy + 40] -anchor e
place configure .ydel -x [expr $cx + 50] -y [expr $cy + 40] -anchor e
place configure .corrlab -x [expr $cx - 180] -y [expr $cy + 60] -anchor w
place configure .xcorr -x $cx -y [expr $cy + 60] -anchor e
place configure .ycorr -x [expr $cx + 50] -y [expr $cy + 60] -anchor e
place configure .rmslab -x [expr $cx - 180] -y [expr $cy + 80] -anchor w
place configure .xrms -x $cx -y [expr $cy + 80] -anchor e
place configure .yrms -x [expr $cx + 50] -y [expr $cy + 80] -anchor e

set cy [expr $cy + 120]
place configure .imhead -x [expr $cx - 180] -y $cy -anchor w
place configure .fwhmlab -x [expr $cx - 180] -y [expr $cy + 20] -anchor w
place configure .fwhm -x [expr $cx + 50] -y [expr $cy + 20] -anchor e
place configure .imagescalelab -x [expr $cx - 180] -y [expr $cy + 40] -anchor w
place configure .imagescale -x [expr $cx + 50] -y [expr $cy + 40] -anchor e
place configure .starfluxlab -x [expr $cx - 180] -y [expr $cy + 60] -anchor w
place configure .starflux -x [expr $cx + 50] -y [expr $cy + 60] -anchor e
place configure .backfluxlab -x [expr $cx - 180] -y [expr $cy + 80] -anchor w
place configure .backflux -x [expr $cx + 50] -y [expr $cy + 80] -anchor e

set cy [expr $cy + 120]
place configure .camhead -x [expr $cx - 180] -y $cy -anchor w
place configure .ccdtemplab -x [expr $cx - 180] -y [expr $cy + 20] -anchor w
place configure .ccdtemp -x [expr $cx + 50] -y [expr $cy + 20] -anchor e
place configure .dewtemplab -x [expr $cx - 180] -y [expr $cy + 40] -anchor w
place configure .dewtemp -x [expr $cx + 50] -y [expr $cy + 40] -anchor e
place configure .camstatlab -x [expr $cx - 180] -y [expr $cy + 60] -anchor w
place configure .camstat -x [expr $cx + 50] -y [expr $cy + 60] -anchor e

# Bindings --------------------------------------------------------------------

bind .layout <Button-1> {
         set xm %x
         set ym %y
         acqboxselect }

bind .exposure <Key-Return> {
     global exptime expsel
     if { $exptime >= 0 && $exptime <= 600 } {
        putmsg ""
        enable .acquire
        enable .startstop
        set expsel $exptime} else {
        set exptime $expsel
        putmsg "Exposure time limits: 0 to 600 sec!"
        disable .acquire
        disable .startstop}
}

# Set exposure time
proc exptimeset { input } {
     global exptime
     set exptime $input
     putmsg ""
     enable .acquire
     enable .startstop
}

# Set acquisition box to odd number of pixels
proc acqboxsize { input } {
     global bs bhs bsp
     if {$input < $bsp} {set bs [expr $bsp - 2]}
     if {$input > $bsp} {set bs [expr $bsp + 2]}
     set bhs [expr $bs / 2]
     set bsp $bs
     acqboxselect
}

proc transform { } {
}

# Define acquisition image and create acquisition box
proc acquire { } {
     global xac yac acquired
     .layout create image $xac $yac -anchor c -image acqimage -tag sky
     drawacqbox
     putmsg ""
     set acquired 1
}

# Start/stop guiding
proc startstop { } {
     global guidestat xgc ygc guideimage bhs xc yc ghs acquired blinkflag exptime
     if { $acquired == 0 } {
        putmsg "Image not yet acquired!" } else {
     if { $guidestat } {
# Stop guiding process
        set guidestat 0
        set blinkflag 0
        enable .acquire
        enable .expslide
        enable .exposure
        enable .box
        disable .moveu
        disable .moved
        disable .movel
        disable .mover
        .startstop configure -background red -text "Guide"
        .layout itemconfigure guidebox -outline white } else {
# Start guiding process
        if { $exptime > 0 } {
        set guidestat 1
        set blinkflag 1
        disable .acquire
        disable .expslide
        disable .exposure
        disable .box
        enable .moveu
        enable .moved
        enable .movel
        enable .mover
        guideimage blank
        guideimage copy acqimage \
                   -from [expr $xc - $bhs] [expr $yc - $bhs] [expr $xc + $bhs] [expr $yc + $bhs] \
                   -to [expr $ghs - $bhs] [expr $ghs - $bhs]
        .layout create image [expr $xgc - $ghs] [expr $ygc - $ghs] -anchor nw -image guideimage
        crosshairs
        .startstop configure -background green -text "Guiding"
        itemblink .layout guidebox -outline "white" "black" 500 } else {
        putmsg "Guide exposure time must be > 0 sec!" }
        }
        }
}

# Draw box on acquisition window
proc drawacqbox { } {
     global xp yp bhs
     .layout delete acqbox
     .layout create rectangle [expr $xp - $bhs] [expr $yp - $bhs] \
                              [expr $xp + $bhs] [expr $yp + $bhs] -outline white -tag acqbox
}

# Blink/unblink text in message widget
proc putmsg { input } {
     global msg blinkflag
     if { $input == "" } {
        set msg ""
        set blinkflag 0
        blink .message -foreground black black 400 } else {
        set msg $input
        set blinkflag 1
        blink .message -foreground black #FFFFAA 400 }
}

# Draw crosshairs on guider window
proc crosshairs { } {
     global xgc ygc gs
     .layout create line $xgc [expr $ygc - $gs * .35] $xgc [expr $ygc - $gs * .15] -fill white
     .layout create line $xgc [expr $ygc + $gs * .15] $xgc [expr $ygc + $gs * .35] -fill white
     .layout create line [expr $xgc - $gs * .35] $ygc [expr $xgc - $gs * .15] $ygc -fill white
     .layout create line [expr $xgc + $gs * .15] $ygc [expr $xgc + $gs * .35] $ygc -fill white
}

# Select region of acquisition box to use for guiding
proc acqboxselect { } {
         global xm ym xlo xhi ylo yhi bhs xp yp xc yc guidestat
     if {!$guidestat } {
         if {$xm < $xlo || $xm > $xhi || $ym < $ylo || $ym > $yhi } { } else {
         if {$xm > $xlo + $bhs && $xm < $xhi - $bhs && $ym > $ylo + $bhs && $ym < $yhi - $bhs } {
           drawacqbox
           .layout move acqbox [expr $xm - $xp] [expr $ym - $yp]
           set xp $xm
           set yp $ym
           set xc [expr $xm - $xlo]
           set yc [expr $ym - $ylo]
           putmsg ""
           enable .startstop
           .startstop configure -background red } else {
           set xm $xp
           set ym $yp
           putmsg "Star too close to border.  Select new star or reduce box to continue."
           disable .startstop }
     } }
}

# Light up guide indicators
proc guide { input } {
     if {$input == "n"} { .nind flash }
     if {$input == "s"} { .sind flash }
     if {$input == "e"} { .eind flash }
     if {$input == "w"} { .wind flash }
}

# Update centroiding display
proc updatedisplay { } {
     global xbar ybar xdel ydel fwhm xrms yrms ccdtemp dewtemp imagescale starflux backflux xcorr ycorr
     roundvar xbar 2
     roundvar ybar 2
     roundvar xdel 2
     roundvar ydel 2
     roundvar xcorr 2
     roundvar ycorr 2
     roundvar xrms 2
     roundvar yrms 2
     roundvar fwhm 2
     roundvar ccdtemp 1
     roundvar dewtemp 1
     roundvar imagescale 3
     roundvar starflux 0
     roundvar backflux 0
}

# Toplevel edit window for guider parameters
proc editparms { w } {
     global deltime filter movelim guidestat
     catch { destroy $w }
     toplevel $w
     wm title $w "Modify guide parameters"
       # filter slider
     scale $w.filslide -from .1 -to .9 -tickinterval .1 -variable filter -resolution 0.1 \
           -digits 1 -length 300 -orient horizontal -showvalue yes -label "Motion Filter"
       # maximum move slider
    scale $w.movelim -from 0.1 -to 1.5 -tickinterval .2 -variable movelim -resolution 0.1 \
           -digits 2 -length 300 -orient horizontal -showvalue yes -label "Motion Limit ('')"
       # delay slider
     scale $w.delslide -from 0 -to 9 -tickinterval 1. -variable deltime -resolution 1 \
           -digits 1 -length 300 -orient horizontal -showvalue yes -label "Guide delay (s)"
     pack $w.filslide $w.movelim $w.delslide -pady 10 -side top
}

# Utility procedures ----------------------------------------------------------

# Return larger of the input variable or a lower limit
proc max { input limit } {
     upvar $input linkvar
     if { $linkvar < $limit } { set linkvar $limit }
}

# Return smaller of the input variable or an upper limit
proc min { input limit } {
     upvar $input linkvar
     if { $linkvar > $limit } { set linkvar $limit }
}

# Controlled blinking of a widget
# set blinkflag = 0 to stop blinking
proc blink { w option value1 value2 interval } {
     global blinkflag
     if { $blinkflag } {
     $w configure $option $value1
     after $interval [list blink $w $option $value2 $value1 $interval] }
}

# Controlled blinking of a canvas item
# set blinkflag = 0 to stop blinking
proc itemblink { w t option value1 value2 interval } {
     global blinkflag
     if { $blinkflag } {
     $w itemconfigure $t $option $value1
     after $interval [list itemblink $w $t $option $value2 $value1 $interval] }
}

# Enable widget
proc enable { w } {
     $w configure -state normal
}

# Disable widget
proc disable { w } {
     $w configure -state disabled
}

# Round variable to specified number of decimal points
proc roundvar { input digits } {
     upvar $input linkvar
     set digits [expr int($digits)]
     if { $digits < 1 } { set linkvar [expr round($linkvar)] } else {
     set fr [expr pow(10., $digits)]
     set linkvar [expr round($linkvar * $fr) / $fr] }
}

updatedisplay
