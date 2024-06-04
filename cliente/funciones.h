#ifndef _funciones 
#define _funciones

#include<stdlib.h> 		// Para los exit
#include<sys/types.h> 	// Para la definicion de pid_t
#include<stdio.h> 		// Para los printf
#include<unistd.h>		// Para el fork
#include<sys/wait.h>	// Para la definicion de wait
#include<string.h> 		//Para manejo de cadenas
#include<sys/socket.h> //socket(),bind(),inet_addr()
#include<netinet/in.h> //inet_addr()
#include<arpa/inet.h> //inet_addr()
#include<sys/stat.h>
#include<limits.h>
#include<fcntl.h> //O_CREATE..etc
#include<regex.h>
#include<time.h>
#include<pthread.h>
#define MAXQ 10
#define SIZE 10

///Estructuras para enviar como clientes y recibir como servidor
typedef struct{int f1,c1,f2,c2;}SendInfoCliente;
//Estructura para enviar como servidor y recibir como cliente
typedef struct{int f1,c1,v1,f2,c2,v2;}SendInfoServidor;

//CABECERA DE FUNCIONES//
void Help();
char*getFecha();
char*getFileName();
void mostrar(int[][SIZE],int[][SIZE]);
void registrarTablero(int[][SIZE],int);
void informar(SendInfoServidor*infoServidor,int tablero[][SIZE],int usados[][SIZE]);
int validaNumero(char*);
void getNumeros(SendInfoCliente*,int[][SIZE]);
void limpiar();
void mostrarEnGrilla(int row, int column,int[][SIZE],int[][SIZE]);
void refresh(int[][SIZE],int[][SIZE]);
int finalizado(int[][SIZE]);
void getIPPORT(char*ip,int *puerto, int argc, char*argv[]);
#endif
