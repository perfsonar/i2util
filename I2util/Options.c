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
 *	File:		Options.c
 *
 *	Author:		Jeff Boote
 *			Internet2
 *
 *	Date:		Tue Apr 23 13:23:47  2002
 *
 *	Description:	
 *
 *	This file manages a resource data base of valid command line 
 *	options. Valid options may be merged into the data base
 *	at any time and later extracted with their coresponding values
 *	as determined by the command line.
 *
 *
 *	Modified from code writen by John Clyne at UCAR...
 *
 *
 *	Based on code from UCAR DCS tools. Copyright information
 *	from UCAR follows:
 *
 *	Copyright 1997 University Corporation for Atmospheric Research,
 *	Scientific Computing Division.  All rights reserved.
 *
 *
 *	Permission to use, copy, modify and distribute this software
 *	and its	documentation for any academic, educational and
 *	scientific research purpose is hereby granted without fee,
 *	provided that the above copyright notice and this permission
 *	notice appear in all copies of this software and its
 *	documentation, and that the software is not sold and/or made
 *	the subject of any commercial activity.  Parties interested
 *	in commercial licensing should contact the copyright holder.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "optionsP.h"

#include <I2util/util.h>

static	OptTable	optTbls[MAX_TBLS];

/*
**	option descriptors currently in use
*/
static	I2Boolean	odTable[MAX_TBLS];
static	int		numUsed = 0;	/* number of used option descriptors */

/*
 *	get_option
 *	[internal]
 *	
 * on entry
 *	*name		: name to lookup
 * on exit
 *	return		: if found return command obj ptr. If name is ambiguous
 *			  return -1. If not found return NULL.
 */
static	OptRec_T	*get_option (OptRec_T *opt_rec, char *name)
{
	const char *p; 
	char	*q;
	OptRec_T *o;
	OptRec_T *found;
	int nmatches, longest;

	longest = 0;
	nmatches = 0;
	found = NULL;

	for (o = opt_rec; o && (p = o->option); o++) {
		for (q = name; *q == *p++; q++) {
			if (*q == 0)            /* exact match? */
				return (o);
		}
		if (*name) {
			if (!*q) {	/* the name was a prefix */
				if (q - name > longest) {
					longest = q - name;
					nmatches = 1;
					found = o;
				} else if (q - name == longest)
					nmatches++;
			}
		}
	}
	if (nmatches > 1)	/* ambiguous	*/
		return ((OptRec_T *)-1);
	return (found);
}

/*
 * convert an array of strings into a singe contiguous array of strings
 */
static	char	*copy_create_arg_string(
	I2ErrHandle	eh,
	char		**argv,
	int		argc
) {
	char	*s,*t;
	int	i,
		len;	/* lenght of new arg string	*/

	for(len=0,i=0; i<argc; i++) {
		len += strlen(argv[i]);
		len++;	/* one for the space	*/
	}

	if ((s = (char *) malloc(len +1)) == NULL) {
		I2ErrLog(eh, "malloc(%d) : %M", len+1);
		return(NULL);
	}
	s = strcpy(s, argv[0]);
	for(i=1, t=s; i<argc; i++) {
		t += strlen(t);
		*t++ = '\0';
		(void) strcpy(t, argv[i]);
	}
	return(s);
}

/*
 * convert a space seprated string to an array of contiguous strings
 */
static	char	*fmt_opt_string(
	I2ErrHandle	eh,
	char		*arg,
	int		n
) {
	int	i;
	char	*s;

	for (i=1, s = arg; i<n; i++) {
		while(*s && ! isspace(*s)) s++;

		if (! *s) {
			I2ErrLog(eh, "Arg string invalid: %s", arg);
			return(NULL);
		}
		*s++ = '\0';

		while(*s && isspace(*s)) s++;
	}
	return(arg);
}

/*
**
**	Type converters. The following functions convert string representations
**	of data into their primitive data formats. A NULL 'from' value
**	should result in a reasonable value.
**
*/

/*
 *	I2CvtToInt()
 *
 *	convert a ascii string to its integer value
 */
