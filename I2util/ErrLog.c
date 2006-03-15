/*
 *      $Id$
 */
/************************************************************************
*									*
*			     Copyright (C)  2002			*
*				Internet2				*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		ErrLog.c
 *
 *	Author:		Jeff Boote
 *			Internet2
 *
 *	Date:		Tue Apr 23 09:11:05  2002
 *
 *	Description:	
 *		Generic error logging routines.
 *
 *
 *		Based on code from UCAR DCS tools. Copyright information
 *		from UCAR follows:
 *
 *		Copyright 1997 University Corporation for Atmospheric Research,
 *		Scientific Computing Division.  All rights reserved.
 *
 *			
 *		Permission to use, copy, modify and distribute this software
 *		and its	documentation for any academic, educational and
 *		scientific research purpose is hereby granted without fee,
 *		provided that the above copyright notice and this permission
 *		notice appear in all copies of this software and its
 *		documentation, and that the software is not sold and/or made
 *		the subject of any commercial activity.  Parties interested
 *		in commercial licensing should contact the copyright holder.
 *
 */
#include <I2util/util.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include "mach_dep.h"


/*
 * May support threading in future.
 */
static I2ThreadMutex_T	MyMutex = I2PTHREAD_MUTEX_INITIALIZER;

#define	TABLE_SIZE	10
#define	MSG_BUF_SIZE	10240

/*
 *	 an error table. Error table[0] contains sys_errlist.
 */
typedef	struct	ErrTable_ {
	unsigned	start,		/* starting index for err_list	*/ 
			num;		/* num elements in err_list	*/
	const char	**err_list;	/* error messags		*/
	} ErrTable;


/*
 * static variables that contain the file name and line number of the most
 * recently logged error via a call to I2ErrLocation_().
 *
 * N.B. I2ErrLocation_() should never be called directly by a client
 * of the error module. I2ErrLocation_() is invoked via the macro call
 * I2ErrLog() which calls I2ErrLocation_() and then immediately calls
 * I2ErrLogFunction_(). This enables multiple instances of an error 
 * logging session w/out having to maintain the current file and line
 * number as a separate state variable for each session with some degree
 * of security. In a threaded environment access to these variables is
 * locked by I2ErrLocation_() and unlocked by I2ErrLogFunction_(). All of
 * this is necessary because the only way to automatically generate the
 * line number and file name information automatically is via and macro
 * call (as opposed to a function call) and macros don't accept variable
 * number of arguments
 */
static const char	*errorFile = "";
static const char	*errorDate = "";
static int		errorLine = 0;


/*
 * An error handle struct. This struct maintains the state of an
 * error reporting session created via I2ErrOpen()
 */
typedef	struct	ErrHandle_ {
	ErrTable	err_tab[TABLE_SIZE];	/* all the error tables	*/
	int		err_tab_num;		/* num error tables	*/
	int		code;		/* error code of last error */
	const char	*program_name;		/* name of calling prog	*/
	void		*data;			/* client data		*/
	I2ErrLogFuncPtr	log_func;		/* client loggin func	*/
	void		*log_func_arg;		/* client arg to log func*/
	I2ErrRetrieveFuncPtr	retrieve_func;	/* client fetch func	*/
	void		*retrieve_func_arg;	/* client arg to retrieve func*/
	I2ErrLogResetFuncPtr	reset_func;	/* reset log_func	*/
	} ErrHandle;

/*
 * fetch an error message associated with a given error code. Return
 * an empty string if no matching error message is found.
 */
static	const char	*get_error(ErrHandle *eh, int error)
{
	int		i;
	unsigned int	index;
	unsigned int	errnum;

	if(error < 0)
		return("");
	errnum = (unsigned int)error;

	if(!eh){
		return strerror(errnum);
	}

	for (i=0; i<eh->err_tab_num; i++) {
		if(eh->err_tab[i].start > errnum)
			continue;
		index = errnum - eh->err_tab[i].start;
		if ((unsigned)errnum >= eh->err_tab[i].start && 
					index < eh->err_tab[i].num) {

			return(eh->err_tab[i].err_list[index]);
		}
	}

	return("");
}

/*
 * format an error message converting any `%M' tokens to the error 
 * string associated with error message number `code, and any 
 * `%N' tokens to the ascii representation of the value of `code
 *
 * `buf' must have enough space to hold the new string
 */
