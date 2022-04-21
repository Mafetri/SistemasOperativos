#include <xinu.h>

void pA(void), pB(void), pC(void);

unsigned int iA, iB, iC;

void planificador(void){
	int pidA, msA, pidB, msB, pidC, msC, pid, prioA, prioB, prioC, msTotales, iTotal;

	msTotales = 200;
	msA = msTotales*60/100;
	msB = msTotales*30/100;
	msC = msTotales*10/100;

	iA = 0;
	iB = 0;
	iC = 0;

	pid = getpid();
	chprio(pid, 30);

	pidA = create(pA, 1024, 20, "Proceso A", 0);
	pidB = create(pB, 1024, 20, "Proceso B", 0);
	pidC = create(pC, 1024, 20, "Proceso C", 0);
	resume(pidA);
	resume(pidB);
	resume(pidC);
	
	// Proceso A	
	prioA = getprio(pidA);
	chprio(pidA, getprio(pid)-1);
	sleepms(msA);
	chprio(pidA, prioA);
	printf("Proceso A interrumpido: %d \n", iA);

	// Proceso B
	prioB = getprio(pidB);
	chprio(pidB, getprio(pid)-1);
	sleepms(msB);
	chprio(pidB, prioB);
	printf("Proceso B interrumpido: %d \n", iB);

	// Proceso C
	prioC = getprio(pidC);
	chprio(pidC, getprio(pid)-1);
	sleepms(msC);
	chprio(pidC, prioC);
	printf("Proceso C interrumpido: %d \n", iC);

	kill(pidA);
	kill(pidB);
	kill(pidC);
		
	iTotal = iA + iB + iC;
	printf("El total de los procesos es: %d \n", iTotal);
	printf("Proceso A realizo el: %d \n", iA*100/iTotal);
	printf("Proceso B realizo el: %d \n", iB*100/iTotal);
	printf("Proceso C realizo el: %d \n", iC*100/iTotal);

}

void pA(void){
	while(1){
		iA++;
	}
}

void pB(void){
	while(1){
		iB++;
	}
}

void pC(void){
	while(1){
		iC++;
	}
}
