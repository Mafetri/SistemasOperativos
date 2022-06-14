#include <stdio.h>
#include <stdlib.h>
#define N 240000
#define BSIZE 4096
#define SMALL 4
char *pp;

main (){
    int i, j, k;
    pp = malloc(N*BSIZE);
    printf("Direcciones de: \n -> Main: 0x%llx \n -> i: 0x%llx \n -> j: 0x%llx \n PID: %d \n", &main, &i, &j, getpid());
    if (pp == NULL) {
        printf("Error al reservar memoria.\n");
        exit(1);
    }
    /* RECORREMOS y modificamos todo el segmento solicitado */  
    for (j=0; j<BSIZE; j++) {
        for (i=0; i<N; i++) {
        *(pp+i*BSIZE+j) = 2; // pp[i][j] = 2;
        }
    }
    /* RECORREMOS y VERIFICAMOS QUE HEMOS MODIFICADO todo el segmento solicitado */
    for (i=0; i<N; i++) {   
        for (j=0; j<BSIZE; j++) {
            if (*(pp+i*BSIZE+j) != 2) { // if (pp[i][j] != 2)
                printf("ERROR! \n");
                exit(1);
            }
        }
    }
    printf("OK \n");
}
