/**********************************************************************

	imstat.c - compute statistics on binary image disk files.
		Currently (10/01/01) only the centroid is calculated.

	This program is compiled into a shared object (.so) library
	for use in Tcl/Tk applications.  It provides at least one
	package called Centroid which in turn produces at least one
	Tcl command called centroid.  The shared object could, in
	the future, provide additional packages such as Background
	or Maxpix which produce additional Tcl commands.  A given
	package could provide many more than one Tcl command; for
	expample Centroid could provide a Tcl command called
	fwhm which returns the seeing full width at half max.

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

	modified 02-Oct-2001, G. Grant Williams

**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <tcl.h>
#include <tk.h>
#include "cent.h"

int centroid(int boxsize, unsigned short *frame, double *vals);

int CentroidObjCmd(ClientData clientData, Tcl_Interp *interp,
                int objc, Tcl_Obj *CONST objv[]);

/*
All the variables which will be passed back to Tcl must be declared
outside the routines, i.e. global.
The elements of centvals are xcenter, ycenter, xerror, yerror
xwidth, ywidth, sky, and framesum.
*/

double	centvals[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

/*
The name of the initialization procedure must begin with an
upper case character and have only lower case characters thereafter.
*/
 
int Centroid_Init(Tcl_Interp *interp)
{

        /*
        Initialize the stub table interface which deals with
        compatibility upon Tcl/Tk upgrade.  The stub
        initializations must come before any other Tcl
        command.  Stubs were implemented in Tcl 8.1 and
        therefore we use 8.1 in the Init command below to
        indicate that any versions later than 8.1 can be used.

        Compilation Notes:
                One must define the USE_TCL_STUBS and USE_TK_STUBS
                symbols. Typically, you would include the
                -DUSE_TCL_STUBS and -DUSE_TK_STUBS flags when
                compiling the extension.

                Link the extension with the Tcl and Tk stubs
                libraries instead of the standard Tcl and Tk
                libraries for example:
                -ltclstub8.3 instead of -ltcl8.3.
                -ltkstub8.3 instead of -ltk8.3.
                On Unix platforms, the library names are
                libtclstub8.1.a and libtkstub8.1.a; on Windows
                platforms, the library names are tclstub81.lib
                and tkstub81.lib.

        Of course replace version numbers with your current version.
        To find your current version use: % info tclversion
        */

        if (Tcl_InitStubs(interp, "8.1", 0) == NULL) {
                return TCL_ERROR;
        }

        if (Tk_InitStubs(interp, "8.1", 0) == NULL) {
                return TCL_ERROR;
        }

	/*
	Note that the command name, the ObjCmd, and the package don't
	need to have the same name ("centroid",CentroidObjCmd,
	and Centroid_Init) it just works well here. 
	*/

        Tcl_CreateObjCommand(interp, "centroid", CentroidObjCmd,
                (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

        /*
        The following command is not strictly necessary.  It
        helps the 'pkg_mkIndex' procedure learn what libraries
        provide what packages.
        */

        Tcl_PkgProvide(interp, "Centroid", "1.0");
        return TCL_OK;
}

int CentroidObjCmd(ClientData clientData, Tcl_Interp *interp,
                int objc, Tcl_Obj *CONST objv[])
{

	Tcl_Obj	*resultPtr;

	FILE	*fp;

        int	i, j;
        int	nrows, ncols;
        int	firstrow, lastrow, firstcol, lastcol;
	int	framesize;
	int	errout;
	long	bsize, flsize, filesize;
	unsigned short	*pix_ptr;
	unsigned short	*boxpix_ptr;
	/*
	unsigned short	pix[31][31];
	unsigned short	boxpix[31][31];
	*/

	/*
        Initialize the pointers to point to the beginning of the
        pixel arrays.
        */

	/*
	pix_ptr = &pix[0][0];
	boxpix_ptr = &boxpix[0][0];
	*/

        if (objc != 8) {
                Tcl_WrongNumArgs(interp, 1, objv, "binfile nrows ncols firstrow lastrow firstcol lastcol");
                return TCL_ERROR;
        }

	/*
	Note that smallest firstrow/firstcol is 0 not 1.
	i.e zero based.
	*/

        if (objc == 8) {
                if (Tcl_GetIntFromObj(interp, objv[2], &nrows) != TCL_OK) {
                        return TCL_ERROR;
                }
                if (Tcl_GetIntFromObj(interp, objv[3], &ncols) != TCL_OK) {
                        return TCL_ERROR;
                }
                if (Tcl_GetIntFromObj(interp, objv[4], &firstrow) != TCL_OK) {
                        return TCL_ERROR;
                }
                if (Tcl_GetIntFromObj(interp, objv[5], &lastrow) != TCL_OK) {
                        return TCL_ERROR;
                }
                if (Tcl_GetIntFromObj(interp, objv[6], &firstcol) != TCL_OK) {
                        return TCL_ERROR;
                }
                if (Tcl_GetIntFromObj(interp, objv[7], &lastcol) != TCL_OK) {
                        return TCL_ERROR;
                }
        }

	/*
	The author needs to add some checks here to see if the nrows
	and ncols is less than 512 (max image size and max allocated).
	Also need to check that lastrow - firstrow = lastcol - firstcol.
	*/

	/*
	Determine the size of the square box for calculating
	the centroid, use only one of the following.
	Don't let that size exceed some max because large boxes
	may result in inaccurate centroids.  Also we want to keep 
	the square of that size an integer rather than a long.
	*/

        framesize = lastrow - firstrow + 1;
	/*
	framesize = lastcol - firstcol + 1;
	*/
 
        if (framesize > MAXFSIZE || framesize < MINFSIZE){
		printf("<framesize> must be between %d and %d\n",
		MINFSIZE, MAXFSIZE);
		return TCL_ERROR;
        }

        /*
        The number of rows and number of columns are command line inputs.
        They are used to calculate the expected binary image size assuming
        that the pixels are 16-bit (2 byte) digitized (0 - 65536).
        */

        flsize = nrows * ncols * sizeof (unsigned short);
        bsize = framesize * framesize * sizeof (unsigned short);

	/*
	Since the size of the arrays is determined only after the 
	user calls the function/routine:
	Dynamically allocate the multidimensional arrays.
	*/

	pix_ptr = (unsigned short *)malloc(flsize);
	if (pix_ptr == NULL)
	{
		fprintf(stderr,"Memory allocation error.\n");
		return TCL_ERROR;
	}

	boxpix_ptr = (unsigned short *)malloc(bsize);
	if (boxpix_ptr == NULL)
	{
		fprintf(stderr,"Memory allocation error.\n");
		return TCL_ERROR;
	}

        /*
        Open the binary "b" for read "r", thus the "rb" in the
        fopen statement below.
        */

        if ((fp = fopen(Tcl_GetStringFromObj(objv[1], NULL),"rb")) == NULL)
        {
                fprintf(stderr, "Cannot open %s\n", Tcl_GetStringFromObj(objv[1], NULL));
                return TCL_ERROR;
        }

        /*
        Obtain file size. fseek with the SEEK_END parameter will
        set the file's position indicator to the end.  ftell returns
        the value of the file's position indicator, i.e. filesize.
        */

        fseek (fp, 0, SEEK_END);
        filesize = ftell(fp);

        if (filesize != flsize)
        {
                fprintf(stderr, "%s: has a format other than %d x %d unsigned short pixels.\n", Tcl_GetStringFromObj(objv[1], NULL), nrows, ncols);
                fprintf(stderr, "The file size is %ld bytes rather than %ld bytes.\n", filesize, flsize);
                return TCL_ERROR;
        }

        rewind (fp);

        /*
        Read the binary file into the buffer pix_ptr.  The second
        argument is the size of each datum and the third argument
        is the number of data to read.
        */

        if (fread(pix_ptr, sizeof (unsigned short), (filesize / sizeof (unsigned short)), fp) != (filesize / sizeof (unsigned short)))
        {
                fprintf(stderr, "Error reading file %s.\n",Tcl_GetStringFromObj(objv[1], NULL));
                return TCL_ERROR;
        }

        fclose(fp);

        /*
        Uncomment the following to print the pixel array here.
        */

        /*
        for (i = 0; i < nrows; i++)
        {
                for (j = 0; j < ncols; j++)
                {
                  printf("%d\n",pix[i][j]);
                }
        }
        */

        /*
        Or test by printing a few pixels.
        */

        /*
        printf("%d\n",pix[0][0]);
        printf("%d\n",pix[0][1]);
        printf("%d\n",pix[1][0]);
        printf("%d\n",pix[0][2]);
        printf("%d\n",pix[2][0]);
        printf("%d\n",pix[0][511]);
	printf("%d\n",pix[511][0]);
        printf("%d\n",pix[511][511]);
        */

	/*
	Find the new pixel array which is inside the centroid box
	*/

        for (i = 0; i < framesize; i++)
        {
                for (j = 0; j < framesize; j++)
                {
			/*
			boxpix[i][j] = *(pix_ptr + ((firstrow + i) * ncols) + (firstcol) + j);
			*/
			*(boxpix_ptr + (i * framesize) + j)= *(pix_ptr + ((firstrow + i) * ncols) + (firstcol) + j);
                }
        }

	/*
	errout = centroid(framesize, &boxpix[0][0], centvals);
	*/
	errout = centroid(framesize, boxpix_ptr, centvals);

	/*
	Now we need to set up global Tcl variables.  The FWHM is
	calculated for both axes but only one is used, we may want
	to average them.

        Two methods of using the global variables.  The second
        method is more favorable as it is similar to returning a
        pointer.

        Tcl_SetVar and Tcl_LinkVar are methods of sharing variables
        between Tcl and C.  The quoted argument is the Tcl variable
        and the next argument is the corresponding C variable or
        variable address.  The Tcl_LinkVar procedure needs to know
        the "sizeof" the variable to convert it to a string in Tcl.
        Thus the argument TCL_LINK_DOUBLE.
        */

        /*
        Tcl_SetVar(interp, "xbar", buffer, 0);
        */

	Tcl_LinkVar(interp, "xbar", (char *) &centvals[0], TCL_LINK_DOUBLE);
	Tcl_LinkVar(interp, "ybar", (char *) &centvals[1], TCL_LINK_DOUBLE);
	Tcl_LinkVar(interp, "fwhm", (char *) &centvals[4], TCL_LINK_DOUBLE);
	Tcl_LinkVar(interp, "backflux", (char *) &centvals[6], TCL_LINK_DOUBLE);
	Tcl_LinkVar(interp, "starflux", (char *) &centvals[7], TCL_LINK_DOUBLE);

        printf("xcenter = %f, ycenter = %f\n", centvals[0], centvals[1]);
        printf("xerror = %f, yerror = %f\n", centvals[2], centvals[3]);
        printf("xwidth = %f, ywidth = %f\n", centvals[4], centvals[5]);
        printf("sky = %f, framesum = %f\n", centvals[6], centvals[7]);
        printf("ellipt = %f, posang = %f\n", centvals[8], centvals[9]);

	free(pix_ptr);
	free(boxpix_ptr);

	/*
        Send the result back to Tcl.  First get a handle on the
        result Obj and then set its value.  The result is set
        to 1 in the case below but it may be changed to return
        something like the filename if it is not specified
        by the user in later versions.
        */

        resultPtr = Tcl_GetObjResult(interp);
        Tcl_SetDoubleObj(resultPtr, 1);

	return TCL_OK;
}