static int	esnprintf(
	ErrHandle	*eh,
	char		*buf, 
	size_t		size_buf,
	const char	*format,
	int		code
) {
	const char	*p1;
	char		*p2;
	size_t		size_p2 = size_buf;
	int		len;


	for(p1=format, p2=buf; *p1 && (size_p2 > 1); p1++, p2++, size_p2--) {
		if (*p1 != '%') {
			*p2 = *p1;
			continue;
		}
		p1++;
		if (*p1 == 'M') {
			len =snprintf(p2,size_p2,"%s",get_error(eh,code))-1;
			p2 += len;
			size_p2 -= len;
		}
		else if (*p1 == 'N') {
			len = snprintf(p2,size_p2,"%d", code) - 1;
			p2 += len;
			size_p2 -= len;
		}
		else {
			*p2++ = '%';
			size_p2--;
			*p2 = *p1;
		}
	}
	*p2 = '\0';

	return(strlen(buf));
}

void	add_ansiC_errors(ErrHandle *eh)
{
	char	**sys_errlist;		/* list of errors	*/
	int	errlist_size;		/* size of $sys_errlist	*/

	/*
	**	Get the list of system errors supported on this platform
	*/
	sys_errlist = I2GetSysErrList(&errlist_size);

	eh->err_tab_num = 0;
	(void) I2ErrList(
		(I2ErrHandle) eh, 0, errlist_size, (const char **) sys_errlist);
}

/*
**
**	A P I
**
*/

/*
 * Function:	I2ErrOpen()
 *
 * Description:	The I2ErrOpen() function starts an error reporting session
 *		and returns an error handle associated with that session.
 *
 *		The `log_func' argument is a pointer to a logging function
 *		that is to be called whenever an error is logged with the
 *		I2ErrLog() macro. This function may be user-defined or may
 *		be one of the pre-defined functions (I2LogFuncStack_()
 *		or I2LogFuncImmediate_()).
 *
 *		The `retrieve_func' argument is a pointer to a error
 *		log retrieval function that is invoked to fetch error 
 *		messages logged with the function pointed to by
 *		`log_func'. `retrive_func' may be NULL if no such
 *		function exists.
 *
 * In Args:
 *
 *	*program_name	A pointer to a character string specifying the
 *			name of the calling program (or any other piece
 *			of information you might wish).
 *
 *	*log_func	A pointer to a user-defined error logging function
 *			as described below.
 *
 *	*log_func_arg	A pointer to user-defined data that is to be passed
 *			to the logging function whenever the logging
 *			function is invoked.
 *
 *	*retrieve_func	A pointer to a user-defined error retrieval function
 *			as described below.
 *
 *	*ret_func_arg	A pointer to user-defined data that is to be passed
 *			to the retrieval function whenever the retrieval
 *			function is invoked.
 *
 *
 * Out Args:
 *
 * Return Values:	An error handle is returned upon success. Otherwise
 *			NULL is returned and the global variable `errno'
 *			is set accordingly
 *
 * Side Effects:
 *
 *	
 * The Logging Function
 *
 *	The logging function pointed to by `log_func' is called indirectly
 *	by the I2ErrLog() macro to log error messages. The `program_name'
 *	parameter points to the array pointed to by the parameter of the
 *	same name passed to I2ErrOpen(). The logging function is also provided
 *	provided with the name of the file from whence I2ErrOpen() was 
 *	called, `file', as well as the line number, `line', the date the
 *	file was compiled, `date', the error message to be logged, `msg', 
 *	the user-defined logging function argument passed to I2ErrOpen(),
 *	`arg', and a hook for any state context the logging function
 *	needs to maintain, `data'. 
 *
 * In Args:
 *
 *	*program_name	A pointer to the character string passed to 
 *			I2ErrOpen() as its first argument.
 *
 *	*file		A pointer to a string containing the name of the
 *			file from whence I2ErrLog() was called.
 *
 *	line		An integer indicating the line number within
 *			`file' from whence I2ErrLog() was called.
 *
 *	*date		A pointer to a string containing the date (of the
 *			file from which I2ErrLog() was called) was compiled.
 *
 *	*msg		A pointer to string containing the error message
 *			to be logged.
 *
 *	*arg		A pointer to the `log_func_arg' argument passed to
 *			I2ErrOpen()
 *
 *	**data		A pointer to a pointer to be used by the logging
 *			function in any desired manner. `*data' is NULL
 *			unless changed by the logging function
 *
 * The Retrieval Function
 *
 *	The retrieval function pointed to by `retrieve_func' is invoked
 *	whenever the error messages  logged by the logging function are
 *	to be retrieved. The retrieval function is passed the same hook,
 *	`data', for maintaining state information as the logging function,
 *	and the user-defined argument, `arg', passed to the I2ErrOpen()
 *	function as `retreive_func_arg'.
 *
 *	The retrieval function should return a pointer to an area of
 *	dynamically allocated memory containing a character string 
 *	representing the error messages logged with previous calls
 *	to the logging function. The caller will free the memory
 *	returned by the retrieval function.
 *
 * In Args
 *
 *	*arg		A pointer to the `retrieve_func_arg' argument passed to
 *			I2ErrOpen()
 *
 *	**data		A pointer to a pointer to be used by the retrieval
 *			function in any desired manner. `*data' is NULL
 *			unless changed by the logging function
 *	
 */