int	I2CvtToInt(
	I2ErrHandle	eh,
	const char	*from,	/* the string	*/
	void		*to
) {
	int	*iptr	= (int *) to;

	if (! from) {
		*iptr = 0;
	}
	else if (sscanf(from, "%d", iptr) != 1) {
		I2ErrLog(eh, "Convert(%s) to int failed", from);
		return(-1);
	}
	return(1);
}

/*
 *	I2CvtToFloat()
 *
 *	convert a ascii string to its floating point value
 */
int	I2CvtToFloat(
	I2ErrHandle	eh,
	const char	*from,	/* the string	*/
	void		*to
) {
	float	*fptr	= (float *) to;

	if (! from) {
		*fptr = 0.0;
	}
	else if (sscanf(from, "%f", fptr) != 1) {
		I2ErrLog(eh, "Convert(%s) to float failed", from);
		return(-1);
	}
	return(1);
}

/*
 *	I2CvtToChar()
 *
 *	convert a ascii string to a char.
 */
int	I2CvtToChar(
	I2ErrHandle	eh,
	const char	*from,	/* the string	*/
	void		*to
) {
	char	*cptr	= (char *) to;

	if (! from) {
		*cptr = '\0';
	}
	else if (sscanf(from, "%c", cptr) != 1) {
		I2ErrLog(eh, "Convert(%s) to char failed", from);
		return(-1);
	}
	return(1);
}


/*
 *	I2ConvetToBoolean()
 *
 *	convert a ascii string containing either "true" or "false" to
 *	to True or False
 */
int	I2CvtToBoolean(
	I2ErrHandle	eh,
	const char	*from,	/* the string	*/
	void		*to
) {
	I2Boolean	*bptr	= (I2Boolean *) to;

	if (! from) {
		*bptr = False;
	}
	else if (strcmp("true", from) == 0) {
		*bptr = True;
	}
	else if (strcmp("false", from) == 0) {
		*bptr = False;
	}
	else {
		I2ErrLog(eh, "Convert(%s) to boolean failed", from);
		return(-1);
	}
	return(1);
}

/*
 *	I2CvtToString()
 *
 */
int	I2CvtToString(
	I2ErrHandle	eh,
	const char	*from,	/* the string	*/
	void		*to
) {
	char	**sptr	= (char **) to;

	*sptr = (char *) from;

	return(1);
}

/*
 *	I2CvtToDimension2D()
 *
 *	convert a ascii string to a dimension.
 */
int	I2CvtToDimension2D(
	I2ErrHandle	eh,
	const char	*from,	/* the string	*/
	void		*to
) {
	I2Dimension2D_T	*dptr	= (I2Dimension2D_T *) to;

	if (! from) {
		dptr->nx = dptr->ny = 0;
	}
	else if (! (
		(sscanf(from, "%dx%d", &(dptr->nx), &(dptr->ny)) == 2) ||
		(sscanf(from, "%dX%d", &(dptr->nx), &(dptr->ny)) == 2))) {

		I2ErrLog(eh, "Convert(%s) to dimension failed", from);
		return(-1);
	}
	return(1);
}


/*
 *	I2OpenOptionTbl()
 *	[exported]
 *
 *
 *	Instantiate an option table. I2OpenOptionTbl() creates an instance
 *	of an option table and returns an option descriptor (od) with
 *	which to reference it.
 *
 * on exit
 *	return		: -1 => failure, else an option descriptor is returned
 */
int	I2OpenOptionTbl(I2ErrHandle eh)
{
	int		od;		/* option descriptor	*/
	OptRec_T	*opt_rec;
	int		tmp;
	static		I2Boolean first = True;

	/*
	** clear the option descriptor table first time through
	*/
	if (first) {
		memset((char *) odTable, 0, sizeof(odTable));
		first = ! first;
	}

	if (numUsed >= MAX_TBLS) {
		I2ErrLog(eh, "Too many option tables open");
		return(-1);
	}

	/* find a free index    */
	for(od=0; od<MAX_TBLS && odTable[od]; od++);

	tmp = sizeof(OptRec_T) * SMALL_BLOCK;
	if (! (opt_rec = (OptRec_T *) malloc(tmp))) {
		I2ErrLog(eh, "malloc(%d) : %M",tmp);
		return(-1);
	}
	opt_rec->option = NULL;
	opt_rec->default_value = NULL;
	opt_rec->value = NULL;
	optTbls[od].opt_rec = opt_rec;
	optTbls[od].size = SMALL_BLOCK;
	optTbls[od].num = 0;
	optTbls[od].eh = eh;

	odTable[od] = True;
	numUsed++;

	return(od);
}


