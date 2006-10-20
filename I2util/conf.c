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
 *	File:		conf.c
 *
 *	Author:		Jeff W. Boote
 *			Internet2
 *
 *	Date:		Tue Sep  9 16:13:25 MDT 2003
 *
 *	Description:	
 */
#include <I2util/util.h>

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/*
 * Function:	I2GetConfLine
 *
 * Description:	
 * 		Read a single line from a file fp. remove leading whitespace,
 * 		skip blank lines and comment lines. Put the result in the
 * 		char buffer pointed at by lbuf, growing it as necessary.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
int
I2GetConfLine(
	I2ErrHandle	eh,
	FILE		*fp,
	int		rc,
	char		**lbuf,
	size_t		*lbuf_max
	)
{
	int	c;
	char	*line = *lbuf;
	size_t	i=0;

	while((c = fgetc(fp)) != EOF){

		/*
		 * If c is a newline - increment the row-counter.
		 * If lbuf already has content - break out, otherwise
		 * this was a leading blank line, continue until there
		 * is content.
		 */
		if(c == '\n'){
			rc++;
			if(i) break;
			continue;
		}

		/*
		 * swallow comment lines
		 */
		if(!i && c == '#'){
			while((c = fgetc(fp)) != EOF){
				if(c == '\n'){
					rc++;
					break;
				}
			}
			continue;
		}

		/*
		 * swallow leading whitespace
		 */
		if(!i && isspace((int)c)){
			continue;
		}

		/*
		 * Check for line continuation.
		 */
		if(c == '\\'){
			if(fgetc(fp) == '\n'){
				rc++;
				continue;
			}
			I2ErrLogP(eh,EINVAL,"Invalid use of \'\\\'");
			return -rc;
		}

		/*
		 * make sure lbuf is large enough for this content
		 */
                if(i+2 > *lbuf_max){
                    while(i+2 > *lbuf_max){
                        *lbuf_max += I2LINEBUFINC;
                    }
                    *lbuf = realloc(line,sizeof(char) * *lbuf_max);
                    if(!*lbuf){
                        if(line){
                            free(line);
                        }
                        I2ErrLog(eh,"realloc(%u): %M",*lbuf_max);
                        return -rc;
                    }
                    line = *lbuf;
                }

		/*
		 * copy char
		 */
		line[i++] = c;
	}

	if(!i){
		return 0;
	}

	line[i] = '\0';

	if(c == EOF){
		rc++;
	}

	return rc;
}

/*
 * Function:	I2ReadConfVar
 *
 * Description:	
 * 	Read the next non-comment line from the config file. The file
 * 	should be in the format of:
 * 		key [value] [#blah comment]
 *
 * 	key and value are delineated by whitespace.  All leading and
 * 	trailing whitespace is ignored. A trailing comment is legal and
 * 	all charactors between a # and the trailing \n are ignored.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
int
I2ReadConfVar(
	FILE	*fp,
	int	rc,
	char	*key,
	char	*val,
	size_t	max,
	char	**lbuf,
	size_t	*lbuf_max
	)
{
	char	*line;

	if((rc = I2GetConfLine(NULL,fp,rc,lbuf,lbuf_max)) > 0){

		/*
		 * Pull off key.
		 */
		if(!(line = strtok(*lbuf,I2WSPACESET))){
			rc = -rc;
			goto DONE;
		}

		/*
		 * Ensure key is not too long.
		 */
		if(strlen(line)+1 > max){
			rc = -rc;
			goto DONE;
		}
		strcpy(key,line);

		if((line = strtok(NULL,I2WSPACESET))){
			/*
			 * If there is no "value" for this key, then
			 * a comment is valid.
			 */
			if(*line == '#'){
				val[0] = '\0';
				goto DONE;
			}

			/*
			 * Ensure value is not too long.
			 */
			if(strlen(line)+1 > max){
				rc = -rc;
				goto DONE;
			}
			strcpy(val,line);
		}
		else{
			val[0] = '\0';
		}

		/*
		 * Ensure there is no trailing data
		 */
		if((line = strtok(NULL,I2WSPACESET))){
			/*
			 * Comments are the only valid token.
			 */
			if(*line != '#'){
				rc = -rc;
			}
		}
	}

