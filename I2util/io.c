/*
**      $Id$
*/
/************************************************************************
*									*
*			     Copyright (C)  2002			*
*				Internet2				*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
**	File:		io.c
**
**	Author:		Anatoly Karp
**                      Jeff W. Boote
**
**	Date:		Wed Apr  24 10:42:12  2002
**
**	Description:	This file contains the private functions to
**			to facilitate IO that the library needs to do.
*/
#include <I2util/util.h>

#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>

/*
 * Function:	I2Readni
 *
 * Description:	
 * 	Read n bytes from the given fd. If !*retn_on_intr - then the function
 * 	will continue to read even through interupts. (This is set to a
 * 	pointer value so it can be modified via a signal handler. So, you
 * 	can make this function return based on signals you care about.)
 *
 * 	If this function returns due to an interrupt - *retn_on_intr will
 * 	be non-zero (although this function won't modify the value). And,
 * 	this function will return -1. errno will also be set to EINTR.
 *
 * 	This function will return -1 for all other error conditions
 * 	as well. (and errno will be set)
 *
 * 	Short reads returned indicate the fd has reached EOF. (i.e. the
 * 	socket is closed if fd is attached to a socket)
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
ssize_t
I2Readni(
	int	fd,
	void	*vptr,
	size_t	n,
	int	*retn_on_intr	/* pointer so it can be modified via signals */
	)
{
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	ptr = vptr;
	nleft = n;
	while(nleft > 0){
		if((nread = read(fd, ptr, nleft)) < 0){
			if((errno == EINTR) && !*retn_on_intr){
				/* call read again */
				nread = 0;
			}
			else{
				return(-1);
			}
		} else if(nread == 0){
			/* EOF */
			break;
		}

		nleft -= nread;
		ptr   += nread;
	}
	return(n - nleft);		/* return >= 0 */
}

/*
 * Function:	I2Readn
 *
 * Description:	
 * 	Wrapper function for I2Readni if you really don't ever want to
 * 	return early from read due to an interrupt.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
ssize_t
I2Readn(
	int	fd,
	void	*vptr,
	size_t	n
	)
{
	int	intr=0;

	return I2Readni(fd,vptr,n,&intr);
}

/*
 * Function:	I2Writeni
 *
 * Description:	
 * 	Write n bytes to the given fd. If !*retn_on_intr - then the function
 * 	will continue to write even through interupts. (This is set to a
 * 	pointer value so it can be modified via a signal handler. So, you
 * 	can make this function return based on signals you care about.)
 *
 * 	If this function returns due to an interrupt - *retn_on_intr will
 * 	be non-zero (although this function won't modify the value). And,
 * 	this function will return -1. errno will also be set to EINTR.
 *
 * 	This function will return -1 for all other error conditions
 * 	as well. (and errno will be set)
 *
 * 	Short reads returned indicate the fd has reached EOF. (i.e. the
 * 	socket is closed if fd is attached to a socket)
 *
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
ssize_t
I2Writeni(
	int		fd,
	const void	*vptr,
	size_t		n,
	int		*retn_on_intr
	)
{
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;

	ptr = vptr;
	nleft = n;
	while(nleft > 0){
		if((nwritten = write(fd, ptr, nleft)) <= 0){
			if((errno == EINTR) && !*retn_on_intr){
				/* call write again */
				nwritten = 0;
			}
			else{
				return(-1);
			}
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}

	return(n);
}

/*
 * Function:	I2Writen
 *
 * Description:	
 * 	Wrapper function for I2Writeni if you really don't ever want to
 * 	return early from write due to an interrupt.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
ssize_t
I2Writen(
	int		fd,
	const void	*vptr,
	size_t		n
	)
{
	int	intr=0;

	return I2Writeni(fd,vptr,n,&intr);
}

/*
 * Function:    I2CopyFile
 *
 * Description:    
 *              Copy one file to another using mmap for speed.
 *
 * In Args:    
 *
 * Out Args:    
 *
 * Scope:    
 * Returns:    
 *              0 on success
 * Side Effect:    
 */
int
I2CopyFile(
        I2ErrHandle eh,
        int         tofd,
        int         fromfd,
        off_t       len
        )
{
    struct stat sbuf;
    int         rc;
    void        *fptr,*tptr;

    if((rc = fstat(fromfd,&sbuf)) != 0){
        I2ErrLog(eh,"I2CopyFile: fstat: %M, status of from file");
        return rc;
    }

    if(len == 0){
        len = sbuf.st_size;
    }
    else{
        len = MIN(len,sbuf.st_size);
    }

    if((rc = ftruncate(tofd,len)) != 0){
        I2ErrLog(eh,"I2CopyFile: ftrunctate(%llu): %M, sizing to file",len);
        return rc;
    }

    if(!(fptr = mmap(NULL,len,PROT_READ|PROT_WRITE,MAP_SHARED,fromfd,0))){
        I2ErrLog(eh,"I2CopyFile: mmap(from file): %M");
        return -1;
    }

    if(!(tptr = mmap(NULL,len,PROT_READ|PROT_WRITE,MAP_SHARED,tofd,0))){
        I2ErrLog(eh,"I2CopyFile: mmap(to file): %M");
        return -1;
    }

    memcpy(tptr,fptr,len);

    if((rc = munmap(fptr,len)) != 0){
        I2ErrLog(eh,"I2CopyFile: munmap(from file): %M");
        return -1;
    }

    if((rc = munmap(tptr,len)) != 0){
        I2ErrLog(eh,"I2CopyFile: munmap(to file): %M");
        return -1;
    }

    return 0;
}