I2ErrHandle	I2ErrOpen(
		const char		*program_name,
		I2ErrLogFuncPtr		log_func,
		void			*log_func_arg,
		I2ErrRetrieveFuncPtr	retrieve_func,
		void			*retrieve_func_arg
) {

	ErrHandle	*eh;

	if (! log_func) {
		errno = EINVAL;
		return(NULL);
	}

	if (! (eh = malloc(sizeof(ErrHandle)))) {
		return(NULL);
	}

	eh->code = 0;
	eh->program_name = program_name;
	eh->log_func = log_func;
	eh->log_func_arg = log_func_arg;
	eh->retrieve_func = retrieve_func;
	eh->retrieve_func_arg = retrieve_func_arg;
	eh->data = NULL;


	/*
	 * add the Standard C Library error messages
	 */
	add_ansiC_errors(eh);

	return((I2ErrHandle) eh);
}

/*
 * Function:	I2ErrSetResetFunc
 *
 * Description:	
 * 		Used to assign the "reset" function. Not used, so it
 * 		was not useful to add to the "open". (Plus, it would
 * 		be hard to make "open" backward compat.)
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
void	I2ErrSetResetFunc(
		I2ErrHandle		dpeh,
		I2ErrLogResetFuncPtr	reset_func
	)
{
	ErrHandle	*eh = (ErrHandle *) dpeh;

	eh->reset_func = reset_func;

	return;
}

/*
 * Function:	I2ErrReset
 *
 * Description:	
 * 		If there is a "reset_func" assigned to the ErrHandle,
 * 		call it.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
I2ErrHandle
I2ErrReset(
		I2ErrHandle	dpeh
		)
{
	ErrHandle	*eh = (ErrHandle *) dpeh;

	if(eh->reset_func){
		if(!(eh->reset_func(eh->log_func_arg, &(eh->data)))){
			I2ErrClose(dpeh);
			return NULL;
		}
	}

	return dpeh;
}

/*
 * Function:	I2ErrClose()
 *
 * Description:	This function closes the error handling session associated
 *		with `dpeh'.
 *
 * In Args:
 *
 *	dpeh		An error handle returned via a call to I2ErrOpen().
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects:
 */
void	I2ErrClose(I2ErrHandle dpeh)
{
	ErrHandle *eh = (ErrHandle *) dpeh;

	if (eh) free(eh);
}

/*
 * Function:	I2ErrRep()
 *
 * Description: The I2ErrRep() function calls the client retrieval function
 *		(if one exists) passed to I2ErrOpen() for the error 
 *		handling session associated with `dpeh'. I2ErrRep() copies
 *		the program name, followed by a ":", followed by a space,
 *		followed by the character string returned by the client
 *		retrieval function, followed by a newline, to the file  
 *		pointer, `fp'. The character string returned by the 
 *		retrieval function is then freed with a call to free().
 *
 * In Args:
 *
 *	dpeh		An error handle returned via a call to I2ErrOpen().
 *
 *	fp		A pointer to a file stream.
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects:
 */
void	I2ErrRep(
	I2ErrHandle	dpeh,
	FILE		*fp
) {
	ErrHandle *eh = (ErrHandle *) dpeh;
	char		*msg;


	if (! eh) return;

	I2ThreadMutexLock(&MyMutex);

	if (eh->retrieve_func) {
		msg = eh->retrieve_func(eh->retrieve_func_arg, &(eh->data));
		(void) fprintf(fp, "%s:\n", eh->program_name);
		(void) fprintf(fp, "%s\n", msg);
		free(msg);
	}
	I2ThreadMutexUnlock(&MyMutex);

}

/*
 * Function:	I2ErrGetMsg()
 *
 * Description: The I2ErrGetMsg() function calls the client retrieval function
 *		(if one exists) passed to I2ErrOpen() for the error 
 *		handling session associated with `dpeh'. I2ErrGetMsg() returns
 *		to the caller the value returned to it by the client
 *		retrieval function (an address in dynamically allocated memory
 *		of a character string). The caller is responsible for freeing
 *		the memory returned by I2ErrGetMsg().
 *
 * In Args:
 *
 *	dpeh		An error handle returned via a call to I2ErrOpen().
 *
 * Out Args:
 *
 * Return Values:	I2ErrGetMsg() returns NULL if no retrieval function
 * 			exists.
 *
 * Side Effects:
 */
