#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
    const int SIZE = 4096;
    const char *name = "Punto1";
    int shm_fd;
    void *ptr;

    // Creacion y tamaño del espacio
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);            
    ftruncate(shm_fd,SIZE);

    // Mapeo de memoria
    ptr = mmap(0,SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        printf("Map failed\n");
        return -1;
    }

    // Apertura y lectura del archivo
    int archivo = open("/usr/share/doc/util-linux/source-code-management.txt", O_RDONLY);
    read(archivo, ptr, 894);

}