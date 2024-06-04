#ifndef _validations
#define _validations

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>
#include <regex.h>		//para manejo de regular expresion
#include <ctype.h>
//#include <funciones.h> --> aca iria la biblioteca que contiene todas las estructuras de datos para el juego

#define MAX_MATCHES 1

//nose si sera provisoria... pero sino puede estar aqui la struct de datos de parametros
typedef struct
{
	int puerto;
	int vidas;
	int conexiones;
}parametros;
parametros Param;

int es_numero(const char*);
int validaNumero(char*);
int validaArchPermExit (const char*);
int archVacio(const char*);
int getCantLineas (char *);
int validarLinea(char *, const char*);
char* getParamString (char*);
int validaConexiones(char *);
int validaVidas(char *);
int validaPuerto(char *);
int existArch(const char*);
char* trim(char *);
char* getDinamicString (char staticstring []);
int validaFormatoYcarga (char *);
void mensajeSalida();
int validacionGeneral(int , char *argv[]);

#endif /*_FUNCIONES_AUXILIARES*/
