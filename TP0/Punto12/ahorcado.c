#include <stdio.h>
#include <stdlib.h>

int main(){
	int c = '-';
	int errados = 0;
	int aciertos = 0;
	char palabra[4] = {'L', 'U', 'N', 'A'};

	system("/bin/stty raw");

	while(errados < 4 && aciertos < sizeof(palabra)){
		printf("\r                       ");
		printf("\r c = %c ingrese una letra (0 para salir): ", c);
		c = getchar();

		if(c == palabra[aciertos]){
			aciertos = aciertos + 1;
		}else{
			errados = errados + 1;
		}

		if(c == '0'){
			break;
		}
	}

	printf("\r                                            ");

	if(aciertos == sizeof(palabra)){
		printf("\r ====== GANÓ ====== ");
	}else{
		printf("\r ====== PERDIÓ ====== ");
	}

}
