/* kbdopen.c  -  kbdopen */

#include <xinu.h>
/*------------------------------------------------------------------------
 * kbdopen  -  Open the ps/2 keyboard device
 *------------------------------------------------------------------------
 */

devcall	kbdopen (struct       dentry  *devptr, char   *name, char     *mode)
{
	// Waits for keyboard semaphore
	wait(Kb_data.kb_mutex);

	Kb_data.pid_mutex = getpid();

}