DONE:
	return rc;
}

/*
 * This is very similar to I2GetConfLine but does not allow line
 * continuation - and copies comment/blank lines to tofp.
 */
static int
parsefileline(
	I2ErrHandle	eh,
	FILE		*fp,
	int		rc,
	char		**lbuf,
	size_t		*lbuf_max,
	FILE		*tofp
	)
{
	int	c;
	size_t	i;
	char	*line = *lbuf;
	size_t	nc=0; /* number of "significant" chars in the line */
	size_t	ns=0; /* number of leading spaces */

	while((c = fgetc(fp)) != EOF){

		/*
		 * If c is a newline - increment the row-counter.
		 * If lbuf already has content - break out, otherwise
		 * this was a blank line, continue until there
		 * is content.
		 */
		if(c == '\n'){
			rc++;
			if(nc) break;
			/* don't worry about leading spaces for blank lines */
			if(tofp) fprintf(tofp,"\n");
			ns=0;
			continue;
		}

		/*
		 * swallow comment lines
		 */
		if(!nc && c == '#'){
			if(tofp){
				/* preserve leading spaces */
				for(i=0;i<ns;i++) fprintf(tofp," ");
				fprintf(tofp,"#");
			}
			while((c = fgetc(fp)) != EOF){
				if(tofp)
					fprintf(tofp,"%c",c);
				if(c == '\n'){
					rc++;
					break;
				}
			}
			continue;
		}

		/*
		 * swallow leading whitespace
		 * (These will be preserved for comment lines - and removed
		 * for all other lines.)
		 */
		if(!nc && isspace((int)c)){
			ns++;
			continue;
		}

                /*
                 * Leading spaces are only allowed in comment lines
                 */
                if(ns){
		    I2ErrLog(eh,"Leading spaces not allowed: line %d",rc);
                    return -rc;
                }

		/*
		 * make sure lbuf is large enough for this content
		 */
                if(nc+2 > *lbuf_max){
                    while(nc+2 > *lbuf_max){
                        *lbuf_max += I2LINEBUFINC;
                    }
                    *lbuf = realloc(line,sizeof(char) * *lbuf_max);
                    if(!*lbuf){
                        if(line){
                            free(line);
                        }
                        I2ErrLog(eh,"realloc(%u): %M",*lbuf_max);
                        return -rc;
                    }
                    line = *lbuf;
                }

		/*
		 * copy char
		 */
		line[nc++] = c;
	}

	if(!nc){
		return 0;
	}

	line[nc] = '\0';

	if(c == EOF){
		rc++;
	}

	return rc;
}

