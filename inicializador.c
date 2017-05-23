//
// Created by eros on 22/05/17.
//

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>



static sem_t *semaforoEscritura;
static sem_t *semaforoLectura;

int crearZonaMemoriaCompartida(key_t clave, int tamano){
    int idMemoria;
    idMemoria = shmget(clave, tamano, 0777 | IPC_CREAT);

    if (idMemoria == -1){
        printf("No consigo Id para la memoria compartida");
        return -1;
    }
    printf("Creado Id para la memoria compartida\n");
    return idMemoria;
}

int *apuntarZonaMemoriaCompartida(key_t clave, int tamano){
    int *punteroMemoria;
    int idMemoria;
    idMemoria = crearZonaMemoriaCompartida(clave, tamano);

    if (idMemoria == -1)
    {
        printf("No consigo la memoria compartida");
        return NULL;
    } else {
        if ((punteroMemoria = shmat(idMemoria, NULL, 0)) == (int *) -1){
            return NULL;
        } else{
            printf("Creado Memoria compartida y puntero\n");
            return punteroMemoria;
        }
    }
}

int main(int argc, char *argb[]){
    int lineas;

    semaforoEscritura = sem_open("/semaforoEscritura", O_CREAT, 0644, 1);
    semaforoLectura = sem_open("/semaforoLectura", O_CREAT, 0644, 1);

    // Escribir Lineas

    //Llaves para Acceso de Memoria Compartida
    key_t llave = 5678;

    // Puntero
    int *llaveMemoria, *handler;

    //Memoria para Lineas
    llaveMemoria = apuntarZonaMemoriaCompartida(llave, sizeof(int));

    if(llaveMemoria == NULL){
        printf("No consigo la memoria compartida para las lineas");
        exit(0);
    } else{
        handler = llaveMemoria;
        *handler = lineas;
    }

    printf("Espacios de Memoria Creados\n");
    exit(0);


}