/*
 *	I2CloseOptionTbl()
 *	[exported]
 *
 *	Free an instance of an option table.
 *
 * on entry
 *	od		: reference the option table to free
 * on exit
 *	return		: -1 => error, else ok
 */
I2CloseOptionTbl(int od)
{
	int	i;
	I2ErrHandle	eh;

	if (! odTable[od]) return(-1);	/* invalid option descriptor	*/

	eh = optTbls[od].eh;

	if (optTbls[od].size > 0) {
		OptRec_T	*opt_rec = optTbls[od].opt_rec;
		int		i;

		for (i=0; i<optTbls[od].num; i++) {
			if (opt_rec[i].default_value && 
				(opt_rec[i].default_value != opt_rec[i].value)){

				free((char *) opt_rec[i].default_value);
			}
			opt_rec[i].default_value = NULL;

			if (opt_rec[i].value) {
				free(opt_rec[i].value);
			}
		}

		free((char *) optTbls[od].opt_rec);
		optTbls[od].size = 0;
		optTbls[od].num = 0;
	}

	odTable[od] = False;
	numUsed--;

	return(0);
}

/*
 *	I2GetOptions
 *	[exported]
 *
 *	I2GetOptions may be called after I2ParseOptionTable in order to 
 *	retrieve values from the option table. The values in the option
 *	data base are converted into appropriate types by calling the 
 *	type converter specifed in 'options' for each option.
 *
 *	The fields of the Option struct are as follows:
 *
 *	char		*option_name;	the options name - used to look the
 *					requested option in the option table.
 *	int		(*type_conv)();	option type converter - converts the
 *					string representation of the option
 *					value into a specified format and store
 *					the result at address given by 'offset'
 *	void		*offset;	offset of return address
 *	int		size;		size of option in bytes - if there are
 *					multiple arguments for a single option
 *					their destination address is computed
 *					by adding 'size' to 'offset' as
 *					appropriate.
 *
 *	I2GetOptions() invokes I2ErrLog() on failure.
 *
 *
 * on entry
 *	od		: option descriptor
 *	options		: Null terminated list of options to be returned
 *
 * on exit
 *	return		: -1 => failure, else OK
 */
I2GetOptions(
	int 		od,
	const I2Option	*options
) {

	int		i, j;
	char		*s;
	int		arg_count;
	OptRec_T	*opt_rec;

	OptRec_T	*orptr;
	void		*offset;
	I2ErrHandle	eh;

	if (! odTable[od]) return(-1);	/* invalid option descriptor	*/

	eh = optTbls[od].eh;

	opt_rec = optTbls[od].opt_rec;

	for (i = 0; options[i].option_name; i++ ) {

		/*
		 * find the option */
		orptr = get_option(opt_rec, options[i].option_name);

		if (! orptr ) {
			I2ErrLog(eh, 
				"Option %s unknown", 
				options[i].option_name
			);
			return(-1);
		}

		/*
		 * zero arg_count options really do have a single argument
		 */
		arg_count = orptr->arg_count ? orptr->arg_count : 1;

		offset = options[i].offset;
		for(j=0,s=orptr->value; j<arg_count; j++){
			if (options[i].type_conv(eh, s, offset) < 0) {
				I2ErrLog(
					eh, 
					"Type converter for option \"-%s\" "
					"failed to convert argument \"%s\"", 
					options[i].option_name, s
				);
				return(-1);
			}
			if (s) s += strlen(s) + 1;
			offset = (char *) offset + options[i].size;
		}
	}
	return(1);
}


