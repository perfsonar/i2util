/*
 * ex: set tabstop=4 ai expandtab softtabstop=4 shiftwidth=4:
 * -*- mode: c-basic-indent: 4; tab-width: 4; indent-tabls-mode: nil -*-
 *      $Id$
 */
/************************************************************************
*                                                                       *
*                           Copyright (C)  2006                         *
*                               Internet2                               *
*                           All Rights Reserved                         *
*                                                                       *
************************************************************************/
/*
 *    File:         pfstore.c
 *
 *    Author:       Jeff W. Boote
 *                  Internet2
 *
 *    Date:         Fri Oct 20 13:01:39 MDT 2006
 *
 *    Description:    
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
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
"   -n             create pffile\n"
"   -d             delete given identity from file\n"
	);

	return;
}

static int      do_create = 0;
static int      do_delete = 0;
static char     *pffname;
static char     *idquery = NULL;
static char     *idname = NULL;
static char     *pf_bytes = NULL;
static size_t   pf_len = 0;

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
    char        *passphrase = NULL;
	char		*pbuf=NULL;
	size_t		pbuf_max=0;
	char		*lbuf=NULL;
	size_t		lbuf_max=0;
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
			if(!(pffname = strdup(optarg))){
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

	if(!pffname){
		usage(progname,"-f option is required");
		exit(1);
	}

	if(argc != 1){
		usage(progname,NULL);
		exit(1);
	}

#define STR(val)	#val
#define STREXP(val)     STR(val)
	if(!(idquery = strdup(argv[0]))){
		fprintf(stderr,"%s:%s:%d: malloc(): %s\n",
				progname,__FILE__,__LINE__,
				strerror(errno));
		exit(errno);
	}

	fromfp = fopen(pffname,"r+");
	if(do_create && (fromfp != NULL)){
		fprintf(stderr,"%s: -n option specified: %s exists\n\n",
				progname,pffname);
		usage(progname,NULL);
		exit(1);
	}
	if(!do_create && !fromfp){
		fprintf(stderr,"%s: Can't open \'%s\': %s\nSee -n option?\n\n",
				progname,pffname,strerror(errno));
		usage(progname,NULL);
		exit(1);
	}

	/* ensure string is nul terminated when strncpy is used */
	memset(lockfname,'\0',sizeof(lockfname));

	/* Don't let the last byte be written over */
	len = sizeof(lockfname) - 1;

	/*
	 * copy pffname to lockfname(used as tmp var here) before calling
	 * dirname to ensure it doesn't modify pffname.
	 */
	strncpy(lockfname,pffname,len);
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
	strncat(lockfname,"/.",len);
	len -= 2; /* strlen("/.") */
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
		char		prompt[2048];

		rc = snprintf(prompt,sizeof(prompt),
				"Enter passphrase for identity '%s': ",
				idquery);
		if((rc < 0) || ((size_t)rc > sizeof(prompt))){
			fprintf(stderr,"%s: Identity '%s': Name too long\n",
					progname,idquery);
			exit(1);
		}

		if(!(passphrase = I2ReadPassPhraseAlloc(prompt,I2RPP_ECHO_OFF,
                                &pbuf,&pbuf_max))){
			fprintf(stderr,"%s: I2ReadPassPhrase(): %s\n",
					progname,strerror(errno));
			exit(1);
		}
	}

	/*
	 * All records that don't match idquery will be copied to tofp.
	 */
	rc = I2ParsePFFile(NULL,fromfp,tofp,0,idquery,
                &idname,&pf_bytes,&pf_len,&lbuf,&lbuf_max);
	if(rc < 0){
		fprintf(stderr,"%s: I2ParseKeyFile('%s'): error line %d\n",
				progname,pffname,rc);
		exit(1);
	}

	if(do_delete){
		/*
		 * If we are deleting, the identity should have been found.
		 */
		if(!rc){
			fprintf(stderr,"%s: Identity '%s' not found\n",
					progname,idquery);
			exit(1);
		}
	}else{
		/*
		 * Not deleting, add the new/changed pf line.
		 */
		if(I2WritePFLine(NULL,tofp,idquery,
                    (uint8_t *)passphrase,strlen(passphrase),
                    &lbuf,&lbuf_max) < 0){
			fprintf(stderr,"%s:I2WritePFLine('%s'): %s\n",
					progname,pffname,strerror(errno));
			exit(1);
		}
	}

	/*
	 * Copy all remaining records.
	 */
	rc = I2ParsePFFile(NULL,fromfp,tofp,rc,idquery,
                &idname,&pf_bytes,&pf_len,&lbuf,&lbuf_max);
	if(rc != 0){
		fprintf(stderr,"%s:I2ParsePFFile('%s'): error line %d\n",
				progname,pffname,rc);
		exit(1);
	}

	/*
	 * Now close the pffile, and rename the lockfile to the pffile.
	 */
	if(fromfp) fclose(fromfp);
	rename(lockfname,pffname);
	fclose(tofp);

	exit(0);
}
