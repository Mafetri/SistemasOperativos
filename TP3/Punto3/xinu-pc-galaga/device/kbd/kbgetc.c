#include <xinu.h>
/*------------------------------------------------------------------------
 * kbgetc  -  Returns the head of the buffer
 *------------------------------------------------------------------------
 */

devcall kbgetc(void){
    unsigned char c;

    if(Kb_data.pid_mutex == getpid()){
        // Waits for inputs into the buffer
        wait(Kb_data.kbbuffsem);

        // Gets the head of the buffer char and pull it out
        c = Kb_data.kbbuff[Kb_data.kbhead];
        Kb_data.kbbuff[Kb_data.kbhead] = 0;
        Kb_data.kbhead = (Kb_data.kbhead + 1) % KEYBOARD_BUFFER_LENGTH;
    }else{
        return SYSERR;
    }

    return c;
}