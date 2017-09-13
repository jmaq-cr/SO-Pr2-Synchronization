//
// Created by eros on 21/05/17.
//

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

//
// Esta union hay que definirla o no según el valor de los defines aqui
// indicados.
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

void main()
{
    key_t Clave;
    int Id_Memoria;
    int *Memoria = NULL;
    int i,j;

    key_t ClaveSem;
    int Id_Semaforo;
    struct sembuf Operacion;
    union semun arg;

    //
    //  Igual que en p1.cc, obtenemos una clave para la memoria compartida
    //
    Clave = ftok ("/bin/ls", 33);
    if (Clave == -1)
    {
        printf("No consigo clave para la memoria compartida\n");
        exit(0);
    }

    //
    //  Igual que en p1.cc, obtenemos el id de la memoria. Al no poner
    //  el flag IPC_CREAT, estamos suponiendo que dicha memoria ya está
    //  creada.
    //
    Id_Memoria = shmget (Clave, sizeof(int)*100, 0777 );
    if (Id_Memoria == -1)
    {
        printf("No consigo Id para la memoria compartida\n");
        exit (0);
    }

    //
    //  Igual que en p1.cc, obtenemos un puntero a la memoria compartida
    //
    Memoria = (int *)shmat (Id_Memoria, (char *)0, 0);
    if (Memoria == NULL)
    {
        printf("No consigo memoria compartida\n");
        exit (0);
    }

    //
    // Igual que en cualquier recurso compartido (memoria compartida, semaforos
    // o colas) se obtien una clave a partir de un fichero existente cualquiera
    // y de un entero cualquiera. Todos los procesos que quieran compartir este
    // semaforo, deben usar el mismo fichero y el mismo entero.
    //
    ClaveSem = ftok ("/bin/ls", 33);
    if (ClaveSem == (key_t)-1)
    {
        printf("No puedo conseguir clave de Semaforo\n");
        exit(0);
    }

    //
    // Se obtiene un array de semaforos (10 en este caso, aunque solo se usara
    // uno.
    // El IPC_CREAT indica que lo  cree si no lo está ya
    // el 0600 con permisos de lectura y escritura para el usuario que lance
    // los procesos. Es importante el 0 delante para que se interprete en
    // octal.
    //
    Id_Semaforo = semget (ClaveSem, 10, 0600 | IPC_CREAT);
    if (Id_Semaforo == -1)
    {
        printf("No puedo crear Semaforo\n");
        exit (0);
    }

    //
    //  Se inicializa el semáforo con un valor conocido. Si lo ponemos a 0,
    //  es semáforo estará "rojo". Si lo ponemos a 1, estará "verde".
    //  El 0 de la función semctl es el índice del semáforo que queremos
    //  inicializar dentro del array de 10 que hemos pedido.
    //
    arg.val = 0;
    semctl (Id_Semaforo, 0, SETVAL, &arg);

    //
    //  Para "pasar" por el semáforo parándonos si está "rojo", debemos rellenar
    //  esta estructura.
    //  sem_num es el indice del semáforo en el array por el que queremos "pasar"
    //  sem_op es -1 para hacer que el proceso espere al semáforo.
    //  sem_flg son flags de operación. De momento nos vale un 0.
    //
    Operacion.sem_num = 0;
    Operacion.sem_op = -1;
    Operacion.sem_flg = 0;

    //
    //  Vamos leyendo el valor de la memoria con esperas de un segundo
    //  y mostramos en pantalla dicho valor. Debería ir cambiando según
    //  p1 lo va modificando.
    //
    while(i<5)
    {
        printf("%d Esperando Semaforo\n", i);
        semop (Id_Semaforo, &Operacion, 1);
        printf("Semaforo en Verde\n");

        printf("Leido %d\n", Memoria[i]);

        printf("Salgo de Semaforo\n");
        printf("\n");

        i++;

        sleep (1);
    }

    if (Id_Memoria != -1)
    {
        shmdt ((char *)Memoria);
    }
}