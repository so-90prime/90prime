/**********************************************************************

	centroid.c - computes the centroid of a stellar image using
		the intensity weighted mean of the marginal
		distributions.

	This function is a modified version of the Steward Observatory
	centroiding algorithm.  Its purpose is to provide a method  
	of telescope guiding with the new CCD guide camera.  The 
	original version was developed circa 1988 by Mike Lesser.

	The function accepts the number of pixels per axis (fsize, 
	square array assumed) and a pointer to the raw data
	array (frame).  The sky background (sky) is calculated as the
	mode of a histogram of pixels in the guide frame.  The sky is 
	subtracted from each pixel.  The 2 x 2 super-pixel with the
	greatest intensity is used to center a subframe of the data.
	This subframe has a size (centsize) which is a fraction
	(FRAMEFRAC) of the original framesize.  The marginal
	distributions in x and y are calculated (by collapsing
	the pixels through summation).  The centroid of
	x and y is calculated using the intensity weighted sums
	of the marginal distributions.

	NOTE: The CCD pixel values have be declared unsigned short
	to keep the definition consistent across OS platforms.  Other
	variables should be checked.

	modified 2-Sep-1988, MPL
	modified 10-Apr-2001, G. Grant Williams
        modified 24-Sep-2001, T. E. Pickering

**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cent.h"

unsigned short mode(int arrsize, unsigned short *arrvals, 
		    unsigned short bsize, int *hist);

/*
TEP
Simple function to do linear interpolation given two points
and a y value
*/

double interp (double y1, double y2, int x1, int x2, double y)
{
  double x;

  x = x2 + (x1-x2)*(y-y2)/(y1-y2);

  return x;

}    

