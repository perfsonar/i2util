/*
 *      $Id$
 */
/************************************************************************
*									*
*			     Copyright (C)  2003			*
*				Internet2				*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		aespasswd.c
 *
 *	Author:		Jeff W. Boote
 *			Internet2
 *
 *	Date:		Thu Dec 18 14:41:59 MST 2003
 *
 *	Description:	
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>
#include <libgen.h>
#include <assert.h>
#include <I2util/util.h>

#if defined HAVE_DECL_OPTRESET && !HAVE_DECL_OPTRESET
int optreset;
#endif

static void
usage(
	const char	*progname,
	const char	*msg
	)
{
	if(msg) fprintf(stderr, "%s: %s\n", progname, msg);
	fprintf(stderr,"usage: %s %s\n",
			progname,
			"[arguments] identity"
			);
	fprintf(stderr,"\n");
	fprintf(stderr,
"   -f fname       filename to edit [required option]\n"
"   -n             create keyfile\n"
"   -d             delete given identity from file\n"
	);

	return;
}

static int	do_create = 0;
static int	do_delete = 0;
static char	*keyfname;
static char	*idname;

int
main(
		int	argc,
		char	**argv
    )
{
	char		*progname;
	static char	*copts = "hf:nd";
	int		ch;
	int		lockfd;
	char		lockfname[PATH_MAX];
	char		*dname;
	size_t		len;
	struct flock	flk;
	FILE		*fromfp;
	FILE		*tofp;
	u_int8_t	aeskey[I2KEYLEN];
	char		*lbuf=NULL;
	size_t		lbuf_max;
	int		rc;

	if((progname = strrchr(argv[0],'/'))){
		progname++;
	}else{
		progname = *argv;
	}

	opterr = 0;
	while((ch = getopt(argc,argv,copts)) != -1){
		switch (ch) {
		case 'f':
			if(!(keyfname = strdup(optarg))){
				fprintf(stderr,"%s:%d: malloc(): %s\n",
						__FILE__,__LINE__,
						strerror(errno));
				exit(errno);
			}
			break;
		case 'n':
			do_create++;
			break;
		case 'd':
			do_delete++;
			break;
		case 'h':
		case '?':
		default:
			usage(progname,"");
			exit(0);
			/* UNREACHED */
		}
	}
	argc -= optind;
	argv += optind;

	if(!keyfname){
		usage(progname,"-f option is required");
		exit(1);
	}

	if(argc != 1){
		usage(progname,NULL);
		exit(1);
	}

#define STR(val)	#val
	len = strlen(argv[0]);
	if(len > I2MAXIDENTITYLEN){
		usage(progname,"identity can't be more than "
				STR(I2MAXIDENTITYLEN)
				" characters");
		exit(1);
	}

	if(!(idname = strdup(argv[0]))){
		fprintf(stderr,"%s:%s:%d: malloc(): %s\n",
				progname,__FILE__,__LINE__,
				strerror(errno));
		exit(errno);
	}

	fromfp = fopen(keyfname,"r+");
	if(do_create && (fromfp != NULL)){
		fprintf(stderr,"%s: -n option specified: %s exists\n\n",
				progname,keyfname);
		usage(progname,NULL);
		exit(1);
	}
	if(!do_create && !fromfp){
		fprintf(stderr,"%s: Can't open \'%s\': %s\nSee -n option?\n\n",
				progname,keyfname,strerror(errno));
		usage(progname,NULL);
		exit(1);
	}

	/* ensure string is nul terminated when strncpy is used */
	memset(lockfname,'\0',sizeof(lockfname));

	/* Don't let the last byte be written over */
	len = sizeof(lockfname) - 1;

	/*
	 * copy keyfname to lockfname(used as tmp var here) before calling
	 * dirname to ensure it doesn't modify keyfname.
	 */
	strncpy(lockfname,keyfname,len);
	if(!(dname = dirname(lockfname))){
		fprintf(stderr,"%s: dirname(): %s\n",progname,strerror(errno));
		exit(1);
	}

	/*
	 * what directory are we putting the lockfile in.
	 */
	strncpy(lockfname,dname,len);

	/*
	 * Concatinate lockfile name on.
	 */
	len = sizeof(lockfname) - strlen(lockfname) - 1;
	strncat(lockfname,"/",len);
	len--; /* for dir separator */
	strncat(lockfname,progname,len);
	len -= strlen(progname);
	strncat(lockfname,".lock",len);


	/*
	 * open the lock file (also used as the tempfile).
	 */
	lockfd = open(lockfname,O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);
	if(lockfd < 0){
		fprintf(stderr,"open(%s): %s\n",lockfname,strerror(errno));
		exit(1);
	}

	/*
	 * lock the file
	 */
	flk.l_start = 0;
	flk.l_len = 0;
	flk.l_type = F_WRLCK;
	flk.l_whence = SEEK_SET;
	if(fcntl(lockfd,F_SETLK,&flk) < 0){
		fprintf(stderr,"%s: Unable to lock \'%s\': %s\n",
				progname,lockfname,strerror(errno));
		exit(1);
	}

	/*
	 * wrap a FILE* around the lockfile
	 */
	if(!(tofp = fdopen(lockfd,"r+"))){
		fprintf(stderr,"%s: fdopen(): %s\n",progname,strerror(errno));
		exit(1);
	}

	if(!do_delete){
		char		*passphrase;
		char		ppbuf[1024];
		char		prompt[1024];
		I2MD5_CTX	mdc;
		size_t		pplen;

		rc = snprintf(prompt,sizeof(prompt),
				"Enter passphrase for identity '%s': ",
				idname);
		if((rc < 0) || ((size_t)rc > sizeof(prompt))){
			fprintf(stderr,"%s: Invalid identity '%s'\n",
					progname,idname);
			exit(1);
		}

		if(!(passphrase = I2ReadPassPhrase(prompt,ppbuf,
						sizeof(ppbuf),I2RPP_ECHO_OFF))){
			fprintf(stderr,"%s: I2ReadPassPhrase(): %s\n",
					progname,strerror(errno));
			exit(1);
		}
		pplen = strlen(passphrase);

		I2MD5Init(&mdc);
		I2MD5Update(&mdc,(unsigned char *)passphrase,pplen);
		I2MD5Final(aeskey,&mdc);
	}

	/*
	 * All records that don't match idname will be copied to idname.
	 */
	rc = I2ParseKeyFile(NULL,fromfp,rc,&lbuf,&lbuf_max,tofp,idname,
			NULL,NULL);
	if(rc < 0){
		fprintf(stderr,"%s:I2ParseKeyFile('%s'): error line %d\n",
				progname,keyfname,rc);
		exit(1);
	}

	if(!do_delete){
		if(I2WriteKeyLine(NULL,tofp,idname,aeskey) < 0){
			fprintf(stderr,"%s:I2WriteKeyLine('%s'): %s\n",
					progname,keyfname,strerror(errno));
			exit(1);
		}
	}

	/*
	 * Copy all remaining records.
	 */
	rc = I2ParseKeyFile(NULL,fromfp,rc,&lbuf,&lbuf_max,tofp,idname,
			NULL,NULL);
	if(rc != 0){
		fprintf(stderr,"%s:I2ParseKeyFile('%s'): error line %d\n",
				progname,keyfname,rc);
		exit(1);
	}

	/*
	 * Now close the keyfile, and rename the lockfile to the keyfile.
	 */
	if(fromfp) fclose(fromfp);
	rename(lockfname,keyfname);
	fclose(tofp);

	exit(0);
}
