/**********************************************************************

	mode.c - this function determines the mode of an array
		of pixels given the number of pixels per axis
		of the square array and the binsize.

	The mode function produces a histogram of a data array 
	with a specified constant binsize and returns the most
	common data value which is the center of the bin.

	modified 27-Apr-2001, G. Grant Williams

**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cent.h"

unsigned short mode(int arrsize, unsigned short *arrvals, unsigned short bsize, int *hist)
{
	unsigned short	k, modebin, modeval;
	long	nbins;
	int 	i, j;

	/*
	Calculate the number of bins in a histogram given the maximum
        value and the binsize.  Adding one accounts for the zeroth bin.
	MAXVAL is defined in the cent.h.
	*/
 
        nbins = (((long) MAXVAL) / bsize) + 1;

	/*
	Fill the bins.
	*/

	for (i = 0, k = 0; i < (arrsize * arrsize); i++)
   	{
		/*
		Truncates to the integer result.
		*/

		k = *(arrvals + i) / bsize; 
		(*(hist + k))++;
   	}

	for (j = 0, modebin = 0; j < nbins; j++)
   		modebin = (*(hist + j) > *(hist + modebin)) ? j : modebin;

	/* Determine the most frequent value which is the center of
	the largest bin. For even numbered bins use the lower of the
	middle two to reduce negative numbers after subtraction. */

	modeval = modebin * bsize + ((bsize - 1) / 2);

	return modeval;
}