/*
 * Function:	I2ParseKeyFile
 *
 * Description:	
 * 		Read a single line from a file fp. remove leading whitespace,
 * 		skip blank lines and comment lines. Put the result in the
 * 		char buffer pointed at by lbuf, growing it as necessary.
 *
 * 		Read a single identity/key from the keyfile. If tofp is set,
 * 		then copy all "unmatched" lines from fp to tofp while parsing
 * 		the file. If id_query is set, only return the entry that
 * 		matches (if any does) skipping all others - and copying them
 * 		to tofp if needed. A quick way to simply copy all remaining
 * 		records to the tofp is to specify an id_query to a 0 length
 * 		string (i.e. id_query[0] == '\0').
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
int
I2ParseKeyFile(
	I2ErrHandle	eh,
	FILE		*fp,
	int		rc,
	char		**lbuf,
	size_t		*lbuf_max,
	FILE		*tofp,
	const char	*id_query,
	char		*id_ret, /* [I2MAXIDENTITYLEN+1] or null */
	uint8_t	*key_ret /* [I2KEYLEN] or null */
	)
{
	char		*line;
	int		i;
	char		rbuf[I2MAXIDENTITYLEN+1]; /* add one extra byte */
	char		*keystart;
	uint8_t	kbuf[I2KEYLEN];

	/*
	 * If there is no keyfile, parsing is very, very fast.
	 */
	if(!fp){
		return 0;
	}

	/*
	 * Fetch each non-blank, non-comment line from the keyfile.
	 * completely validate each line and then determine at the
	 * end of the loop if the caller is interested in this line or not.
	 * (This strict interpretation of the syntax of the keyfile should
	 * help find errors as quickly as possible instead of letting them
	 * hide until they actually bite someone.)
	 */
	while((rc = parsefileline(eh,fp,rc,lbuf,lbuf_max,tofp)) > 0){

		line = *lbuf;

		i=0;
		/*
		 * Can potentially copy I2MAXIDENTITYLEN+1 bytes: rbuf is
		 * sized to handle this and the next 'if' is setup to
		 * detect this error condition.
		 */
		while(i <= I2MAXIDENTITYLEN){
			if(isspace((int)*line) || (*line == '\0')){
				break;
			}
			rbuf[i++] = *line++;
		}

		if( i > I2MAXIDENTITYLEN){
			I2ErrLogP(eh,EINVAL,"Invalid identity name (too long)");
			return -rc;
		}
		rbuf[i] = '\0';

		/*
		 * Get the hexkey
		 */
		while(isspace((int)*line)){
			line++;
		}

		keystart = line;
		i=0;
		while(*line != '\0'){
			if(isspace((int)*line)){
				break;
			}
			i++;
			line++;
		}

		/*
		 * If i is not equal to the hex-encoded length of a key...
		 */
		if(i != (I2KEYLEN*2)){
			I2ErrLogP(eh,EINVAL,"Invalid key length");
			return -rc;
		}

		/*
		 * Make sure the only thing trailing the key is a comment
		 * or whitespace.
		 */
		while(*line != '\0'){
			if(*line == '#'){
				break;
			}
			if(!isspace((int)*line)){
				I2ErrLogP(eh,EINVAL,"Invalid chars after key");
				return -rc;
			}
			line++;
		}

		if(!I2HexDecode(keystart,kbuf,I2KEYLEN)){
			I2ErrLogP(eh,EINVAL,"Invalid key: not hex?");
			return -rc;
		}

		/*
		 * If a specific "identity" is being searched for: skip/copy
		 * lines that don't match and continue parsing the file.
		 */
		if(id_query && strncmp(rbuf,id_query,I2MAXIDENTITYLEN)){
			/*
			 * Write line to tofp, then 'continue'
			 */
			if(tofp) fprintf(tofp,"%s\n",*lbuf);
			continue;
		}

		/*
		 * caller is interested in this record - return the values.
		 */
		if(id_ret){
			strncpy(id_ret,rbuf,sizeof(rbuf));
		}

		if(key_ret){
			memcpy(key_ret,kbuf,I2KEYLEN);
		}

		break;
	}

	return rc;
}

/*
 * Function:	I2WriteKeyLine
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
int
I2WriteKeyLine(
	I2ErrHandle	eh,
	FILE		*fp,
	const char	*id,
	const uint8_t	*key
	)
{
	int	ret;
	char	hbuf[(I2KEYLEN*2)+1]; /* size for hex version */

	if(!id || (id[0] == '\0') || (strlen(id) > I2MAXIDENTITYLEN)){
		I2ErrLogP(eh,EINVAL,"I2WriteKeyLine(): Invalid identity name");
		return -1;
	}

	I2HexEncode(hbuf,key,I2KEYLEN);

	/*
	 * if fprintf has an error, set ret < 0 for a failure return.
	 */
	ret = fprintf(fp,"%s\t%s\n",id,hbuf);

	if(ret > 0) ret = 0;

	return ret;
}

