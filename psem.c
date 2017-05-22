//
// Created by eros on 21/05/17.
//
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

//
//  Esta union hay que definirla o no según el valor de los defines aqui
//  indicados.
//
#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
// La union ya está definida en sys/sem.h
#else
// Tenemos que definir la union
union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
    struct seminfo *__buf;
};
#endif

int main()
{
    key_t Clave; // CLave comun para todos los programas que quieran compartir memoria
    int Id_Memoria; // Identificador para la zona de memoria que se va a utilizar
    int *Memoria = NULL; // Puntero a memoria
    int i,j;

    key_t ClaveSem;
    int Id_Semaforo;
    struct sembuf Operacion;
    union semun arg;

    //  Se consigue una clave para la memoria compartida. Todos los procesos que quieran compartir la memoria,
    //  deben obtener la misma clave.
    //  Funcion ftok
    //  Recibe: Un fichero existente y accesible (los procesos comparten el mismo fichero), Un entero cualquiera
    //  Retorna: Clave para la memoria compartida.
    Clave = ftok ("/bin/ls", 33);
    if (Clave == -1)
    {
        printf("No consigo clave para la memoria compartida");
        exit(0);
    }

    //  Se crea la zona de memoria con la clave obtenida.
    //  Funcion shmget
    //  Recibe: Clave, tamano de memoria a reservar y banderas
    //  Retorna: Identificador para la memoria recién creada.
    //  0777 son  los permisos de lectura/escritura/ejecucion
    //  para propietario, grupo y otros y IPC_CREAT es para indicar que cree la memoria.
    Id_Memoria = shmget (Clave, sizeof(int)*100, 0777 | IPC_CREAT);
    if (Id_Memoria == -1)
    {
        printf("No consigo Id para la memoria compartida");
        exit (0);
    }

    //  El puntero anteriormente creado ahora apunta a la zona de memoria creada
    //  Funcion shmat
    //  Recibe: identificador de zona de memoria y un par de parámetros extraños, que son puros 0 en realidad.
    Memoria = (int *)shmat (Id_Memoria, (char *)0, 0);
    if (Memoria == NULL)
    {
        printf("No consigo la memoria compartida");
        exit (0);
    }

    //
    //  Igual que en cualquier recurso compartido (memoria compartida, semaforos
    //  o colas) se obtien una clave a partir de un fichero existente cualquiera
    //  y de un entero cualquiera. Todos los procesos que quieran compartir este
    // semaforo, deben usar el mismo fichero y el mismo entero.
    //
    ClaveSem = ftok ("/bin/ls", 33);
    if (ClaveSem == (key_t)-1)
    {
        printf("No puedo conseguir clave de Semaforo\n");
        exit(0);
    }

    //
    //  Se obtiene un array de semaforos (10 en este caso, aunque solo se usara
    //  uno.
    //  El IPC_CREAT indica que lo  cree si no lo está ya
    //  el 0600 con permisos de lectura y escritura para el usuario que lance
    //  los procesos. Es importante el 0 delante para que se interprete en
    //  octal.
    //
    Id_Semaforo = semget (ClaveSem, 10, 0600 | IPC_CREAT);
    if (Id_Semaforo == -1)
    {
        printf("No puedo crear Semaforo\n");
        exit (0);
    }

    //
    //  Se levanta el semáforo. Para ello se prepara una estructura en la que
    //  sem_num indica el indice del semaforo que queremos levantar en el array
    //  de semaforos obtenido.
    //  El 1 indica que se levanta el semaforo
    //  El sem_flg son banderas para operaciones raras. Con un 0 vale.
    //
    Operacion.sem_num = 0;
    Operacion.sem_op = 1;
    Operacion.sem_flg = 0;

    //
    //  Ya podemos utilizar la memoria.
    //  Escribimos cosas en la memoria. Los números de 1 a 10 esperando
    //  un segundo entre ellos. Estos datos serán los que lea el otro
    //  proceso.
    //
    void crearProceso(int lineas, int tiempo) {
        for (i = 0; i < lineas; i++) {
            printf("Levanto Semaforo\n");
            printf("Semaforo en Rojo\n");
            semop(Id_Semaforo, &Operacion, 1);

            printf("Escrito %d\n", i);
            Memoria[i] = i;

            sleep(tiempo);
        }

        // Liberar memoria
        shmdt((char *) Memoria);
        shmctl(Id_Memoria, IPC_RMID, (struct shmid_ds *) NULL);
    }

    crearProceso(5, 3);

}