/*
 *	I2LoadOptionTable
 *	[exported]
 *
 *	Add a list of valid application options to the option table. All 
 *	options are assumed to begin with a '-' and contain 0 or more arguments.
 *
 *	The fields of the I2OptDescRec struct are as follows: 
 *
 *	char	*option;	name of option without preceeding '-'
 *	int	arg_count;	num args expected by option
 *	char	*value;		default value for the argument - if 'arg_count'
 *				is zero 'value' is ignored. if 'arg_count' is 
 *				greater than 1 'value' is a string of space 
 *				separted arguments.
 *	char	*help;		help string for option
 *
 *	I2LoadOptionTable() invokes I2ErrLog() on error.
 *
 * on entry
 *	od		: option descriptor
 *	optd		: Null terminated list of options
 *
 * on exit
 *	return		: -1 => failure, else OK.
 */
I2LoadOptionTable(
	int			od,
	const I2OptDescRec	*optd
) {

	int	i,j,n;
	int	num;
	OptRec_T	*opt_rec;
	unsigned	tmp;
	I2ErrHandle	eh;

	if (! odTable[od]) return(-1);	/* invalid option descriptor	*/

	eh = optTbls[od].eh;

	if (! optd[0].option) return (0);

	opt_rec = optTbls[od].opt_rec;

	/*
	 * make sure there are no duplicate names in the table. This only 
	 * compares new entries with old entries. If there are duplicate
	 * entries within the new entries they won't be found. Count
	 * how many options there are as well
	 */
	for (j = 0, num = 0; optd[j].option; j++) {
		for ( i = 0; i < optTbls[od].num; i++) {
			if (strcmp(opt_rec[i].option, optd[j].option) == 0){
				I2ErrLog(eh, 
					"Option %s already in option table", 
					optd[j].option
				);
				return(-1);	/* duplicate entry	*/
			}
		}
		num++;
	}

	if ((optTbls[od].num + num + 1) >= optTbls[od].size) {

		optTbls[od].size += num+1;
		tmp = optTbls[od].size * sizeof(OptRec_T);

		if (! (opt_rec = (OptRec_T *)realloc((char *) opt_rec, tmp))){
			I2ErrLog(eh, "realloc(%d) : %M", tmp);
			return(-1);
		}
		optTbls[od].opt_rec = opt_rec;
	}

	/*
	 * copy all the options into the option table allocating memory
	 * as necessary.
	 */
	for (i = 0, n = optTbls[od].num; i < num; i++, optTbls[od].num++, n++) {
		char	*value;
		char	*s;

		if (optd[i].arg_count == 0) {
			s = "false";
		}
		else {
			s = optd[i].value;
		}

		if (s) {
			value = malloc(strlen(s) + 1);
			if (! value) {
				I2ErrLog(eh, "malloc(%d) : %M", strlen(s +1));
				return(-1);
			}
			(void) strcpy(value, s);
		}
		else {
			value = NULL;
		}

		if (! optd[i].option) {
			I2ErrLog(eh, ("Invalid option descriptor"));
			return(-1);
		}

		if (value) {
			value = fmt_opt_string(eh, value, optd[i].arg_count);
			if (! value) {
				return(-1);
			}
		}

		opt_rec[n].default_value = value;
		opt_rec[n].value = opt_rec[n].default_value;
		opt_rec[n].option = optd[i].option;
		opt_rec[n].arg_count = optd[i].arg_count;
		opt_rec[n].help = optd[i].help;

	}
	opt_rec[n].option = NULL;	/* terminate list  */
	
	return(1);
}

/*
 *	I2RemoveOptions
 *	[exported]
 *
 *	Remove a list of application options from the option table. 
 *	Only the 'option' field of the I2OptDescRec struct is referenced, 
 *	all other fields are ignored. The options to be freed must have
 *	previously been set with a call to I2LoadOptionTable().
 *
 *
 * on entry
 *	od		: option descriptor
 *	optd		: Null terminated list of options
 *
 */