char	*I2ErrGetMsg(
	I2ErrHandle	dpeh
) {
	ErrHandle	*eh = (ErrHandle *) dpeh;
	char		*msg;

	I2ThreadMutexLock(&MyMutex);

	if (eh && eh->retrieve_func) {
		msg = eh->retrieve_func(eh->retrieve_func_arg, &(eh->data));
	}
	else {
		msg = NULL;
	}
	
	I2ThreadMutexUnlock(&MyMutex);

	return(msg);
}



/*
 * Function:	I2ErrGetCode()
 *
 * Description: The I2ErrGetCode() function returns the error number currently
 *		associated with the error handle, `dpeh'.
 *
 *		N.B. the error code returned is the error code stored from
 *		the most *recent* call to I2ErrLog() or I2ErrLogP(). If
 *		you are nesting calls to these functions only the error 
 *		code from the outer-most logging function is available - i.e
 *		inner calls get overwritten.
 *
 * In Args:
 *
 *	dpeh		An error handle returned via a call to I2ErrOpen().
 *
 * Out Args:
 *
 * Return Values:	I2ErrGetCode() returns an integer.
 *
 * Side Effects:
 */
int	I2ErrGetCode(
	I2ErrHandle	dpeh
) {
	ErrHandle	*eh = (ErrHandle *) dpeh;

	return(eh->code);
}




/*
 *	I2ErrList()
 *
 *	Adds an error list to the error table. 'start' should be the first
 *	valid error number for this table. The values 0 - 1000 are reserved.
 *	The index into 'err_list' is calculated by subtracting 'start'
 *	from the error number. Thus if you add a list with 'start' equal
 *	to 1001 and later invoke ESprintf with 'code' equal to 1001 the 
 *	error message referenced will be 'err_list[0]'
 *
 * on entry
 *	start		: first valid error number
 *	num		: number of elements in 'err_list'. 
 *	**err_list	: address of error list
 *
 * on exit
 *	return		: -1 => table full, else OK.
 */
int	I2ErrList(
	I2ErrHandle 	dpeh, 
	unsigned 	start, 
	unsigned 	num, 
	const char 	**err_list
) {

	ErrHandle *eh = (ErrHandle *) dpeh;

	if (! eh) return(0);

	if (eh->err_tab_num >= TABLE_SIZE -1) {
		return(-1);	/* table full	*/
	}

	eh->err_tab[eh->err_tab_num].start = start;
	eh->err_tab[eh->err_tab_num].num = num;
	eh->err_tab[eh->err_tab_num].err_list = err_list;
	eh->err_tab_num++;

	return(0);
}



/*
 * Function:	I2ErrLocation_
 *
 * Description: The I2ErrLocation_() records the file name, `file', line
 *		number, `line', and compilation date, `date' reported by
 *		the I2ErrLog() macro. This function should never be 
 *		called directly by the user himself.
 *
 * In Args:
 *
 *	*file		A pointer to a character string containing a file
 *			name.
 *
 *	*date		A pointer to a character string containing the
 *			compiliation date of `file'
 *
 *	line		An integer specifying a line number within the 
 *			file, `file'.	
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects:
 *
 *		I2ErrLocation_() writes the global, static variables, 
 *		`errorFile', `errorDate', and `errorLine'. In order to 
 *		be thread-save this function should lock these variables 
 *		before attemping to write them (it does if `THREADS_ENABLE'
 *		is defined). The variables should then
 *		remain locked until unlocked by I2ErrLogFunction() or
 *		I2ErrLogPFunction().
 */
void	I2ErrLocation_(
	const char	*file,
	const char	*date,
	int		line
) {

	I2ThreadMutexLock(&MyMutex);

	errorFile = file;
	errorDate = date;
	errorLine = line;
}

/*
 * Function:	I2ErrLogPFunction_()
 *
 * Description:	The I2ErrLogPFunction() is identical to the I2ErrLogFunction()
 *		except that it is invoked by the I2LogPErr() macro which
 *		takes an additional error code argument that is passed
 *		on to I2ErrLogPFunction_(), `code'
 *
 *		`code' may be any valid value of the system global
 *		variable, `errno', or it may be an error code made valid
 *		by a call to I2ErrList().
 *
 *		
 *
 * In Args:
 *
 *	dpeh		An error handle returned via a call to I2ErrOpen().
 *
 *	code	An integer indicating the error number.
 *
 *	*format		A pointer to a character string containing formating
 *			information.
 *
 *	...		variable arguments as indicated in `format'
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects:
 */
