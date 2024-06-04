#ifndef _semaphore
#define _semaphore
#include <semaphore.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>

sem_t* crearSemaforo(const char* nombreSemaforo, int valorInicial);
sem_t* accederSemaforo(const char* nombreSemaforo);
int procesoLiberaSemaforo(sem_t * sem);
int eliminarSemaforo(const char* nombreSemaforo);
void P(sem_t * sem);
void V(sem_t * sem);
int getValSem(sem_t * sem);
int setValueSem(sem_t *sem,int value);
void PNoBloq(sem_t * sem);
void verSemaforos();
void verSemaforosV2();

#endif
