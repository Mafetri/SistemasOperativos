#include <xinu.h>
#include <stdio.h>
#include <stdlib.h>

int posDeLetra(char palabra[], char letra){
	int pos = -1;
	for(int i = 0; i < sizeof(palabra); i++){
		if(palabra[i] == letra){
			pos = i;
		}
	}
	return pos;
}

void ahorcado(){
	int c = '-';
	int errados = 4;
	int aciertos = 0;
	int posC = -1;
	char palabra[4] = {'L', 'U', 'N', 'A'};
	char letrasAcertadas[10] = "xxxx";

	control(0, TC_MODER, 0, 0);

	while(errados > 0 && aciertos < sizeof(palabra)){
		printf("\r Palabra: %s , intentos restantes: %d \n",letrasAcertadas, errados);
		printf("\r Ingrese una letra (0 para salir): %c",c);
		c = getchar();

		posC = posDeLetra(palabra, c);
		if(posC != -1){
			aciertos = aciertos + 1;
			letrasAcertadas[posC] = c;
		}else{
			errados = errados - 1;
		}

		if(c == '0'){
			break;
		}
	}

	printf("\r                                           ");

	if(aciertos == sizeof(palabra)){
		printf("\r ============== GANO ============= \n \r");
	}else{
		printf("\r ============ PERDIO ============== \n \r");
	}
}