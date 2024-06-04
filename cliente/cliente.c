#include"funciones.h"
#include<stdlib.h>
#define MAXQ 10


int i,j,caller_socket=0,fdLogCliente,puerto;
char ip[12];
pthread_t tidR,tidE;
int exitClient=0;
void SocketConnect();//FUNCION PARA CONECTARSE AL SERVIDOR
void*recibiendoInfo(int*);//FUNCION QUE RECIBE PAQUETES
void*enviandoInfo(int*);//FUNCION QUE ENVIA PAQUETES
void inicializar();//CREA FICHERO LOG//SETEA SEÑALES//CONECTA AL SERVIDOR//DIBUJA PANTALLA
int terminarReceive=0;
int terminarSend=0;

int main(int argc, char*argv[]){
        char buffer[200];
        getIPPORT(ip,&puerto,argc,argv);//OBTENGO IP Y PUERTO
        SocketConnect(ip,puerto);//CREO LA CONEXION CON EL SERVIDOR
        printf("Ingrese su nick: ");
        gets(buffer);
        send(caller_socket,buffer,sizeof(char)*200,0);
        pthread_create(&tidR, NULL, (void *)&recibiendoInfo,(void*)&exitClient);//CREO UN THREAD PARA RECIBIR MENSAJES
        pthread_create(&tidE, NULL, (void *)&enviandoInfo,(void*)&exitClient);//CREO UN THREAD PARA ENVIAR MENSAJES       
        
        //ESPERO MIENTRAS EL JUEGO NO TERMINE O EL SERVIDOR NO SE DESCONECTE
        pthread_join(tidR, NULL);
        pthread_join(tidE, NULL);
        close(caller_socket);//CIERRO SOCKET
	close(fdLogCliente);
}


//FUNCION PARA RECIBIR PAQUETES//MANEJADA POR UN THREAD
void *recibiendoInfo(int *exitClient){
        char buffer[200];
        while(strcmp(buffer,"exit")!=0 && terminarReceive==0){
                if(recv(caller_socket,buffer,sizeof(char)*200,0)!=-1){//RECIBO RESULTADO
                        printf("[%s]\n",buffer);
                        if(strcmp(buffer,"exit")==0)
                                terminarReceive=1;
                }
                else{//SI TUVE ERROR AL RECIBIR
                        pthread_cancel(tidE);//CIERO THREAD DE ENVIOS
                        printf("Se ha perdido la conexión con el servidor\n");
                        terminarReceive=1;
                }
        }
        //pthread_cancel(tidE);//CIERO THREAD DE ENVIOS     
	pthread_exit(exitClient);
}
//FUNCION PARA ENVIAR PAQUETES//MANEJADA POR UN THREAD
void *enviandoInfo(int *exitClient){
        char buffer[200];
        while(terminarSend==0){//MIENTRAS EL JUEGO NO TERMINE
                //ENVIO COORDENADAS
                printf("mensaje:");
                fflush(stdin);
                scanf("%s",buffer);
                if(send(caller_socket,buffer,sizeof(char)*200,0)==-1){//SI SE ENVIO CORRECTAMENTE ESPERO RESULTADO
                        //pthread_cancel(tidR);//CIERRO THREAD DE RECIBIR
                        printf("Se ha perdido la conexión con el servidor\n");
                        terminarSend=1;
                }
                //sleep(1);
                if(strcmp(buffer,"exit")==0)
                        terminarSend=1;
        }
        pthread_exit(exitClient);
}

//CREA SOCKET DE CONEXION
void SocketConnect(char*ip,int puerto){
        char buffer[200];
        unsigned short int listen_port=0;
        unsigned long int listen_ip_address=0;
        struct sockaddr_in listen_address;
        //Creación del socket
        caller_socket=socket(AF_INET,SOCK_STREAM,0);
        if(caller_socket==-1){
                sprintf(buffer,"[%s]: No sea ha podido establecer conexión con [%s:%d]. Programa finalizado",getFecha(),ip,puerto);
                printf("%s\n",buffer);
                exit(EXIT_FAILURE);
        }        
        listen_address.sin_family=AF_INET;
        listen_port=htons(puerto);//Indico puerto
        listen_address.sin_port=listen_port;
        listen_ip_address=inet_addr(ip);
        listen_address.sin_addr.s_addr=listen_ip_address;
        bzero(&(listen_address.sin_zero),8);
        if(connect(caller_socket,(struct sockaddr*)&listen_address,sizeof(struct sockaddr))==-1){//Me conecto al servidor
                sprintf(buffer,"[%s]: No sea ha podido establecer conexión con [%s:%d]. Programa finalizado",getFecha(),ip,puerto);
                printf("%s\n",buffer);
                exit(EXIT_FAILURE);
        }        
}