void	I2RemoveOptions(
	int			od,
	const I2OptDescRec	*optd
) {
	int	i,j;
	OptRec_T	*opt_rec;

	if (! odTable[od]) return;	/* invalid option descriptor	*/

	if (! optd[0].option) return ;

	opt_rec = optTbls[od].opt_rec;

	/*
	 * look for the option in the option table.
	 */
	for (j=0; optd[j].option; j++) {
		for (i=0; i < optTbls[od].num; i++) {
			if (strcmp(opt_rec[i].option, optd[j].option) == 0){
				optTbls[od].num--;
				break;
			}
		}
		/*
		 * we won't enter this loop unless a match is found
		 * and optTbls[od].num is decremented
		 */
		for( ; i<optTbls[od].num; i++) {
			opt_rec[i] = opt_rec[i+1];
		}
		opt_rec[i].option = NULL;
	}
}


/*
 *	I2ParseOptionTable
 *	[exported]
 *
 *	parse the option table with the command line options. After the 
 *	option table is created with I2LoadOptionTable this function may
 *	be called to change values of options in the table. We assume
 *	argv[0] is the program name and we leave it unmolested
 *
 *	I2ErrLog() is invoked on error.
 *
 * on entry:
 *	od		: option descriptor
 *	**argv		: list of command line args
 *	*argc		: num elements in argv
 *	*optds		: additional options to merge into the option table
 * on exit
 *	**argv		: contains options not found in the option table
 *	*argc		: num elements in argv
 *	return		: -1 => failure, else OK
 */
int	I2ParseOptionTable(
	int			od,
	int			*argc,
	char			**argv,
	const	I2OptDescRec	*optds
) {
	int	i;
	char	**next = argv + 1;
	OptRec_T	*orptr;
	OptRec_T	*opt_rec;
	OptRec_T	*o;
	int		new_argc = 1;
	I2ErrHandle	eh;

	if (! odTable[od]) return(-1);	/* invalid option descriptor	*/

	eh = optTbls[od].eh;

	/*
	 * if any options to be merged do so
	 */
	if (optds) {
		if (I2LoadOptionTable(od, optds) == -1) return(-1);
	}
	opt_rec = optTbls[od].opt_rec;

	if (! argv) return(1);

	/*
	**	Restore default values for options in option table
	*/
	for (o = opt_rec; o && o->option; o++) {
		if (o->value && (o->value != o->default_value))free(o->value);
		o->value = o->default_value;
	}
	
	/*
	 * look for matches between elements in argv and in the option table
	 */
	for (i = 1; i < *argc; i++) {

		if (*argv[i] == '-') {	/* is it an option specifier?	*/
			orptr = get_option(opt_rec, (char *) (argv[i] + 1));
		}
		else {
			orptr = (OptRec_T *) NULL;	/* not a specifier */ 
		}

		if (orptr == (OptRec_T *) -1) {
			I2ErrLog(eh, "Ambiguous option: %s", argv[i]);
			return(-1);
		}

		/*
		 * if no match found leave option in argv along with anything
		 * that follows that is not an option specifier
		 */
		if (! orptr ) {	/* not found	*/
			*next = argv[i];
			new_argc++;
			next++;
			while(i+1 < *argc && *argv[i+1] != '-') {
				i++;
				new_argc++;
				*next  = argv[i];
				next++;
			}
			continue;
		}

		/*
		 * make sure enough args for option
		 */
		if ((i + orptr->arg_count) >= *argc) {
			I2ErrLog(eh, 
				"Option -%s expects %d args",
				orptr->option, orptr->arg_count
				);
			return(-1);
		}

		/*
		 * Options with no args are a special case. Assign them
		 * a value of true. They are false by default
		 */
		if (orptr->arg_count == 0) {
			orptr->value = strdup("true");
			continue;
		}


		/*
		 * convert the arg list to a single string and stash it
		 * in the option table
		 */
		orptr->value = copy_create_arg_string(
			eh,&argv[i+1],orptr->arg_count
		);
		if (! orptr->value) {
			return(-1);
		}
		i += orptr->arg_count;
				
	}
	*argc = new_argc;
	argv[*argc] = NULL;

	return(1);
}

