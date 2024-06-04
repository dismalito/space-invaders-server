#include"_semaphore.h"

sem_t* crearSemaforo(const char* nombreSemaforo, int valorInicial){
	sem_t *semaforo;
	semaforo = sem_open(nombreSemaforo,O_CREAT, 0666, valorInicial);//parametros: nombre, creacion,permisos, valor inicial
	if(semaforo == SEM_FAILED)
		printf("[El semaforo no se pudo CREAR]\n");
	return semaforo;
}

sem_t* accederSemaforo(const char* nombreSemaforo){
	sem_t *semaforo;
	semaforo = sem_open(nombreSemaforo,0);
	if(semaforo == SEM_FAILED)
		printf("[El semaforo no EXISTE]\n");
	return semaforo;
}

int procesoLiberaSemaforo(sem_t * sem){
	int estado;
	estado = sem_close(sem);
	if(estado == -1)
		printf("[No se pudo liberar el semaforo]\n");
	return estado;
}

int eliminarSemaforo(const char* nombreSemaforo){
	int estado;
	estado = sem_unlink(nombreSemaforo);
	return estado;
}

void P(sem_t * sem){
	int estado;
	estado = sem_wait(sem);
	if(estado == -1)
		printf("ERROR");
}

void V(sem_t * sem){
	int estado;
	estado = sem_post(sem);
}

int getValSem(sem_t * sem){
	int val;
	sem_getvalue(sem,&val);
	return val;
}

int setValueSem(sem_t *sem,int value){
        int i;
        for(i=0;i<value;i++)
                V(sem);
}

void PNoBloq(sem_t * sem){
	int estado;
	estado = sem_trywait(sem);
}

void verSemaforos(){
	system("ls /dev/shm");
}

void verSemaforosV2(){
	system("ipcs -sa");
}
