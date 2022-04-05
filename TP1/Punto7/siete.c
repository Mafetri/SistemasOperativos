#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

void main(void){
	 int pid;
	 int x = 0;

	 pid = fork();

	 if (pid == 0){
		 printf("Proceso hijo %d\n", x++);
	 }else{ 
		 printf("Proceso padre. Mi hijo es el pid=%d \n", pid);
	 }
}
