#include <xinu.h>

void operar(void), incrementar(void), mutex_lock(void), mutex_unlock();

unsigned char x = 0;

sid32 sem;

void tres(void){
	int i;

	mutex_init();
	resume(create(operar, 1024, 20, "Process 1", 0));
	resume(create(incrementar, 1024, 20, "Process 2", 0));

	sleep(10);
}

void operar(void){
	int y = 0;

	printf("Si no existen mensajes de ERROR entonces todo va Ok! \n");

	while(1){
		mutex_lock();

		if((x % 10) == 0){
			y = x * 2;
			if((y % 10) != 0){
				printf("\r ERROR!! y=%d, x=%d \r", y , x);
			}
		}

		mutex_unlock();
	}
}

void incrementar(void){
	while(1){
		mutex_lock();
		x = x + 1;
		mutex_unlock();
	}
}

void mutex_init(void){
	sem = semcreate(1);
}

void mutex_lock(void){
	wait(sem);
}

void mutex_unlock(void){
	signal(sem);
}