/*
 * Leave this out for now - not worth including ATOARGV stuff.
 * -boote
 */
#if	NOT
/*
 *	I2ParseEnvOptions()
 *
 *	I2ParseEnvOptions() is analogous to I2ParseOptionTable except that
 *	it takes a list of environment variables and their coresponding
 *	option names instead of an argv list. If a given environment
 *	variable is set its value is used as the argument value for the 
 *	option with  which it is associated. If the environment variable
 *	is not set the option/environemnt variable pair are ignored.
 *
 * on entry
 *	od		: option descriptor
 *	*envv		: NUll-terminated list of option/env pairs
 *	*optds		: additional options to merge into the option table
 *
 * on exit
 *	return		: -1 => error, else OK
 */
int	I2ParseEnvOptions(
	int			od,
	const I2EnvOpt		*envv,
	const	I2OptDescRec	*optds
) {
	int	envc;		/* size of envv list		*/
	const I2EnvOpt	*envptr;	/* pointer to envv		*/
	char	**argv;		/* arg vector created from envv	*/
	int	argc;		/* size of argv list		*/
	char	*arg_string;	/* env variable value		*/
	char	buf[MAX_ATOARGV_STRING];
	I2ErrHandle	eh;

	if (! odTable[od]) return(-1);	/* invalid option descriptor	*/

	eh = optTbls[od].eh;

	/*
	 * if any options to be merged do so
	 */
	if (optds) {
		if (I2LoadOptionTable(od, optds) == -1) return(-1);
	}

	/*
 	 * look for environment variables. Generate the argument vector, argv
	 */
	for (envptr = envv; envptr->option; envptr++) {
		if (arg_string = getenv(envptr->env_var)) {

			(void) strcpy(buf, "-");
			(void) strcat(buf, envptr->option);
			(void) strcat(buf, " ");
			(void) strncat(buf,arg_string,sizeof(buf)-strlen(buf)-1);
			if (! (argv = I2AToArgv(eh, buf, "dummy", &argc))) {
				return(-1);
			}
			if (I2ParseOptionTable(
				od, &argc, argv, (I2OptDescRec *) NULL) == -1) {

				return(-1);
			}

			if (argc != 1) {
				I2ErrLog(eh, 
					"Environment variable %s='%s'",
					envptr->env_var, arg_string
				);
				return(-1);
			}
		}
	}
	return(1);
}
#endif

/*
 *	I2PrintOptionHelp()
 *
 *	Print help about each option.
 *
 *	Each option known to the option table is printed, followed by
 *	the string "arg0 arg1 ... argN", where N+1 is the number of arguments
 *	expected by the option, followed by the contents of the 'help' field.
 *
 * on entry
 *	od		: option descriptor
 *	*fp		: file pointer where output is written.
 */
void	I2PrintOptionHelp(
	int	od,
	FILE	*fp
) {
	int		i,j;
	char		buf[30];
	char		sbf[20];
	OptRec_T	*opt_rec;

	if (! odTable[od]) return;	/* invalid option descriptor	*/

	opt_rec = optTbls[od].opt_rec;

	for(i=0; i<optTbls[od].num; i++) {
		sprintf(buf, "    -%-8.8s", opt_rec[i].option);
		if (opt_rec[i].arg_count < 4) {
			for(j=0; j<opt_rec[i].arg_count; j++) {
				sprintf(sbf, " arg%d", j);
				if (strlen(sbf) + strlen(buf) < sizeof(buf)) {
					(void) strcat(buf, sbf);
				}
				else {
					break;
				}
			}
		}
		else {
			sprintf(sbf," arg0 .. arg%d",opt_rec[i].arg_count-1);
			(void) strcat(buf, sbf);
		}
		(void) fprintf(fp, buf);
		for(j=strlen(buf); j<sizeof(buf); j++) {
			putc(' ', fp);
		}
		if (opt_rec[i].help) {
			(void) fprintf(fp, "%s\n", (char *) opt_rec[i].help);
		}
		else {
			(void) fprintf(fp, "\n");
		}
	}
}
