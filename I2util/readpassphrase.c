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
 *	File:		readpassphrase.c
 *
 *	Author:		Jeff Boote
 *			Internet2
 *
 *	Date:		Wed Dec 10 15:29:10 MST 2003
 *
 *	Description:	
 *		The Internet2 copyright only applies to changes made
 *		to this code. The original copyright follows.
 *
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
/*
 * Copyright (c) 2000 Todd C. Miller <Todd.Miller@courtesan.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <I2util/readpassphrase.h>
#include <I2util/conf.h>

#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#ifdef HAVE_PATHS_H
#include <paths.h>
#endif

#ifndef _PATH_TTY
#define _PATH_TTY	"/dev/tty"
#endif


#ifndef	TCSASOFT
#define	TCSASOFT	0
#endif

static volatile sig_atomic_t signo;

static void handler(int);

char *
I2ReadPassPhrase(const char *prompt, char *buf, size_t bufsiz, int flags)
{
	ssize_t nr;
	ssize_t nw __attribute__((unused));
	int input, output, save_errno;
	char ch, *p, *end;
	struct termios term, oterm;
	struct sigaction sa, saveint, savehup, savequit, saveterm;
	struct sigaction savetstp, savettin, savettou;

	/* I suppose we could alloc on demand in this case (XXX). */
	if (bufsiz == 0) {
		errno = EINVAL;
		return(NULL);
	}

restart:
	/*
	 * Read and write to /dev/tty if available.  If not, read from
	 * stdin and write to stderr unless a tty is required.
	 */
	if ((input = output = open(_PATH_TTY, O_RDWR)) == -1) {
		if (flags & I2RPP_REQUIRE_TTY) {
			errno = ENOTTY;
			return(NULL);
		}
		input = STDIN_FILENO;
		output = STDERR_FILENO;
	}

	/*
	 * Catch signals that would otherwise cause the user to end
	 * up with echo turned off in the shell.  Don't worry about
	 * things like SIGALRM and SIGPIPE for now.
	 */
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;		/* don't restart system calls */
	sa.sa_handler = handler;
	(void)sigaction(SIGINT, &sa, &saveint);
	(void)sigaction(SIGHUP, &sa, &savehup);
	(void)sigaction(SIGQUIT, &sa, &savequit);
	(void)sigaction(SIGTERM, &sa, &saveterm);
	(void)sigaction(SIGTSTP, &sa, &savetstp);
	(void)sigaction(SIGTTIN, &sa, &savettin);
	(void)sigaction(SIGTTOU, &sa, &savettou);

	/* Turn off echo if possible. */
	if (tcgetattr(input, &oterm) == 0) {
		memcpy(&term, &oterm, sizeof(term));
		if (!(flags & I2RPP_ECHO_ON))
			term.c_lflag &= ~(ECHO | ECHONL);
#ifdef	VSTATUS
		if (term.c_cc[VSTATUS] != _POSIX_VDISABLE)
			term.c_cc[VSTATUS] = _POSIX_VDISABLE;
#endif
		(void)tcsetattr(input, TCSAFLUSH|TCSASOFT, &term);
	} else {
		memset(&term, 0, sizeof(term));
		memset(&oterm, 0, sizeof(oterm));
	}

	nw = write(output, prompt, strlen(prompt));
	end = buf + bufsiz - 1;
	for (p = buf; (nr = read(input, &ch, 1)) == 1 && ch != '\n' && ch != '\r';) {
		if (p < end) {
			if ((flags & I2RPP_SEVENBIT))
				ch &= 0x7f;
			if (isalpha((int)ch)) {
				if ((flags & I2RPP_FORCELOWER))
					ch = tolower(ch);
				if ((flags & I2RPP_FORCEUPPER))
					ch = toupper(ch);
			}
			*p++ = ch;
		}
	}
	*p = '\0';
	save_errno = errno;
	if (!(term.c_lflag & ECHO))
		nw = write(output, "\n", 1);

	/* Restore old terminal settings and signals. */
	if (memcmp(&term, &oterm, sizeof(term)) != 0)
		(void)tcsetattr(input, TCSANOW|TCSASOFT, &oterm);
	(void)sigaction(SIGINT, &saveint, NULL);
	(void)sigaction(SIGHUP, &savehup, NULL);
	(void)sigaction(SIGQUIT, &savequit, NULL);
	(void)sigaction(SIGTERM, &saveterm, NULL);
	(void)sigaction(SIGTSTP, &savetstp, NULL);
	(void)sigaction(SIGTTIN, &savettin, NULL);
	(void)sigaction(SIGTTOU, &savettou, NULL);
	if (input != STDIN_FILENO)
		(void)close(input);

	/*
	 * If we were interrupted by a signal, resend it to ourselves
	 * now that we have restored the signal handlers.
	 */
	if (signo) {
		kill(getpid(), signo); 
		switch (signo) {
		case SIGTSTP:
		case SIGTTIN:
		case SIGTTOU:
			signo = 0;
			goto restart;
		}
	}

	errno = save_errno;
	return(nr == -1 ? NULL : buf);
}

