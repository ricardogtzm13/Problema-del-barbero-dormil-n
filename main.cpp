#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

// El número máximo de subprocesos de clientes
#define MAX_CUSTOMERS 25

// Prototipos de funciones…
void *customer(void *num);
void *barber(void *);

void randwait(int secs);

// Define los semaforos

// waitingRoom Limita el número de clientes permitidos
// para entrar en la sala de espera a la vez
sem_t waitingRoom;

// barberChair garantiza acceso mutuamente exclusivo a la silla de barbero
sem_t barberChair;

// barberPillow se utiliza para permitir que el peluquero duerma hasta que llegue un cliente
sem_t barberPillow;

// seatBelt se utiliza para hacer que el cliente espere hasta que el barbero termine de cortarle el cabello
sem_t seatBelt;

// Marcar para detener el hilo de barbero cuando todos los clientes hayan sido atendidos
int allDone = 0;

int main(int argc, char *argv[]) {

pthread_t btid;
pthread_t tid[MAX_CUSTOMERS];
long RandSeed;
int i, numCustomers, numChairs;
int Number[MAX_CUSTOMERS];

printf("Ingrese el numero de Clientes : "); scanf("%d",&numCustomers) ;
printf("Ingrese el numero de sillas : "); scanf("%d",&numChairs);

// Asegura que la cantidad de subprocesos sea menor que la cantidad de clientes que podemos atender
if (numCustomers > MAX_CUSTOMERS) {
printf("El numero maximo de Clientes es %d.\n", MAX_CUSTOMERS);
exit(-1);
}

// Inicializa el array de números
for (i=0; i<MAX_CUSTOMERS; i++) {
Number[i] = i;
}

// Inicializa los semáforos con los valores iniciales
sem_init(&waitingRoom, 0, numChairs);
sem_init(&barberChair, 0, 1);
sem_init(&barberPillow, 0, 0);
sem_init(&seatBelt, 0, 0);

// Crea el barbero.
pthread_create(&btid, NULL, barber, NULL);

// Crea los clientes.
for (i=0; i<numCustomers; i++) {
pthread_create(&tid[i], NULL, customer, (void *)&Number[i]);
sleep(1);
}

// Une a cada uno de los hilos para esperar a que terminen
for (i=0; i<numCustomers; i++) {
pthread_join(tid[i],NULL);
sleep(1);
}

// Cuando todos los clientes hayan terminado, mata el hilo del barbero
allDone = 1;
sem_post(&barberPillow); // Despierta al barbero para que salga
pthread_join(btid,NULL);
}


void *customer(void *number) {
int num = *(int *)number;

// Sale para la tienda y toma una cantidad aleatoria de tiempo para llegar
printf("El cliente %d se va a la peluqueria.\n", num);
randwait(2);
printf("El cliente %d llegó a la peluqueria.\n", num);

// Espera a que se abra espacio en la sala de espera
sem_wait(&waitingRoom);
printf("Cliente %d entrando a la sala de espera.\n", num);

// Espera a que la silla de barbero quede libre
sem_wait(&barberChair);

// La silla está libre así que cede su lugar en la sala de espera
sem_post(&waitingRoom);

// Despierta al barbero
printf("Cliente %d despertando al barbero.\n", num);
sem_post(&barberPillow);

// Espera a que el barbero termine de cortarte el pelo
sem_wait(&seatBelt);

// abandona la silla
sem_post(&barberChair);
printf("Cliente %d saliendo de la peluqueria.\n", num);
}

void *barber(void *junk) {
// Si bien todavía hay clientes para ser atendidos
// Nuestro barbero es omnisciente y puede saber si todavía hay clientes en camino a su tienda
while (!allDone) {

// Duerme hasta que alguien llegue y lo despierte
printf("El barbero esta durmiendo\n");
sem_wait(&barberPillow);

// Salta estas cosas al final
if (!allDone) {

// Toma una cantidad aleatoria de tiempo para cortar el cabello del cliente
printf("El barbero esta cortando el cabello\n");
randwait(2);
printf("EL barbero ha terminado de cortar el cabello.\n");

// Suelta al cliente cuando termine de cortar
sem_post(&seatBelt);
}
else {
printf("El peluquero se va a casa por el dia.\n");
}
}
}

void randwait(int secs) {
int len;

// Generar un número aleatorio
len = (int) ((1 * secs) + 1);
sleep(len);
}
