#include"funciones.h"

int x1,ys1,x2,ys2,mode=0;

void Help(){
        printf("###############################################################################");
        printf("\n\t\t***MEMOTEST CLIENTE***\n\n");
        printf("Cliente para el juego memotest.\n\n");
        printf("PARAMETROS:\n\t-? para la ayuda, -i para la ip y -p para ingresar el puerto de comunicación\n");
        printf("MODO DE EJECUCION: ./cliente -i IP -p PUERTO\n");
        printf("Se registra todo en un fichero de log con la fecha\n\n");
        printf("###############################################################################\n");
        exit(0);
}
//Función para tener la fecha actual
char* getFecha (){
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char* fechahora = (char*) malloc(25);
	sprintf(fechahora,"%02d/%02d/%d %d:%02d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min);
	return fechahora;
}

char* getFileName (){
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char *fechahora = (char*)malloc(sizeof(char)*30);
	sprintf(fechahora,"%02d%02d%d%02d%02d[cliente].log", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min);
	return fechahora;
}

void setInfoClienteToServidor(SendInfoCliente *infoCliente, SendInfoServidor *infoServidor, int grilla[][SIZE]){
	infoServidor->f1=infoCliente->f1;
        infoServidor->c1=infoCliente->c1;
        infoServidor->v1=grilla[infoCliente->f1][infoCliente->c1];
        infoServidor->f2=infoCliente->f2;
        infoServidor->c2=infoCliente->c2;
        infoServidor->v2=grilla[infoCliente->f2][infoCliente->c2];
}

void getNumeros(SendInfoCliente *infoCliente,int usados[][SIZE]){
        char aux[100];
        int cond1,cond2;
        do{
                do{
                        printf("\033[0;34mFila 1:\033[0m");
                        fgets(aux,99,stdin);
                
                }while(validaNumero(aux)==-1);
                infoCliente->f1=atoi(aux);
                do{
                        printf("\033[0;34mColumna 1:\033[0m");
                        fgets(aux,99,stdin);
                }while(validaNumero(aux)==-1);
                infoCliente->c1=atoi(aux);
                do{
                        printf("\033[0;34mFila 2:\033[0m");
                        fgets(aux,99,stdin);
                }while(validaNumero(aux)==-1);
                infoCliente->f2=atoi(aux);
                do{
                        printf("\033[0;34mColumna 2:\033[0m");
                        fgets(aux,99,stdin);
                }while(validaNumero(aux)==-1);
                infoCliente->c2=atoi(aux);
                cond1 = (infoCliente->f1==infoCliente->f2 && infoCliente->c1==infoCliente->c2)?1:0;//COORDENADAS IGUALES
                cond2 = (usados[infoCliente->f1][infoCliente->c1] || usados[infoCliente->f2][infoCliente->c2])?1:0;//COORDENADAS USADAS
        }while(cond1 || cond2);
        //printf("\033[19;9f");
}

int validaNumero(char*string){
        char *aux=string;aux+=strlen(string)-1;*aux='\0';//ELIMINO EL SALTO DE LINEA '\n'
        regex_t retmp;
        regmatch_t mtmp;
        regcomp(&retmp,"^[0-9]$",REG_EXTENDED);
        if(!regexec(&retmp,string, (size_t) 1, &mtmp, 0))
                return 0;
        return -1;
}

void mostrarEnGrilla(int row, int column,int tablero[][SIZE],int usados[][SIZE]){
        if(usados[row][column]==0)
                printf(" -- |");
        else
                if(mode==0)
                        printf(" \033[0;34m%02d\033[0m |",tablero[row][column]);
                else
                        if((x1==row && ys1==column) || (x2==row && ys2==column))
                                printf(" \033[0;31m%02d\033[0m |",tablero[row][column]);
                        else
                                printf(" \033[0;34m%02d\033[0m |",tablero[row][column]);
}

void mostrar(int tablero[][SIZE],int usados[][SIZE]){
        int i,j;
        printf("\033[0;34m********************** \033[1;34mMEMOTEST\033[0;34m **********************\033[0m\n\nTablero:\n\n");
        printf("\033[0;33m     [0]  [1]  [2]  [3]  [4]  [5]  [6]  [7]  [8]  [9]\033[0m\n");
        for(i=0;i<SIZE;i++){
                printf("\033[0;33m[%d]\033[0m|",i);
                for(j=0;j<SIZE;j++)
                        mostrarEnGrilla(i,j,tablero,usados);
                printf("\n");
        }
}

void refresh(int tablero[][SIZE],int usados[][SIZE]){
        limpiar();
        printf("\033[0;0f");
        mostrar(tablero,usados);//MOSTRAR MATRIZ
        printf("\nIngrese las coordenadas elegidas:\n\n");
}

void informar(SendInfoServidor*infoServidor,int tablero[][SIZE],int usados[][SIZE]){
        tablero[infoServidor->f1][infoServidor->c1]=infoServidor->v1;
        tablero[infoServidor->f2][infoServidor->c2]=infoServidor->v2;
        usados[infoServidor->f1][infoServidor->c1]=1;usados[infoServidor->f2][infoServidor->c2]=1;
        x1=infoServidor->f1;ys1=infoServidor->c1;x2=infoServidor->f2;ys2=infoServidor->c2;mode=1;
        refresh(tablero,usados);
        sleep(1);
        usados[infoServidor->f1][infoServidor->c1]=0;usados[infoServidor->f2][infoServidor->c2]=0;mode=0;
}

int finalizado(int usados[][SIZE]){//VERIFICA SI SE DESCUBRIERON TODOS LOS CASILLEROS
        int i,j;
        int res=1;
        for(i=0;i<SIZE;i++){
                for(j=0;j<SIZE;j++){
                        res*=usados[i][j];
                }
        }
        return res;
}

void limpiar(){
	system("clear");        
	//printf("\033[2J");
        printf("\n");
        int i;
        printf("\033[0;0f");//ME POSICIONO EN LA (0,0)    
        for(i=0;i<19;i++)//LIMPIO UNAS 19 LINEAS
                printf("                                                      \n");       
        printf("\033[0;0f");//ME VUELVO A POSICIONAR EN LA (0,0)        
}

void getIPPORT(char*ip,int *puerto, int argc, char*argv[]){
        if(argc==2){//SI ENTRA A LA AYUDA
                if(strcmp(argv[1],"-?")==0 || strcmp(argv[1],"-help")==0)
	                Help();
	        else{
	                printf("ERROR: La cantidad de parametros ingresados es incorrecta.\n");
		        printf("Uso: %s -i IP -p PUERTO\nPara más ayuda ingrese -? -help\n", argv[0]);
		        exit(EXIT_FAILURE);
		}
        }
        if(argc!=5){
		printf("ERROR: La cantidad de parámetros ingresados es incorrecta.\n");
		printf("Uso: %s -i IP -p PUERTO\nPara más ayuda ingrese -? -help\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	if(strcmp(argv[1],"-i")!=0 && strcmp(argv[3],"-p")!=0){
	        printf("ERROR: Los parámetros son incorrectos\n");
		printf("Uso: %s -i IP -p PUERTO\nPara más ayuda ingrese -? -help\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	regex_t retmp;regmatch_t mtmp;
        regcomp(&retmp,"^(([1-9]?[0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]).){3}([1-9]?[0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$",REG_EXTENDED);
        if(regexec(&retmp,argv[2], (size_t) 1, &mtmp, 0)){
                printf("ERROR: El parámetro ingresado no corresponde a una dirección IP válida.\n");
		printf("Para más ayuda ingrese -? -help\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	regcomp(&retmp,"^[0-9][0-9][0-9][0-9][0-9]$",REG_EXTENDED);
        if(regexec(&retmp,argv[4], (size_t) 1, &mtmp, 0)){
                printf("ERROR: El parámetro ingresado debe ser un puerto del rango 49152-65535.\n");
		printf("Para más ayuda ingrese -? -help\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	strcpy(ip,argv[2]);
	*puerto=atoi(argv[4]);
}