int centroid(int fsize, unsigned short *frame, double *vals)
{

  int 	i, j, ix, iy, k, l;
  int 	*histogram;
  int 	centsize;
  int	supxcent, supycent, xorigin, yorigin;
  int	xmaxpix, ymaxpix;
  unsigned short  sky;
  unsigned short  binsize = BINSIZE; /* defined in cent.h */
  double 	x, y;
  double 	xvariance, yvariance;
  double 	xerr, yerr;
  long	numbins;
  long	xsum[fsize], ysum[fsize];

  /* 2 x 2 super-pixels are defined by there central vertices */

  long	superpix[fsize - 1][fsize - 1], maxsp, maxpix;
  long 	mxx, myy, mxy;

  /* framesum is the sum of the sky subtracted subframe pixels */

  long 	framesum;  
  double 	mxxnorm, myynorm, mxynorm;
  double 	asq, bsq;
  double 	ellipt, posang;
  double 	xweight, yweight;
  double 	xsqweight, ysqweight;
  double 	hm, fwhmx, fwhmy;

  /*
    Calculate the number of bins in a histogram given the maximum
    value and the binsize.  Adding one accounts for the zeroth bin.
    This also requires numbins to be type long because it could be
    65535 + 1
  */
 
  numbins = (((long) MAXVAL) / binsize) + 1;

  /*
    Dynamically allocate space for the histogram.  NOTE: calloc
    initializes all array values to zero.
  */

  histogram = (int *) calloc(numbins, sizeof(int)); 

  if (histogram == NULL)
    {
      fprintf(stderr, "Cannot allocate %ld histogram bins.\n",
	      numbins);
      return -1;
    }

  /*
    Call the mode function to return the most frequent
    pixel value and thus the sky background value.
  */

  sky = mode(fsize, frame, binsize, histogram);

  free(histogram);

  /*
    Locate the brightest 2 x 2 box and center on it.  Count
    vertices between super-pixels and thus the fsize - 1.
    A super-pixel should not contain pixels from the end of 
    one line and the begining of the next, i.e. wrapped.
  */

  for (k = 0, supxcent = supycent = maxsp = 0; k < fsize - 1; k++)
    for (l = 0, superpix[k][l] = 0; l < fsize - 1; l++)
      {
	superpix[k][l] = *(frame + k + (l * fsize)) + 
	  *(frame + (k + 1) + (l * fsize)) + 
	  *(frame + k + ((l + 1) * fsize)) + 
	  *(frame + (k + 1) + ((l + 1) * fsize));
	
	supxcent = (superpix[k][l] > maxsp) ? k : supxcent;
	supycent = (superpix[k][l] > maxsp) ? l : supycent;
	maxsp = (superpix[k][l] > maxsp) ? superpix[k][l] : maxsp;
      }

  /*
    Reduce the size of the box.
  */

  centsize = (int) (fsize * FRAMEFRAC);

  /*
    Find the pixel with the largest intensity in the super-pixel
    with the largest intensity.
  */

  for (i = supxcent, xmaxpix = ymaxpix = maxpix = 0; i <= supxcent + 1; i++)
    for (j = supycent; j <= supycent + 1; j++)
      {
	xmaxpix = (*(frame + i + (j * fsize)) > maxpix) ? i : xmaxpix;
	ymaxpix = (*(frame + i + (j * fsize)) > maxpix) ? j : ymaxpix;
	maxpix = (*(frame + i + (j * fsize)) > maxpix) ? 
	  *(frame + i + (j * fsize)) : maxpix;
      }

  /*
    Calculate where the marginal sums begin.  The centsize - 1
    works well for both even and odd size subframes.
  */

  xorigin = xmaxpix - ((centsize - 1) / 2);
  yorigin = ymaxpix - ((centsize - 1) / 2);

  /*
    Make sure the box fits within the original data.
  */

  if ((xorigin < 0) || (yorigin < 0) || (xorigin + centsize > fsize)
      || (yorigin + centsize > fsize))
    {
      fprintf(stderr, "The brightest super-pixel is too far from
		       the center of the box.\n");
      return -1;
    }
		
  /*
    Compute the sky subtracted marginals.
    xsum is the sum of all the x values along a row.
    ysum is the sum of all the y values along a given column.
    mxx, myy, and mxy are the unormalized second moments. 
  */

  for (i = 0, framesum = mxx = myy = mxy = 0; i < centsize; i++)
    for (j = 0, xsum[i] = ysum[i] = 0; j < centsize; j++)
      {
	xsum[i] += *(frame + (((i + yorigin) * fsize)
			      + (j + xorigin))) - sky;
	ysum[i] += *(frame + (((j + yorigin) * fsize)
			      + (i + xorigin))) - sky;
	framesum += *(frame + (((i + yorigin) * fsize)
			       + (j + xorigin))) - sky;
	mxx += (*(frame + (((i + yorigin) * fsize)
			   + (j + xorigin))) - sky) * j * j;
	myy += (*(frame + (((i + yorigin) * fsize)
			   + (j + xorigin))) - sky) * i * i;
	mxy += (*(frame + (((i + yorigin) * fsize)
			   + (j + xorigin))) - sky) * j * i;
      }

  /*
    Use the intensity weighted sums to compute the means and the
    second moments of the marginals to compute the error.
  */
	
  
  for (i = 0, xweight = yweight = xsqweight = ysqweight = 0.0; i < centsize; i++)
    {
      xweight += ysum[i] * (double) i;
      yweight += xsum[i] * (double) i;
      xsqweight += ysum[i] * ((double) i) * ((double) i);
      ysqweight += xsum[i] * ((double) i) * ((double) i);
    }


  /*
    The centroid.
  */

  x = xweight/framesum + xorigin;
  y = yweight/framesum + yorigin;

  /*
    The intensity weighted variance of the marginals.
    Also used in IRAF.
  */

  xvariance = fabs((xsqweight / framesum) - ((x - xorigin)
					     * (x - xorigin)));
  yvariance = fabs((ysqweight / framesum) - ((y - yorigin)
					     * (y - yorigin)));

  xerr = sqrt(xvariance / framesum);
  yerr = sqrt(yvariance / framesum);

  /*
    The normalized centered second moments used for calculating
    the ellipticity and position angle.
  */

  mxxnorm = ((double) mxx / framesum) - ((x - xorigin) * (x - xorigin));
  myynorm = ((double) myy / framesum) - ((y - yorigin) * (y - yorigin));
  mxynorm = ((double) mxy / framesum) - ((x - xorigin) * (y - yorigin));
  
  /*
    The square of the major axis and minor axis are asq and bsq
    respectively.  See for instance SExtractor documentation.
  */

  asq = (2.0 * (mxxnorm + myynorm)) + 
    (2.0 * sqrt(((mxxnorm - myynorm) * (mxxnorm - myynorm)) + 
		(4.0 * mxynorm * mxynorm)));
  bsq = (2.0 * (mxxnorm + myynorm)) - 
    (2.0 * sqrt(((mxxnorm - myynorm) * (mxxnorm - myynorm)) + 
		(4.0 * mxynorm * mxynorm)));

  /*
    The ellipticity (not eccentricity) and position angle.
  */ 

  ellipt = 1.0 - (sqrt(bsq)) / (sqrt(asq));
  posang = 0.5 * (180.0 / PI) * atan((2. * mxynorm)
				     / (mxxnorm - myynorm));

  /*
    IRAF uses an approximation for ellipticity which is given here
    for comparison.
  */ 

  /*
    ellipt = sqrt(((mxxnorm - myynorm) * (mxxnorm - myynorm))
    + ((2.0 * mxynorm) * (2.0 * mxynorm))) / (mxxnorm + myynorm);
  */

  /*
    Calculate the FWHM.  This should not be truncated
    but rather rounded up thus the + 0.5. The rint function
    rounds to the nearest integer but I don't know if rint
    is standard.
  */

  /*
    ix = (int) rint((double) x);
    iy = (int) rint((double) y);
  */

  ix = (int) (x + 0.5);
  iy = (int) (y + 0.5);

  hm = (*(frame + ix + ((iy * fsize))) - sky)/2.;

  /*
    Find the fwhm along the centroid row and columns using linear interpolation
    on both sides of the maximum
  */

  for (i = ix; i < fsize; i++)
    if ((*(frame + i + (iy * fsize)) - sky) < hm )
      {
	fwhmx = fabs(interp(*(frame+i-1+(iy*fsize)), *(frame+i+(iy*fsize)),
		       i-1-ix, i-ix, hm));
	break;
      }

  for (i = ix; i > 0; i--)
    if ((*(frame + i + (iy * fsize)) - sky) < hm )
      {
	fwhmx += fabs(interp(*(frame+i-1+(iy*fsize)), *(frame+i+(iy*fsize)),
			i-1-ix, i-ix, hm));
	break;
      }

  for (i = iy; i < fsize; i++)
    if ((*(frame + ix + (i * fsize)) - sky) < hm )
      {
	fwhmy = fabs(interp(*(frame+ix+((i-1)*fsize)), *(frame+ix+(i*fsize)),
		       i-1-iy, i-iy, hm));
	break;
      }

  for (i = iy; i > 0; i--)
    if ((*(frame + ix + (i * fsize)) - sky) < hm )
      {
	fwhmy += fabs(interp(*(frame+ix+((i-1)*fsize)), *(frame+ix+(i*fsize)),
			i-1-iy, i-iy, hm));
	break;
      }
  
  /*
    Store all the stellar parameters in the proper location.
  */

  *vals = x;
  *(vals + 1) = y;
  *(vals + 2) = xerr;
  *(vals + 3) = yerr;
  *(vals + 4) = fwhmx;
  *(vals + 5) = fwhmy;
  *(vals + 6) = sky;
  *(vals + 7) = framesum;
  *(vals + 8) = ellipt;
  *(vals + 9) = posang; 
  
  return 0;
}

