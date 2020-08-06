/**********************************************************************
	addnums.c - a routine built as an example to demonstrate the
		use of C routines in Tcl/Tk.

	This function takes in two arguments and adds them together.
	As a demonstration both the Tcl string commands and object
	cammands are used.  The object commands are prefered as the
	string commands are depricated.  The variables which are to
	be passed back to the Tcl interpreter must be declared as
	global, i.e. outside of any routince. 

	See:

	http://dev.scriptics.com/

	for more Tcl/Tk information.

	Documentation and man pages are located at:

	http://dev.scriptics.com/doc/

**********************************************************************/

/* Both the Tcl and Tk libraries should be included. */

#include <tcl.h>
#include <tk.h>

/*
Declarations for application-specific command procedures.
All Tcl command procedure must adhere to this format where
clientData, of type ClientData (Tcl), is a pointer to some
arbitrary application specific data specified when the
command is registered, interp is the Tcl interpreter, and 
argc and argv (and also objc and objv) have there normal
c definitions of number of arguments and array of inputs.  

Both string and object commands are demonstrated but only 
one method should be used.  The object commands are favored
as the string method is depricated.
*/

int SumCmd(ClientData clientData,
		Tcl_Interp *interp,
		int argc, char *argv[]);

int SumObjCmd(ClientData clientData,
		Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);

/* The global variables which are passed back to Tcl. */

double summed = 0.;
double osummed = 0.;

/*
Addition_Init is an initialization procedure which is
called when the package is loaded.
*/

int Addition_Init(Tcl_Interp *interp) {

	/*
	Initialize the stub table interface which assists with
	compatibility upon upgrade.  This was implemented in 
	Tcl version 8.1.  I use version 8.0 so it is commented
	out.  Note: One must compile with the TCL_USE_STUBS flag. 
	*/

	/*
	if (Tcl_InitStubs(interp, "8.0", 0) == NULL) {
		return TCL_ERROR;
	}
	*/

	/*
	Register two variations of sums.  The osum command
	uses the object interface.  The quoted string is 
	the command to be used in Tcl and the xxxCmd arguments
	are the Tcl command procedures written as a C routine.
	Client data or the command upon deletion procedure are
	set to null because I don't care to use them here.
	*/

	Tcl_CreateCommand(interp, "sums", SumCmd,
		(ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

	Tcl_CreateObjCommand(interp, "osums", SumObjCmd,
		(ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

	/*
	Declare that the package Addition is provided by the C code,
	so scripts that do "package require addition" can load the
	library automatically.  The 1.1 is a version number.
	Not really used here but demonstrated.
	*/

	Tcl_PkgProvide(interp, "Addition", "1.1");
	return TCL_OK;

}


/*
SumCmd --
This implements the sums Tcl command. It requires two
arguments and returns the sum of the two.  It also sets
the global variable xbar to the sum of the two.
*/

int
SumCmd(ClientData clientData, Tcl_Interp *interp,
		int argc, char *argv[])

{
	int error;
	char buffer[100];
	/*
	double summed = 0.;
	*/
	double numone = 0.;
	double numtwo = 0.;

	if (argc != 3) {
		interp->result = "Usage: sums ?number? ?number?";
		return TCL_ERROR;
	}

	/*
	The Tcl_GetDouble routine tells Tcl to put the second (1) and
	third (2) arguments into the addresses of numone and numtwo. 
	*/

	if (argc == 3) {
		if (Tcl_GetDouble(interp, argv[1], &numone) != TCL_OK) {
			return TCL_ERROR;
		}
		if (Tcl_GetDouble(interp, argv[2], &numtwo) != TCL_OK) {
			return TCL_ERROR;
		}
	}

	summed = numone + numtwo;
        sprintf(buffer,"%f",summed);

	/*
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

	Tcl_LinkVar(interp, "xbar", (char *) &summed, TCL_LINK_DOUBLE);

	/*
	The result of the procedure is returned to Tcl through the
	result member buffer using sprintf.
	*/

	sprintf(interp->result, "%f", summed);
	return TCL_OK;
}


/*
SumObjCmd --
This implements the osums Tcl command. It requires two
arguments and returns the sum of the two.  It also sets
the global variable ybar to the sum of the two.
*/

int
SumObjCmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])

{
	Tcl_Obj *resultPtr;
	int error;
	/*
	double osummed = 0.;
	 */
	double onumone = 0.;
	double onumtwo = 0.;

	/* Tcl_WrongNumArgs is a Tcl library routine which generates
	a standard error message and stores it in the result object
	of interp.  It requires the use of objc and objv.
	*/  

	if (objc != 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "?number? ?number?");
		return TCL_ERROR;
	}

	if (objc == 3) {
		if (Tcl_GetDoubleFromObj(interp, objv[1], &onumone) != TCL_OK) {
			return TCL_ERROR;
		}
		if (Tcl_GetDoubleFromObj(interp, objv[2], &onumtwo) != TCL_OK) {
			return TCL_ERROR;
		}
	}

	osummed = onumone + onumtwo;

	Tcl_LinkVar(interp, "ybar", (char *) &summed, TCL_LINK_DOUBLE);

	/*
	The Tcl_GetObjResult routine is used to set the result
	of the call to an object command procedure.
	*/

	resultPtr = Tcl_GetObjResult(interp);
	Tcl_SetDoubleObj(resultPtr, osummed);

	return TCL_OK;
}
