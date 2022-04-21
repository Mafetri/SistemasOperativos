#include <xinu.h>

void produce(void), consume(void);

int32 n=0;
sid32 semCon, semPro;

void dos(void){
	semCon = semcreate(1);
	semPro = semcreate(0);
	resume(create(consume, 1024, 20, "cons", 0));
	resume(create(produce, 1024, 20, "prod", 0));
}

void produce(void){
	int32 i;

	for(i=1; i <= 2000; i++){
		wait(semPro);
		n++;
		signal(semCon);
	}
}

void consume(void){
	int32 i;

	for(i=1; i <= 2000; i++){
		wait(semCon);
		printf("The value of n is %d \n", n);
		signal(semPro);
	}
}
