/************* DEMONIO SPACE INVADERS *****************

Demonio encargado de verificar el estado del servidor
Y liberar recursos ante una finalización abrupta     */

#include<stdlib.h> 		// Para los exit
#include<sys/types.h> 	// Para la definicion de pid_t
#include<stdio.h> 		// Para los printf
#include<unistd.h>		// Para el fork
#include<string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include<errno.h>

typedef struct{
        int socketListen;//SOCKET DE ESCUCHA
        int comm_sockets[10];//SOCKETS DE COMUNICACION
}Sockets;

Sockets *dataSockets;

int open_and_map(void **addr,int size,char*name){
        size=shm_open(name,O_RDWR,0666);
        if(size==-1)
                printf("error in shm_open\n");
        if(ftruncate(size,100) == -1)
                printf("error in ftruncate\n");
        if((*addr=mmap(0,100,PROT_READ|PROT_WRITE,MAP_SHARED,size,0))== MAP_FAILED)
                printf("error in mmap\n");
        return 0;
}
//GUARDA PID DEL DEMONIO PARA QUE SOLO SE CREE UNA INSTANCIA DEL MISMO
void saveFile(){
        int pid=getpid();
        FILE *pf=fopen("space_invaders_daemon.pid","w");
        fwrite(&pid,sizeof(int),1,pf);//GUARDO NUEVO PID
        fclose(pf);
}

int search_process(int pid){//BUSCA SI EL PROCESO ESTA ACTIVO
	FILE *fichero=NULL;
	char aux[1000];int i=0;
	sprintf(aux,"ps %d",pid);
	fichero = popen (aux,"r");
	fgets(aux, 1000, fichero);//OBTENGO PRIMERA LINEA DEL PS
	while (!feof (fichero)){
		i++;//CUENTA LAS LINEAS DEL PS
		fgets(aux, 1000, fichero);
	}
	pclose (fichero);
	if(i==1)
	        return -1;//NO SE ENCONTRO PROCESO ACTIVO
	return 0;//SE ENCONTRO PROCESO ACTIVO
}

int main(int argc,char*argv[]){//SE RECIBE COMO PARAMETRO EL PID DEL SERVIDOR Y EL NOMBRE DEL PROGRAMA "./servidor"
	if(fork())
	        exit(0);
	umask(0);
	int sid=setsid();
	
        //INICIA DEMONIO
        int status;char buffer[200];
        
        saveFile();//GUARDO PID DAEMON
        
        do{
                FILE *pf = fopen("servidor.pid","r");
                int pid;
                fread(&pid,sizeof(int),1,pf);//RECUPERO EL PID DEL FICHERO
                fclose(pf);
                //sprintf(buffer,"bash source/search_process.sh %d \"%s\"",ppid.pid,ppid.argv);
                do{
                        sleep(1);
                        //status=system(buffer);//VERIFICA SI EL SERVIDOR SIGUE CORRIENDO (0 AFIRMATIVO !=0 NEGATIVO)
                }while(!search_process(pid));//MIENTRAS ESTE ONLINE SE VERIFICA        
                //INICIANDO LIBERACIÓN DE RECURSOS
                open_and_map((void *)&dataSockets,sizeof(int),"/memorySockets");
                //close(dataSockets->comm_sockets[0]);
                close(dataSockets->socketListen);
                //BORRANDO SEMAFOROS Y MEMORIA COMPARTIDA
                printf("borrando recursos\n");
                remove("/dev/shm/sem.semFicheroMutex");
                remove("servidor.pid");
                remove("/dev/shm/memorySockets");//NO SE ELIMINA LOS SOCKETS PORQUE SE VUELVEN A USAR EN EL RELANZAMIENTO
                system("./space_invaders_server -R &");
                sleep(2);//ESPERA A QUE SE ACTUALICE EL ARCHIVO
        }while(1);
        
        return 0;
}
