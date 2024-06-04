#ifndef _functions 
#define _functions
#include<stdlib.h> 		// Para los exit
#include<sys/types.h> 	// Para la definicion de pid_t
#include<stdio.h> 		// Para los printf
#include<unistd.h>		// Para el fork
#include<errno.h>		// Para el perror
#include<sys/wait.h>	// Para la definicion de wait
#include<string.h> 		//Para manejo de cadenas
#include<sys/socket.h> //socket(),bind(),inet_addr()
#include<netinet/in.h> //inet_addr()
#include<arpa/inet.h> //inet_addr()
#include<sys/stat.h>
#include<limits.h>
#include<fcntl.h>
#include<regex.h>
#include<pthread.h>
#include<signal.h>
#include"_semaphore.h" // Funciones de Usuario
#include"_validations.h" // Funciones de Usuario
#include"_shared_memory.h"
#include"_SDL.h"

#define SIZE 12
#define LISTEN 11
#define PORT 53200
//MARGEN DEL ALTO
#define MARGEN_X 861
//MARGEN DEL ANCHO
#define MARGEN_Y 437
//BITS POR PIXEL (DEFINE LA CALIDAD DE IMAGEN)
#define BITS 32

int *sockets;//VECTOR DE SOCKETS INDEX 0 PARA LISTEN INDEX>0 PARA COMUNICACION
typedef struct{
        pid_t pid;
        int mode;//SI YA FUE JUGADA O ESTA ONLINE
        int indexPlayer1;//-1 REUTILIZA
        int indexPlayer2;
}Partida;
Partida partidas[SIZE*SIZE];//PARTIDAS JUGADAS O JUGANDOSE

typedef struct{
        int rank;
        char nick[8];//7 CARACTERES MAX
        int points;
        int games;//CANTIDAD DE PARTIDAS JUGADAS
        int ratio;//WINS/LOSSES
}Player;//ESTRUCTURA DE MEMORIA COMPARTIDA
Player *memory_players;//MEMORIA COMPARTIDA DE JUGADORES

typedef struct{
        int point;
        int lives;
        int naveRoja;
}InfoPlayer;
InfoPlayer InfoplayersPartida[SIZE*SIZE];

typedef struct{
        int bloques[12];
}Escudo;

typedef struct{
        int naves[50][50];
}NavesEnemigas;
NavesEnemigas NavEnemigas[SIZE*SIZE];


/*Declaro semaforos*/
sem_t *semFicheroMutex;//CONTROLA LAS PETICIONES
pthread_mutex_t mutexSDL;
pthread_mutex_t mutex_rcv[SIZE];
pthread_mutex_t mutex_partida[SIZE];
struct sockaddr_in con_address[SIZE];//INFORMACION DE LAS IPS CONECTADAS

/*Declaro Threads*/
pthread_t sdl_pid,event_pid,listen_pid;

/*BUFFERS COLAS */
int id_conectados[SIZE];
int status[SIZE];//ESTADO EN ESPERA O NO
int id_enEspera[SIZE];
int cant_partidas;
Partida id_enPartida[SIZE];
pthread_t sdl_pid,event_pid,listen_pid,tids[SIZE], tidsPartidas[SIZE], tidsPartidasSend[SIZE*SIZE];
char bufferPartidas[200];

//CABECERA DE FUNCIONES//

///// MEMORIA SEMAFOROS /////
void crearMemoriaCompartida();
void eliminarMemoriaCompartida();
void creandoSemaforos();
void liberandoSemaforos();
void eliminandoSemaforos();

///// INICIO Y FINALIZADO /////
void finalizar();
void inicializar(int);
void setSignal();
void manejar_senal(int,siginfo_t*,void*);
int check_server_run();
int daemon_start();
void createSocket(int);

//// ESTADISTICAS ////
void rank_refresh();
void wait_refresh();
void games_refresh();

//// THREADS /////
void*events(int*);
void*thread_draw(int*);
void*acceptConnection(int*);
void*communication(int*);
void*enviandoNaves(int *);
void*communicationPartida(int *);
void*communicationPartidaSend(int *);

//PROCESO SERVIDOR DE PARTIDA
void newGame(int,int,int);

///// OTROS /////
char*getFecha();
void setPlayer(int,int,char*,int,int,int,int);
void setWaitPlayer(int,int);
int getIndex();
int getIndexPartida();
void check_mouse_position(Uint16,Uint16,int*,int*);
int cantidadCola();//CANTIDAD DE CONECTADOS
int cantidadWait(int[SIZE]);
int check_partida(int,int);
int search_pid(int);

#endif