/*
 * Function:	I2ParsePFFile
 *
 * Description:	
 * 		Read a single line from a file fp. remove leading whitespace,
 * 		skip blank lines and comment lines. Put the result in the
 * 		char buffer pointed at by lbuf, growing it as necessary.
 *
 * 		Read a single identity/pass-phrase from the pffile. If tofp
 * 		is set, then copy all "unmatched" lines from fp to tofp while
 * 		parsing the file. If id_query is set, only return the entry that
 * 		matches (if any does) skipping all others - and copying them
 * 		to tofp if needed. A quick way to simply copy all remaining
 * 		records to the tofp is to specify an id_query to a 0 length
 * 		string (i.e. id_query[0] == '\0').
 *
 * 		The entry that is returned currently breaks the line, so
 * 		*lbuf can not just be written back to the file directly.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
int
I2ParsePFFile(
	I2ErrHandle eh,
	FILE	    *fp,
	FILE	    *tofp,
	int	    rc,
	const char  *id_query,
	char	    **id_ret,   /* points in lbuf */
	uint8_t	    **pf_ret,   /* points in lbuf */
        size_t      *pf_len,
	char	    **lbuf,
	size_t	    *lbuf_max
	)
{
    char    *line;
    char    *ts;
    char    *hex;
    char    *pf;
    size_t  id_len;
    size_t  hex_len;
    size_t  idq_len;
    size_t  hex_oset;

    /*
     * If there is no pffile, parsing is very, very fast.
     */
    if(!fp){
        return 0;
    }

    if(id_query){
        idq_len = strlen(id_query);
    }

    /*
     * Fetch each non-blank, non-comment line from the pffile.
     *
     * Completely validate each line and then determine at the
     * end of the loop if the caller is interested in this line or not.
     * (This strict interpretation of the syntax of the pffile should
     * help find errors as quickly as possible instead of letting them
     * hide until they actually bite someone.)
     *
     * Valid syntax is:
     * 
     */
    while((rc = parsefileline(eh,fp,rc,lbuf,lbuf_max,tofp)) > 0){

        size_t  used,needed;

        /*
         * line starts with id
         */
        line = *lbuf;

        /*
         * Find beginning of hex-pf
         */
        id_len = strcspn(line,I2WSPACESET);
        hex = line + id_len;

        /*
         * Must be at least one space separator
         */
        if(!isspace((int)*hex)){
            return -rc;
        }

        /*
         * If a specific "identity" is being searched for: skip/copy
         * lines that don't match and continue parsing the file.
         */
        if(id_query && strncmp(line,id_query,MIN(id_len,idq_len))){
            /*
             * Write line to tofp, then 'continue'
             */
            if(tofp) fprintf(tofp,"%s\n",*lbuf);
            continue;
        }

        /* Terminate id, then eat trailing white-space */
        *hex = '\0';
        hex++;
        while(isspace((int)*hex)){
            hex++;
        }
        hex_oset = hex - line;

        /* how long is the hex-pf */
        hex_len = strcspn(hex,I2WSPACESET);

        /*
         * eat trailing white-space - and terminate hex-pf
         */
        ts = hex + hex_len;
        while(isspace((int)*ts)){
            *ts = '\0';
            ts++;
        }

        /*
         * hex can not be nul, must be multiple of 2,
         * and must be the last thing on the line.
         */
        if(!hex_len || (hex_len % 2) || (*ts != '\0')){
            return -rc;
        }

        /*
         * Make sure there is enough room after full line in lbuf to hold
         * return pf. (ts currently points at the terminating nul
         * byte for the full line.)
         */
        used = (ts - *lbuf) + 1;
        needed = (hex_len / 2);

        /*
         * make sure lbuf is large enough for this content
         */
        if((used + needed) > *lbuf_max){
            while((used + needed) > *lbuf_max){
                *lbuf_max += I2LINEBUFINC;
            }
            *lbuf = realloc(line,sizeof(char) * *lbuf_max);
            if(!*lbuf){
                if(line){
                    free(line);
                }
                I2ErrLog(eh,"realloc(%u): %M",*lbuf_max);
                return -rc;
            }
            line = *lbuf;
        }

        /*
         * Terminate id, and point pf to beginning of binary pf
         * to be returned.
         */
        hex = line + hex_oset;
        pf = line + used;

        if(!I2HexDecode(hex,(uint8_t *)pf,hex_len/2)){
            I2ErrLogP(eh,EINVAL,"Invalid key: not hex?");
            return -rc;
        }

        *id_ret = line;
        *pf_ret = (uint8_t *)pf;
        *pf_len = hex_len/2;

        break;
    }

    return rc;
}

