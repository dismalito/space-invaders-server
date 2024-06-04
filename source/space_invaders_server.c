#include<stdlib.h> 		// Para los exit
#include<sys/types.h> 		// Para la definicion de pid_t
#include<stdio.h> 		// Para los printf
#include<unistd.h>		// Para el fork
#include<string.h>		// Para cadenas
#include"../library/_functions.h"

/*VARIABLES GLOBALES*/
//pid_t *partidas;//sera un vector de partidas 


void main(int argc, char *argv[]){
        
	printf("Ejecutando Validaciones Preliminares...\n");
	/***********VALIDACIONES*********/
	if (validacionGeneral(argc, argv) == 1)
		exit(EXIT_FAILURE);

	printf("Conexiones Maximas Iniciales: %d\n",Param.conexiones);
	printf("Puerto de Comunicacion(indicar a los clientes): %d\n",Param.puerto);
	printf("Vidas Iniciales por Cliente: %d\n",Param.vidas);
	/********FIN VALIDACIONES********/
        
	int salir=0;
        char buffer[100];
        pthread_mutex_init(&mutexSDL,NULL);//INICIA EL MUTEX
       
        inicializar(argc);//INICIA SEMAFOROS//MEMORIA COMPARTIDA//SOCKET LISTEN
        
        pthread_create(&sdl_pid, NULL, (void *)&thread_draw,(void*)&salir);//CREO UN THREAD PARA DIBUJAR
        pthread_create(&event_pid, NULL, (void *)&events,(void*)&salir);//CREO UN THREAD PARA TECLAS Y MOUSE
        pthread_create(&listen_pid, NULL, (void *)&acceptConnection,(void*)&salir);//CREO UN THREAD PARA ACEPTAR CONEXIONES DE JUGADORES DISTINTOS
        
	//logica para inicio de servidor de partida mediante proceso pesad
        while(salir!=1){//podria ser un flag de corte total
	//mientras haya uno solo conectado sigo buscando y ademas no haya terminado el tiempo de torneo 
	//y no hayan jugado todos contra todos sigo tirando partidas
	        int i,j,k;
	        int cola[SIZE];
	        int cant=cantidadWait(cola);
	        if(cant>1)//SI ME VOY DE LA PARTIDA ELIMINAR SUS PARTIDAS
	                for(i=0;i<cant;i++)
	                        for(j=i+1;j<cant;j++){
	                                printf("[%d:%d]\n",cola[i],cola[j]);
	                                if(check_partida(cola[i],cola[j])==0){//SE PUEDE
	                                        int index_partida = cant_partidas;
	                                        partidas[index_partida].indexPlayer1 = cola[i];
	                                        partidas[index_partida].indexPlayer2 = cola[j];
	                                        partidas[index_partida].mode=1;//JUGANDOSE
	                                        
	                                        setWaitPlayer(cola[i],0);
	                                        setWaitPlayer(cola[j],0);
	                                        cant_partidas++;
	                                        pthread_mutex_unlock(&mutexSDL);//PERMITO DIBUJAR
	                                        //INICIO EL PROCESO DE PARTIDA
	                                        newGame(cola[i],cola[j],index_partida);
	                                        //SACO A LOS PLAYERS DE LISTA DE ESPERA
	                                        
	                                        j=cant;i=cant;
	                                }
	                        }
	        
	        sleep(1);
       }

        pthread_join(event_pid, NULL);
        strcpy(buffer,"exit");
        //send(dataSockets->comm_sockets[0],buffer,sizeof(char)*200,0);
        //close(dataSockets->comm_sockets[0]);
        close(sockets[LISTEN]);
        eliminarMemoriaCompartida();//ELIMINO MEMORIA COMPARTIDA
}