void
I2ErrLogVT(
	I2ErrHandle	dpeh,
	int		level,
	int		code, 
	const char	*format,
	va_list		ap
)
{
	ErrHandle		*eh = (ErrHandle *) dpeh;
	char			new_format[MSG_BUF_SIZE];
	char			buf[MSG_BUF_SIZE];
	struct I2ErrLogEvent	event;

        if(level == I2LOG_NONE)
            return;

	event.mask = 0;

	if(!code)
		code = errno;
	else{
		event.code = code;	event.mask |= I2CODE;
	}

	(void)esnprintf(eh,new_format,sizeof(new_format),format,code);

	/*
	 * deal with variable args
	 */
        (void) vsnprintf(buf,sizeof(buf),new_format,ap);

	if(!eh){
		fwrite(buf,sizeof(char),strlen(buf),stderr);
		fwrite("\n",sizeof(char),1,stderr);
		I2ThreadMutexUnlock(&MyMutex);
		return;
	}

	eh->code = code;
	event.name = eh->program_name;	event.mask |= I2NAME;
	event.file = errorFile;		event.mask |= I2FILE;
	event.line = errorLine;		event.mask |= I2LINE;
	event.date = errorDate;		event.mask |= I2DATE;
	event.msg = buf;		event.mask |= I2MSG;

	if(level > -1 && level < 8){
		event.level = level;	event.mask |= I2LEVEL;
	}

	eh->log_func(
		&event, eh->log_func_arg, &(eh->data)
	);
	I2ThreadMutexUnlock(&MyMutex);

	return;
}

void
I2ErrLogTFunction_(
	I2ErrHandle	dpeh,
	int		level,
	int		code, 
	const char	*format,
	...
)
{
	va_list		ap;

        va_start(ap, format);
	I2ErrLogVT(dpeh,level,code,format,ap);
        va_end(ap);

	return;
}

/*
 * Function:	I2ErrLogPFunction_()
 *
 * Description:	The I2ErrLogPFunction() is identical to the I2ErrLogFunction()
 *		except that it is invoked by the I2LogPErr() macro which
 *		takes an additional error code argument that is passed
 *		on to I2ErrLogPFunction_(), `code'
 *
 *		`code' may be any valid value of the system global
 *		variable, `errno', or it may be an error code made valid
 *		by a call to I2ErrList().
 *
 *		
 *
 * In Args:
 *
 *	dpeh		An error handle returned via a call to I2ErrOpen().
 *
 *	code	An integer indicating the error number.
 *
 *	*format		A pointer to a character string containing formating
 *			information.
 *
 *	...		variable arguments as indicated in `format'
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects:
 */
void	I2ErrLogPFunction_(
	I2ErrHandle	dpeh,
	int		code, 
	const char	*format, ...
)
{
	va_list		ap;

        va_start(ap, format);
	I2ErrLogVT(dpeh,-1,code,format,ap);
        va_end(ap);

	return;
}

/*
 * Function:	I2ErrLogFunction_()
 *
 * Description:	The I2ErrLogFunction() converts and formats its arguments 
 *		under the control of `format'. I2ErrLogFunction_()
 *		then invokes the client-supplied logging function associated
 *		with `dpeh' and passes it the formatted argument string.
 *
 *		`format' is a character string which accepts an identical
 *		syntax to that of Standard C's *printf family of functions.
 *		Additionaly, `format' may contain instances of the format
 *		specifier, "%M", which are replaced by the UNIX system
 *		error message associated with `errno', and instances of 
 *		the specifier "%N", which are replaced with the ascii value
 *		of `errno'.
 *
 *		I2ErrLogFunction() is never invoked directly by the user.
 *		Instead, it is invoked via the I2ErrLog() macro, which
 *		calls I2ErrLocation_() first, passing it the error location
 *		information.
 *
 *		
 *
 * In Args:
 *
 *	dpeh		An error handle returned via a call to I2ErrOpen().
 *
 *	*format		A pointer to a character string containing formating
 *			information.
 *
 *	...		variable arguments as indicated in `format'
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects:
 */
void	I2ErrLogFunction_(
	I2ErrHandle	dpeh,
	const char	*format, ...
)
{
	va_list		ap;

        va_start(ap, format);
	I2ErrLogVT(dpeh,-1,0,format,ap);
        va_end(ap);

	return;
}
