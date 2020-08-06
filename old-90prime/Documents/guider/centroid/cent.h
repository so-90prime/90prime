/**********************************************************************

   	cent.h - declarations for centroid.c.
 

	MAXFSIZE - the maximum number of pixels along one dimension.
 		This is kept at 255 to keep the total number of pixels
 		(MAXFSIZE squared) a short integer, i.e. 2 bytes.  
 
 	MINFSIZE - the minimum number of pixels along one dimension.
 		Can take any value but it should be large enough to
 		accurately determine the centroid.
 
 	MAXVAL - the maximum pixel intensity.  For a sixteen bit digitizer
 		MAXVAL = ((2^16) - 1) = 65535
 
 	FRAMEFRAC - the fraction of the user defined frame to use in
 		determining the centroid.  The user will typically define
 		a frame which contains enough sky pixels to accurately 
 		estimate the sky flux.  FRAMEFRAC should contain mostly
 		star pixels and thus is smaller than the user defined frame.

 	BINSIZE - the size of the histogram bins used in determing the
 		mode of the data pixels.  The mode is the sky flux. 
 
 	PI - pi.
 
   	modified 4-May-2001, G. Grant Williams
 
**********************************************************************/

#define MAXFSIZE	255  
#define MINFSIZE	5
#define MAXVAL		65535
#define FRAMEFRAC	0.5
#define BINSIZE		1
#define PI		3.141592654
