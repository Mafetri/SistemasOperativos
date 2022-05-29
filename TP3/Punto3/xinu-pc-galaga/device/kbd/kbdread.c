/* kbdread.c  -  kbdread */

#include <xinu.h>
/*------------------------------------------------------------------------
 * kbdread  -  Read the status of the keyboard driver
 *------------------------------------------------------------------------
 */
devcall	kbdread (
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char          *buffer,        /* Address of buffer            */
      uint32        count           /* Length of buffer             */
	)
{	

	if(Kb_data.pid_mutex == getpid()){
		// To simplify only puts one character from kbgetc into the user buffer
		buffer[0] = kbgetc();
	}else{
		return SYSERR;
	}
		
    return OK;
}
