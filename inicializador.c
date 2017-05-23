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

#include <sys/sem.h>
#include <unistd.h>

static sem_t *semaforoEscritura;
static sem_t *semaforoLectura;

int crearZonaMemoriaCompartida(key_t clave, int tamano){
    idMemoria = shmget (clave, tamano, 0777 | IPC_CREAT);
    if (idMemoria == -1)
    {
        printf("No consigo Id para la memoria compartida");
        return -1;
    }
    return IdMemoria;
}

int *apuntarZonaMemoriaCompartida(key_t clave, int tamano){
    int *punteroMemoria;
    idMemoria = crearZonaMemoriaCompartida(clave, tamano);

    if (Memoria == -1)
    {
        printf("No consigo la memoria compartida");
        return -1;
    } else {
        if ((punteroMemoria = shmat(idMemoria, (char *)0, 0)) == -1){
            return NULL;
        } else{
            return punteroMemoria;
        }
    }
}

int main(int argc, char *argb[]){
    int lineas;

    semaforoEscritura = sem_open("/semaforoEscritura", O_CREAT, 0777, 1);
    semaforoLectura = sem_open("/semaforoLectura", O_CREAT, 0777, 1);

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

    printf("Espacios de Memoria Creados";)
    exit(0);


}

