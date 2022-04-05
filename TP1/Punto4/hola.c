#include <xinu.h>

void	sndA(void), sndB(void), sndch(char);

void	hola(void)
{
	int pidA, pidB;
	printf("Hola mundo! \n");

	pidA = create(sndA, 128, 20, "process 1", 0);
        pidB = create(sndB, 128, 20, "process 2", 0);	

	resume( create(sndch, 1024, 20, "send A", 1, 'A') );
	resume( create(sndch, 1024, 20, "send B", 1, 'B') );	
	resume(pidA);	
	resume(pidB);

	sleep(10);

	kill(pidA);
	kill(pidB);
}

/*------------------------------------------------------------------------
 * sndA  --  repeatedly emit 'A' on the console without terminating
 *------------------------------------------------------------------------
 */
void	sndA(void)
{
	while( 1 ){
		putc(CONSOLE, 'A');
		sleep(1.0);
	}
}

/*------------------------------------------------------------------------
 * sndB  --  repeatedly emit 'B' on the console without terminating
 *------------------------------------------------------------------------
 */
void	sndB(void)
{
	while( 1 ){
		putc(CONSOLE, 'B');
		sleep(1.0);
	}
}


/*------------------------------------------------------------------------
 * sndch  --  repeatedly emit the sent char on the console without terminating
 *------------------------------------------------------------------------
 */
void sndch(char ch){
	while(1){
		putc(CONSOLE, ch);
	}
}