/*
 * Function:	I2WritePFLine
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
int
I2WritePFLine(
	I2ErrHandle	eh,
	FILE		*fp,
	const char	*id,        /* nul terminated */
	const uint8_t	*bytes,
        size_t          nbytes,
	char	        **lbuf,     /* memory for hex string */
	size_t	        *lbuf_max
	)
{
	int	ret;
        char    *line = *lbuf;

	if(!id || (id[0] == '\0')){
		I2ErrLogP(eh,EINVAL,"I2WriteKeyLine(): Invalid identity name");
		return -1;
	}

        /*
         * make sure lbuf is large enough for this content
         */
        if((nbytes*2 + 1) > *lbuf_max){
            while((nbytes*2 + 1) > *lbuf_max){
                *lbuf_max += I2LINEBUFINC;
            }
            *lbuf = realloc(line,sizeof(char) * *lbuf_max);
            if(!*lbuf){
                if(line){
                    free(line);
                }
                I2ErrLog(eh,"realloc(%u): %M",*lbuf_max);
                return -1;
            }
            line = *lbuf;
        }


	I2HexEncode(line,bytes,nbytes);

	/*
	 * if fprintf has an error, set ret < 0 for a failure return.
	 */
	ret = fprintf(fp,"%s\t%s\n",id,line);

	if(ret > 0) ret = 0;

	return ret;
}

/*
 * Function:	I2StrToNum
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
int
I2StrToNum(
		I2numT	*limnum,
		char	*limstr
		)
{
	size_t		silen=0;
	size_t		len;
	char		*endptr;
	I2numT		ret, mult=1;

	while(isdigit((int)limstr[silen])){
		silen++;
	}
	len = strlen(limstr);

	if(len != silen){
		/*
		 * Ensure that there is at most one non-digit and that it
		 * is the last char.
		 */
		if((len - silen) > 1){
			return -1;
		}

		switch (tolower(limstr[silen])){
		case 'e':
			mult *= 1000;	/* 1e18 */
		case 'p':
			mult *= 1000;	/* 1e15 */
		case 't':
			mult *= 1000;	/* 1e12 */
		case 'g':
			mult *= 1000;	/* 1e9 */
		case 'm':
			mult *= 1000;	/* 1e6 */
		case 'k':
			mult *= 1000;	/* 1e3 */
			break;
		default:
			return -1;
			/* UNREACHED */
		}
		limstr[silen] = '\0';
	}
	ret = strtoull(limstr, &endptr, 10);
	if(endptr != &limstr[silen]){
		return -1;
	}

	if(ret == 0){
		*limnum = 0;
		return 0;
	}

	/* Check for overflow. */
	*limnum = ret * mult;
	return (*limnum < ret || *limnum < mult)? (-1) : 0;
}

/*
 * Function:	I2StrToByte
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
int
I2Str2Byte(
		I2numT	*limnum,
		char	*limstr
		)
{
	size_t		silen=0;
	size_t		len;
	char		*endptr;
	I2numT		ret, mult=1;

	while(isdigit((int)limstr[silen])){
		silen++;
	}
	len = strlen(limstr);

	if(len != silen){
		/*
		 * Ensure that there is at most one non-digit and that it
		 * is the last char.
		 */
		if((len - silen) > 1){
			return -1;
		}

		switch (tolower(limstr[silen])){
		case 'e':
			mult <<= 10;
		case 'p':
			mult <<= 10;
		case 't':
			mult <<= 10;
		case 'g':
			mult <<= 10;
		case 'm':
			mult <<= 10;
		case 'k':
			mult <<= 10;
			break;
		default:
			return -1;
			/* UNREACHED */
		}
		limstr[silen] = '\0';
	}
	ret = strtoull(limstr, &endptr, 10);
	if(endptr != &limstr[silen]){
		return -1;
	}

	if(ret == 0){
		*limnum = 0;
		return 0;
	}

	/* Check for overflow. */
	*limnum = ret * mult;
	return (*limnum < ret || *limnum < mult)? (-1) : 0;
}