char *
I2ReadPassPhraseAlloc(
        const char  *prompt,
        int         flags,
        char        **lbuf,
        size_t      *lbuf_max
        )
{
    ssize_t nr;
    ssize_t nw __attribute__((unused));
    size_t  pf_len = 0;
    int input, output, save_errno;
    char *line;
    char ch, *p;
    struct termios term, oterm;
    struct sigaction sa, saveint, savehup, savequit, saveterm;
    struct sigaction savetstp, savettin, savettou;

restart:
    /*
     * Read and write to /dev/tty if available.  If not, read from
     * stdin and write to stderr unless a tty is required.
     */
    if ((input = output = open(_PATH_TTY, O_RDWR)) == -1) {
        if (flags & I2RPP_REQUIRE_TTY) {
            errno = ENOTTY;
            return(NULL);
        }
        input = STDIN_FILENO;
        output = STDERR_FILENO;
    }

    /*
     * Catch signals that would otherwise cause the user to end
     * up with echo turned off in the shell.  Don't worry about
     * things like SIGALRM and SIGPIPE for now.
     */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;		/* don't restart system calls */
    sa.sa_handler = handler;
    (void)sigaction(SIGINT, &sa, &saveint);
    (void)sigaction(SIGHUP, &sa, &savehup);
    (void)sigaction(SIGQUIT, &sa, &savequit);
    (void)sigaction(SIGTERM, &sa, &saveterm);
    (void)sigaction(SIGTSTP, &sa, &savetstp);
    (void)sigaction(SIGTTIN, &sa, &savettin);
    (void)sigaction(SIGTTOU, &sa, &savettou);

    /* Turn off echo if possible. */
    if (tcgetattr(input, &oterm) == 0) {
        memcpy(&term, &oterm, sizeof(term));
        if (!(flags & I2RPP_ECHO_ON))
            term.c_lflag &= ~(ECHO | ECHONL);
#ifdef	VSTATUS
        if (term.c_cc[VSTATUS] != _POSIX_VDISABLE)
            term.c_cc[VSTATUS] = _POSIX_VDISABLE;
#endif
        (void)tcsetattr(input, TCSAFLUSH|TCSASOFT, &term);
    } else {
        memset(&term, 0, sizeof(term));
        memset(&oterm, 0, sizeof(oterm));
    }

    /*
     * Write prompt
     */
    nw = write(output, prompt, strlen(prompt));

    /*
     * Read pass-phrase
     */
    p = line = *lbuf;
    while(((nr = read(input, &ch, 1)) == 1)&&(ch != '\n')&&(ch != '\r')){

        /*
         * alloc more memory if required for content up to this point
         * plus this character
         */
        if((pf_len+1) > *lbuf_max){
            while((pf_len+1) > *lbuf_max){
                *lbuf_max += I2LINEBUFINC;
            }
            *lbuf = realloc(line,sizeof(char) * *lbuf_max);
            if(!*lbuf){
                if(line){
                    free(line);
                }
                return NULL;
            }
            line = *lbuf;
            p = line + pf_len;
        }

        /* modify read char if needed */
        if ((flags & I2RPP_SEVENBIT))
            ch &= 0x7f;
        if (isalpha((int)ch)) {
            if ((flags & I2RPP_FORCELOWER))
                ch = tolower(ch);
            if ((flags & I2RPP_FORCEUPPER))
                ch = toupper(ch);
        }

        /* save the char */
        *p++ = ch;
        pf_len++;
    }

    /* alloc more memory if required for nul terminator character */
    if((pf_len+1) > *lbuf_max){
        while((pf_len+1) > *lbuf_max){
            *lbuf_max += I2LINEBUFINC;
        }
        *lbuf = realloc(line,sizeof(char) * *lbuf_max);
        if(!*lbuf){
            if(line){
                free(line);
            }
            return NULL;
        }
        line = *lbuf;
        p = line + pf_len;
    }

    /* terminate pf */
    *p = '\0';

    save_errno = errno;
    if (!(term.c_lflag & ECHO))
        nw = write(output, "\n", 1);

    /* Restore old terminal settings and signals. */
    if (memcmp(&term, &oterm, sizeof(term)) != 0)
        (void)tcsetattr(input, TCSANOW|TCSASOFT, &oterm);
    (void)sigaction(SIGINT, &saveint, NULL);
    (void)sigaction(SIGHUP, &savehup, NULL);
    (void)sigaction(SIGQUIT, &savequit, NULL);
    (void)sigaction(SIGTERM, &saveterm, NULL);
    (void)sigaction(SIGTSTP, &savetstp, NULL);
    (void)sigaction(SIGTTIN, &savettin, NULL);
    (void)sigaction(SIGTTOU, &savettou, NULL);
    if (input != STDIN_FILENO)
        (void)close(input);

    /*
     * If we were interrupted by a signal, resend it to ourselves
     * now that we have restored the signal handlers.
     */
    if (signo) {
        kill(getpid(), signo); 
        switch (signo) {
            case SIGTSTP:
            case SIGTTIN:
            case SIGTTOU:
                signo = 0;
                goto restart;
        }
    }

    errno = save_errno;
    return(nr == -1 ? NULL : *lbuf);
}

static void handler(int s)
{

	signo = s;
}
