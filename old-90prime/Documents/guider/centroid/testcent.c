/**********************************************************************

	testcent.c - test a centroiding function on CCD data.

	This file is used to test the accompanying centroiding
	function.  In its final version the centroiding function
	will be called by the guide camera software.  For test
	purposes this program will read the contents of a file
	assumed to be n^2 pixel values.  The memory location of
	the data and the size of the array are sent to centroid.
	Centroid returns the x and y center positions, the error
	in those positions, the x and y full width half max values,
	the sky value, the sum of the pixels in the subframe, the
	ellipticity of the star, and the position angle of the star.

	modified 11-Apr-2001, G. Grant Williams

**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cent.h"
 
int centroid(int fsize, unsigned short *frame, double *vals);
 
int main(int argc, char *argv[])
{

	FILE	*fp;

        int	framesize;
	int	errout;
	unsigned short	pixels[atoi(argv[2])][atoi(argv[2])], *pixels_ptr;

	/* The elements of centvals are xcenter, ycenter, xerror, yerror
	xwidth, ywidth, sky, and framesum. */

	double	centvals[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

	pixels_ptr = &pixels[0][0];
 
        if (argc != 3){
       	  printf("Usage: testcent <data filename> <framsize>\n");
          printf("<framsize> is the number pixels per axis for
			the square array.\n");
          exit(-1);
        }
 
        framesize = atoi(argv[2]);
 
        if (framesize > MAXFSIZE || framesize < MINFSIZE){
          printf("<framesize> must be between %d and %d\n",
		MINFSIZE, MAXFSIZE);
          exit(-1);
        }

	if ((fp = fopen(argv[1],"r")) == NULL)
        {
	  fprintf(stderr, "Cannot open %s\n", argv[1]);
          exit(-1);
        }
 
        printf("Reading %d x %d pixels from file: %s\n",
		framesize, framesize, argv[1]);

	while(fscanf(fp,"%hu",pixels_ptr++) != EOF);

	fclose(fp);

	/* Uncomment the following to print the pixel array here. */

	/*
	for (i = 0; i < framesize; i++)
	{
		for (j = 0; j < framesize; j++)
		{
		  printf("%d\n",pixels[i][j]);
		}
	}
	*/

        printf("Begining centroid of file: %s\n", argv[1]);

	errout = centroid(framesize, &pixels[0][0], centvals);

        printf("xcenter = %f, ycenter = %f\n", centvals[0], centvals[1]);
        printf("xerror = %f, yerror = %f\n", centvals[2], centvals[3]);
        printf("xwidth = %f, ywidth = %f\n", centvals[4], centvals[5]);
        printf("sky = %f, framesum = %f\n", centvals[6], centvals[7]);
        printf("ellipt = %f, posang = %f\n", centvals[8], centvals[9]);

	return 0;
}
