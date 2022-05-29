/* kbdclose.c  -  kbdclose */

#include <xinu.h>
struct	kbdata	Kb_data;

/*------------------------------------------------------------------------
 * kbdclose  -  Close the keyboard device
 *------------------------------------------------------------------------
 */
devcall	kbdclose (
	  struct dentry	*devptr		/* Entry in device switch table	*/
	)
{
	if(Kb_data.pid_mutex == getpid()){
		Kb_data.pid_mutex = 0;
		signal(Kb_data.kb_mutex);
	}
}
