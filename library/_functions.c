#include"_functions.h"
//SETEA ESTADISTICAS DEL JUGADOR
void setPlayer(int index,int rank,char*nick,int points,int games, int ratio,int _status){
        id_conectados[index]=rank;//MODIFICAR!!!
        strcpy(memory_players[index].nick,nick);
        memory_players[index].rank=rank;
        memory_players[index].points=points;
        memory_players[index].games=games;
        memory_players[index].ratio=ratio;
        status[index]=_status;
}
//FUNCION PARA LIBERAR RECURSOS
void finalizar(){
        //RECUPERO PID PARA MANDAR SEÑAL AL DEMONIO
        int pid;
        FILE *pf=fopen("space_invaders_daemon.pid","r");
        fread(&pid,sizeof(int),1,pf);
        fclose(pf);
        kill(pid,SIGKILL);//MANDO SEÑAL AL DEMONIO
        remove("servidor.pid");//BORRO ARCHIVOS DE PID
        remove("space_invaders_daemon.pid");//BORRO ARCHIVOS DE PID
        SDL_Quit();//LIBERO RECURSOS SDL
        liberandoSemaforos();//LIBERO RECURSOS SEMAFORO
        eliminandoSemaforos();//ELIMINO SEMAFOROS
}
//SE INICIALIZAN SEMAFOROS, MEMORIA, SOCKETS, DEMONIO, VERIFICA DUPLICADO DE SERVIDORES
void inicializar(int argc){
        //VERIFICAR QUE EL SERVIDOR NO ESTE INICIADO (DUPLICADOS DE SERVIDORES)
        if(check_server_run()==-1){
                printf("Se ha detectado que el servidor ya se encuentra corriendo\n");
                exit(EXIT_FAILURE);
        }
        if(argc==1){//PARAMETRO -R
                //SE INICIA EL DEMONIO
                daemon_start();
        }
        SDLRun(MARGEN_X,MARGEN_Y,BITS,"SPACE INVADERS");        
        creandoSemaforos();
        crearMemoriaCompartida();
        createSocket(53200);//creo socket        
        setSignal();
        int i;
        for(i=0;i<SIZE;i++){
                id_conectados[i]=0;
                partidas[i].indexPlayer1=-1;
                partidas[i].mode=0;
                status[i]=0;
                pthread_mutex_init(&(mutex_rcv[i]),NULL);
        }
}

/*MANEJA SEÑALES*/
void manejar_senal(int iNumSen, siginfo_t *info, void *noseusa){
        int index;
	switch (iNumSen){
		case SIGCHLD:
        		index=search_pid(info->si_pid);
		        if(info->si_code != CLD_KILLED){		        
	        	        partidas[index].mode=0;
	        	        printf("Se termino el game: %d\n",index);
	        	        pthread_mutex_unlock(&mutexSDL);//PERMITO DIBUJAR
	        	        pthread_mutex_unlock(&(mutex_rcv[partidas[index].indexPlayer1]));
      	        	        pthread_mutex_unlock(&(mutex_rcv[partidas[index].indexPlayer2]));
	        	        //DESBLOQUEAR LOS MUTEX
	        	}else{//RELANZAR
	        	        printf("Se aborto el game: %d\n",index);
	        	        printf("Relanzando el game: %d\n",index);
	        	        newGame(partidas[index].indexPlayer1,partidas[index].indexPlayer2,index);
	        	}
			while (waitpid (-1, NULL, WNOHANG) > 0);
			break;
		default:
			break;
	}
}
//FUNCION PARA OBTENER UN SLOT LIBRE DEL VECTOR DE CONECTADOS
int getIndex(){//BUSCA UN LUGAR LIBRE
        int index;
        for(index=0;index<SIZE;index++)
                if(id_conectados[index]==0)
                        return index;
        return -1;
}
//FUNCION PARA OBTENER UN SLOT LIBRE DEL VECTOR PARTIDAS
int getIndexPartida(){//BUSCA UN LUGAR LIBRE
        int index;
        for(index=0;index<SIZE*SIZE;index++)
                if(partidas[index].indexPlayer1==-1)
                        return index;
        return -1;
}
//THREAD PARA CAPTURAR EVENTOS DEL MOUSE
void *events(int *salir){
        int status_exit=0,status_stop=0,terminar=0;
        Uint16 x,y;
        while(!terminar){//PIDO MUTEX DIBUJAR
                //// INDICO QUE HAY UN NUEVO EVENTO ////
                SDL_WaitEvent(&event);
                if(event.type==SDL_QUIT)//SE PRESIONA LA X DE SALIR
                        terminar=1;
                if(event.type==SDL_MOUSEMOTION)//EVENTO DE MOUSE MOVIENDOSE
                        check_mouse_position(event.motion.x,event.motion.y,&status_exit,&status_stop);
                if(status_exit==1 && event.type==SDL_MOUSEBUTTONDOWN && event.button.button==SDL_BUTTON_LEFT)
                        terminar=1;//SE PULSA BOTON EXIT
        }
        *salir=1;
        pthread_cancel(sdl_pid);//CIERRO THREAD DE DIBUJO
        pthread_cancel(listen_pid);//CIERO THREAD LISTEN
        finalizar();//LIBERA RECURSOS//DA DE BAJA AL DEMONIO
        pthread_exit(salir);
}
//THREAD PARA DIBUJAR LA PANTALLA
void *thread_draw(int *salir){
        setPosition(&btn_stop,308,92);//POSICION BOTON TERMINAR
        setSprite(&btn_stop,BTN_STOP);
        setPosition(&btn_exit,592,92);//SETEO POSICION DEL BOTON SALIR
        setSprite(&btn_exit,BTN_EXIT);//SETEO IMAGEN DEL BOTON SALIR
        //ESCRIBO TEXTO A MOSTRAR RANK NICK POINTS GAMES RATIO
        while(1){//PIDO MUTEX DIBUJAR
                printf("redibujando\n");
                pthread_mutex_lock(&mutexSDL);
                //PIDO MUTEX MEMORIA_RANK//MUTEX MEMORIA_ESPERA//MUTEX MEMORIA_JUGANDO
                SDLClear();//LIMPIO PANTALLA
	        SDL_BlitSurface(SDLScreen,NULL,SDLBuffer,NULL);//AGREGO FONDO	        
                //MOSTRAR ESTADISTICAS
                rank_refresh();
                wait_refresh();
                games_refresh();                
	        addScreen(&btn_stop,BOTH);
	        addScreen(&btn_exit,BOTH);
                showScreen();
        }
        *salir=1;
        pthread_exit(salir);
}
//MUESTRA ESTADISTICAS DE JUGADORES CONECTADOS
void rank_refresh(){
        char buffer[100];
        int i,inc=200;
	for(i=0;i<SIZE;i++){
	        if(id_conectados[i]!=0){
                        sprintf(buffer,"%02d    %-7s %06d     %02d   %d",memory_players[i].rank,
                        memory_players[i].nick,memory_players[i].points,memory_players[i].games,
                        memory_players[i].ratio);
	                ttext.imageText=TTF_RenderText_Shaded(fuente,buffer,fuenteColor,screenColor);
	                ttext.position.x=32;
	                ttext.position.y=inc;
	                SDL_BlitSurface(ttext.imageText,NULL,SDLBuffer,&ttext.position);//AGREGO TEXTO
	                inc+=17;
	        }
       }
}
//MUESTRA ESTADISTICAS DE JUGADORES EN ESPERA
void wait_refresh(){
        char buffer[100];
        int i,inc=200;
	for(i=0;i<SIZE;i++){
	        if(id_conectados[i]!=0 && status[i]==1){
                        sprintf(buffer,"%02d    %-7s %06d     %02d   %d",memory_players[i].rank,memory_players[i].nick,
                        memory_players[i].points,
                        memory_players[i].games,memory_players[i].ratio);
	                ttext.imageText=TTF_RenderText_Shaded(fuente,buffer,fuenteColor,screenColor);
	                ttext.position.x=318;
	                ttext.position.y=inc;
	                SDL_BlitSurface(ttext.imageText,NULL,SDLBuffer,&ttext.position);//AGREGO TEXTO
	                inc+=17;
	        }
       }
}
//MUESTRA ESTADISTICAS DE PARTIDAS INICIADAS
void games_refresh(){
        char buffer[100];
        int i,inc=200;
        int player;
        int incNumber=210;
	for(i=0;i<SIZE*SIZE;i++){
	        if(partidas[i].mode==1){
        	        //AGREGO UNION
	                setPosition(&game_union,670,incNumber-4);
	                addScreen(&game_union,BOTH);
	                //SDL_BlitSurface(SDLSprites,&game_union.box,SDLBuffer,&game_union.position);
	                //AGREGO NUMERO DE GAME
	                sprintf(buffer,"%02d",i);
	                ttext.imageText=TTF_RenderText_Shaded(fuente,buffer,fuenteColor,screenColor);
	                ttext.position.x=643;
	                ttext.position.y=incNumber;
	                SDL_BlitSurface(ttext.imageText,NULL,SDLBuffer,&ttext.position);//AGREGO TEXTO
	                incNumber+=33;
	                //AGREGO PLAYER 1
	                player=partidas[i].indexPlayer1;
                        sprintf(buffer,"%02d    %-7s",memory_players[player].rank,memory_players[player].nick);
	                ttext.imageText=TTF_RenderText_Shaded(fuente,buffer,fuenteColor,screenColor);
	                ttext.position.x=710;
	                ttext.position.y=inc;
	                SDL_BlitSurface(ttext.imageText,NULL,SDLBuffer,&ttext.position);//AGREGO TEXTO
	                inc+=17;	                
	                //AGREGO PLAYER 2
	                player=partidas[i].indexPlayer2;
                        sprintf(buffer,"%02d    %-7s",memory_players[player].rank,memory_players[player].nick);
	                ttext.imageText=TTF_RenderText_Shaded(fuente,buffer,fuenteColor,screenColor);
	                ttext.position.x=710;
	                ttext.position.y=inc;
	                SDL_BlitSurface(ttext.imageText,NULL,SDLBuffer,&ttext.position);//AGREGO TEXTO
	                inc+=17;	                	                
	        }
       }
}
//THREAD PARA ACEPTAR NUEVOS JUGADORES
void *acceptConnection(int *salir){//ACEPTA NUEVAS CONEXIONES ENTRANTES Y CREA SOCKETS DE COMUNICACION
	int tamsin=sizeof(struct sockaddr_in);
	char buffer[100];
	//Acepto conexion entrante
	while(*salir!=1){//MIENTRAS EL SERVIDOR NO SE DE DE BAJA
		//bzero(&(con_address[indexSocket+1]),sizeof(con_address[indexSocket+1]));
		struct sockaddr_in _con_address;
                int socket = accept(sockets[LISTEN],(struct sockaddr*)&_con_address,&tamsin);//para cada cliente le corresponde un socket
                int indice=getIndex();//BUSCA UN NUEVO SLOT
                con_address[indice]=_con_address;
                sockets[indice]=socket;
		sprintf(buffer,"[%s]: Se ha establecido una nueva conexion [%s:%d]\n",getFecha(),
		inet_ntoa((con_address[indice]).sin_addr),(con_address[indice]).sin_port);
                printf("%s",buffer);
        	//bzero(buffer,TAMBUF);
        	pthread_create(&tids[indice],NULL,(void*)&communication,(void*)&indice);
	}//SI TERMINE DE ACEPTAR PETICIONES (CERRANDO EL SERVIDOR)
	printf("terminando conexiones\n");
	pthread_exit(salir);
}
//THREAD DE COMUNICACION 1 POR JUGADOR//SE RECIBE NICK DEL JUGADOR//SEÑAL DE EXIT//SEÑAL DE SEARCH_GAME
void *communication(int *indexS){
	int index=*indexS;int term=0;char buffer[200];char nick[8];
        
        //RECIBO NICK
	recv(sockets[index],nick,sizeof(char)*8,0);
	printf("recibi [%s]\n",nick);
	//AGREGO AL JUGADOR EN LA LISTA DE RANK
	//(index,RANK,NICK,POINTS,GAMES,RATIO,STATUS);//STATUS 0 PARA NO ESTAR EN WAIT
	setPlayer(index,1,nick,98722,0,0,0);
	id_conectados[index]=1;
	pthread_mutex_unlock(&mutexSDL);//PERMITO DIBUJAR (MUESTRA AL NUEVO JUGADOR)
        //char *ipCliente=inet_ntoa((con_address[index]).sin_addr);
        //int portCliente=(con_address[index]).sin_port;
        //pthread_mutex_unlock(&(mutex_rcv[index]));    
        recv(sockets[index],buffer,sizeof(char)*200,0);
	while(term!=1){//MIENTRAS EL CLIENTE NO ME DIGA QUE FINALIZO O EL SERVIDOR NO SE DE DE BAJA
	
	        //PIDO MUTEX
	        pthread_mutex_lock(&(mutex_rcv[index]));    
	            printf("entro\n");
               	if(recv(sockets[index],buffer,sizeof(char)*200,0)!=-1){//RECIBIR COORDENADAS DEL CLIENTE
                        printf("recibi [%s]\n",buffer);
                        if(strcmp(buffer,"buscar")==0){
                                setWaitPlayer(index,1);
                                pthread_mutex_unlock(&mutexSDL);//PERMITO DIBUJAR
                                pthread_mutex_unlock(&(mutex_rcv[index]));
                                //LIBERO MUTEX                                
                        }
                        if(strcmp(buffer,"salir")==0){
                                setWaitPlayer(index,0);
                                pthread_mutex_unlock(&mutexSDL);//PERMITO DIBUJAR
                                //LIBERO MUTEX
                                pthread_mutex_unlock(&(mutex_rcv[index]));
                        }
                        if(strcmp(buffer,"exit")==0){
                                term=1;
                        }
			//char bu[200];
			//strcpy(bu,buffer);
			//sprintf(buffer,"eco:%s",bu);
			//send(dataSockets->comm_sockets[indice],buffer,sizeof(char)*200,0);
               	}
        }//SE FINALIZO O POR CLIENTE FINALIZADO (-1) O POR PERDIDA DE CONEXION (-2)
        strcpy(buffer,"exit");
	send(sockets[index],buffer,sizeof(char)*200,0);	
	id_conectados[index]=0;
	pthread_mutex_unlock(&mutexSDL);//PERMITO DIBUJAR
        close(sockets[index]);//CIERRO EL SOCKET
        pthread_exit(indexS);
}

//PROCESO SERVIDOR DE PARTIDA
void newGame(int index_player1, int index_player2,int indexPartida){//RECIBE POR PARAMETROS A LOS DOS JUGADORES
        char buffer[200];
        printf("Entre a newgame\n");
        //BLOQUEA LOS THREADS DE RECEIVE DE CADA JUGADOR
        //pthread_mutex_lock(&(mutex_rcv[index_player1]));
        //pthread_mutex_lock(&(mutex_rcv[index_player2]));
        //SEND A LOS DOS CLIENTES PARA QUE HAGAN UN SEND Y DESBLOQUEEN EL RECEIVE DEL PADRE
        strcpy(buffer,"start");
	send(sockets[index_player1],buffer,sizeof(char)*200,0);
	send(sockets[index_player2],buffer,sizeof(char)*200,0);
        //recv(sockets[index],nick,sizeof(char)*8,0);
        //LOS VUELVE A HABILITAR DESDE LA SIGCHLD
        partidas[indexPartida].pid=fork();        
        if(partidas[indexPartida].pid==0){//POR AHORA VA A SER UN CHAT, RECIBE INFOR DE CLIENTES Y LO REPLICA A LOS DOS
                //INICIO THREADS DE ESCUCHA PARA CADA CLIENTE
		
		printf("empieza la partida loco!!!\n");
		pthread_create(&tidsPartidas[index_player1],NULL,(void*)&communicationPartida,(void*)&index_player1);
		pthread_create(&tidsPartidas[index_player2],NULL,(void*)&communicationPartida,(void*)&index_player2);
		pthread_create(&tidsPartidasSend[indexPartida],NULL,(void*)&communicationPartidaSend,(void*)&indexPartida);
                
		sleep(60);//para darle tiempo a que los clientes hagan algo
                
                //INICIO VARIABLES DE ESCUDOS MATRIZ DE MARCIANOS
        
        	//CUANDO SE DETECTA QUE SALE JUGADOR SE REINICIA TODO JUNTO CON LOS MUTEX
		//pthread_join(tidsPartidas[index_player1], NULL);
        	//pthread_join(tidsPartidas[index_player2], NULL);
		printf("cancelo los threads de partida loco!!!\n");
		pthread_cancel(tidsPartidas[index_player1]);
		pthread_cancel(tidsPartidas[index_player2]);
		pthread_cancel(tidsPartidasSend[indexPartida]);
		
                exit(EXIT_SUCCESS);
        }
        printf("Iniciando game: %d [%d]\n",indexPartida,partidas[indexPartida].pid);
}


//FUNCION PARA ENVIAR(SEND) MATRIZ NAVES ENEMIGAS
void *enviandoNaves(int *indexPartida){
	int indexPartidaencurso = *indexPartida;
	int index_player1 = partidas[indexPartidaencurso].indexPlayer1;
	int index_player2 = partidas[indexPartidaencurso].indexPlayer2;
	int terminarSend = 0;

        while((InfoplayersPartida[index_player1].lives >= 0 && InfoplayersPartida[index_player2].lives >= 0)){
	//MIENTRAS LA PARTIDA NO TERMINE (termina cuando no tiene vidas o termina abrupto)	
        	send(sockets[index_player1],&NavEnemigas,sizeof(NavEnemigas),0);
		send(sockets[index_player2],&NavEnemigas,sizeof(NavEnemigas),0);
                sleep(1);//para darle tiempo al sincronismo del movimiento
              
        }
        pthread_exit(indexPartida);
}


//THREAD DE COMUNICACION PARA RECIBIR 1 POR JUGADOR EN PARTIDA
void *communicationPartida(int *indexS){
	int index_player=*indexS;
	int term=0;
	char buffer[200];//por ahora es chat

	while(term!=1){//MIENTRAS EL CLIENTE NO ME DIGA QUE FINALIZO O EL SERVIDOR NO SE DE DE BAJA
		printf("Estoy en el recv\n");
               	if(recv(sockets[index_player],bufferPartidas,sizeof(char)*200,0)!=-1){
                        printf("recibi estando en partida del cliente[%s]\n",bufferPartidas); //si recibi algo de alguno de los clientes tengo que desbloquear el proceso para hacer el send
			pthread_mutex_unlock(&(mutex_partida[0]));//PERMITO ENVIAR		            	
		}
		
		if(strcmp(bufferPartidas,"salir")==0)
                        term=1;

        }//SE FINALIZO O POR CLIENTE FINALIZADO (-1) O POR PERDIDA DE CONEXION (-2)
        pthread_exit(indexS);
}


//THREAD DE COMUNICACION PARA ENVIAR A LOS 2 JUGADORES EN PARTIDA
void *communicationPartidaSend(int *indexPartida){
	int indexPartidaencurso = *indexPartida;	
	int index_player1 = partidas[indexPartidaencurso].indexPlayer1;
	int index_player2 = partidas[indexPartidaencurso].indexPlayer2;
	int term=0;
	//por ahora es chat      
	while(term!=1){//MIENTRAS EL CLIENTE NO ME DIGA QUE FINALIZO O EL SERVIDOR NO SE DE DE BAJA
		//envio lo que me mando 1 cliente a los 2 	
		pthread_mutex_lock(&(mutex_partida[0]));		
		printf("enviando a los clientes en partida [%s]\n",bufferPartidas);		
		send(sockets[index_player1],bufferPartidas,sizeof(char)*200,0);
		send(sockets[index_player2],bufferPartidas,sizeof(char)*200,0);        	
		
        }//SE FINALIZO O POR CLIENTE FINALIZADO (-1) O POR PERDIDA DE CONEXION (-2)
        pthread_exit(indexPartida);
}

//CANTIDAD DE CONECTADOS
int cantidadCola(){
	int i, cant=0;		
	for(i=0;i<SIZE;i++){
	        if(id_conectados[i]!=0)
	                cant++;
	}
	return cant;
}
//CANTIDAD EN ESPERA
int cantidadWait(int cola[SIZE]){
        int index_=0;
	int i, cant=0;
	for(i=0;i<SIZE;i++){
	        if(status[i]!=0){
	                //printf("est %d index %d\n",status[i],i);
	                cant++;
	                cola[index_++]=i;
	        }
	}
	return cant;
}
//VERIFICA SI LA PARTIDA YA SE JUGO
int check_partida(int index1,int index2){
        int i;
        for(i=0;i<SIZE*SIZE;i++){
                if((partidas[i].indexPlayer1==index1 && partidas[i].indexPlayer2==index2) ||
                (partidas[i].indexPlayer1==index2 && partidas[i].indexPlayer2==index1))
                        return 1;//LO ENCONTRE
        }
        return 0;
}
//SETEA A UN PLAYER EN LISTA DE ESPERA O NO
void setWaitPlayer(int index, int mode){
        status[index]=mode;
}
//BUSCA POR PID Y DEVUELVE UN INDEX DE PARTIDA
int search_pid(int pid){
        int i;
        for(i=0;i<SIZE*SIZE;i++)
                if(partidas[i].pid==pid)
                        return i;
        return -1;
}
//FUNCION QUE DETECTA LA POSICION DEL MOUSE Y COLOREA BOTONES
void check_mouse_position(Uint16 x, Uint16 y, int *status_exit, int *status_stop){
        if(*status_stop==0){//SI NO ESTA EN FOCUS
                if((x>308 && x<557) && (y>92 && y<115)){//SI ESTOY DENTRO
                        setSprite(&btn_stop,BTN_STOP_FOCUS);//SETEO IMAGEN DEL BOTON TERMINAR
                        *status_stop=1;//LO PONGO EN FOCO
                        pthread_mutex_unlock(&mutexSDL);//PERMITO DIBUJAR
                }//SI ESTOY FUERA NO HAGO NADA
        }else{//SI ESTA EN FOCUS
                if(!((x>308 && x<557) && (y>92 && y<115))){//SI NO ESTOY DENTRO
                        setSprite(&btn_stop,BTN_STOP);//SETEO IMAGEN DEL BOTON TERMINAR
                        *status_stop=0;//LO PONGO FUERA DE FOCO
                        pthread_mutex_unlock(&mutexSDL);//PERMITO DIBUJAR
                }//SI ESTOY DENTRO NO HAGO NADA
        }
        if(*status_exit==0){//SI NO ESTA EN FOCUS
                if((x>592 && x<840) && (y>92 && y<115)){//SI ESTOY DENTRO
                        setSprite(&btn_exit,BTN_EXIT_FOCUS);//SETEO IMAGEN DEL BOTON EXIT
                        *status_exit=1;//LO PONGO EN FOCO
                        pthread_mutex_unlock(&mutexSDL);//PERMITO DIBUJAR
                }//SI ESTOY FUERA NO HAGO NADA
        }else{//SI ESTA EN FOCUS
                if(!((x>592 && x<840) && (y>92 && y<115))){//SI NO ESTOY DENTRO
                        setSprite(&btn_exit,BTN_EXIT);//SETEO IMAGEN DEL BOTON EXIT
                        *status_exit=0;//LO PONGO FUERA DE FOCO
                        pthread_mutex_unlock(&mutexSDL);//PERMITO DIBUJAR
                }//SI ESTOY DENTRO NO HAGO NADA
        }
}
//FUNCION PARA VERIFICAR SI YA EXISTE UNA INSTANCIA DE SERVIDOR CORRIENDO
int check_server_run(){
        char buffer[100];FILE *pf;int pid;
        if(validaArchPermExit("servidor.pid")){//SI EL FICHERO PID EXISTE
                pf=fopen("servidor.pid","r");
                fread(&pid,sizeof(int),1,pf);//RECUPERO EL PID DEL FICHERO
                fclose(pf);
                //sprintf(buffer,"bash source/search_process.sh %d %s",ppid.pid,ppid.argv);
                if(!search_process(pid))//BUSCA OTRA INSTANCIA DE SERVIDOR//DE ENCONTRARLO TERMINA LA EJECUCIÓN                        
                        return -1;
        }//SI NO HAY OTRA INSTANCIA GUARDO LA NUEVA
        pf=fopen("servidor.pid","w");
        pid=getpid();
        fwrite(&pid,sizeof(int),1,pf);//GUARDO NUEVO PID Y LA FORMA QUE FUE EJECUTADO
        fclose(pf);
        return 0;
}
//FUNCION PARA INICIAR EL DEMONIO
int daemon_start(){
        //HAY QUE VERIFICAR QUE EL EJECUTABLE DEMONIO EXISTA
        if(!validaArchPermExit("space_invaders_daemon")){
                printf("Error: No se encontro el binario space_invaders_daemon");
                exit(EXIT_FAILURE);
        }
        system("./space_invaders_daemon &");//INICIO EL DEMONIO (ACA HAY QUE PONER EL PATH DONDE IRA FINALMENTE)
        return 0;
}
//FUNCION PARA CREAR EL SOCKET DE ESCUCHA
void createSocket(int puerto){
        char buffer[100];
	struct sockaddr_in listen_address;
	unsigned short int listen_port=0;
	unsigned long int listen_ip_address=0;
	//Creación del socket
        sockets[LISTEN]=socket(AF_INET,SOCK_STREAM,0); 
	if(sockets[LISTEN]==-1){
                printf("No se pudo crear el socket de escucha\n");
                exit(EXIT_FAILURE);	
	}
        //Asigno dirección y puerto al socket a escuchar
        bzero(&listen_address,sizeof(listen_address));
        listen_address.sin_family=AF_INET; 
        listen_port=htons(puerto);//INDICO PUERTO
        listen_address.sin_port=listen_port;
        listen_ip_address=htonl(INADDR_ANY);
        listen_address.sin_addr.s_addr=listen_ip_address;
        bzero(&(listen_address.sin_zero),8);
        bind(sockets[LISTEN],(struct sockaddr *)&listen_address,sizeof(struct sockaddr));
        //Socket de escucha
        listen(sockets[LISTEN],10);
        printf("Se ha creado el socket de escucha\n");
}
//FUNCION PARA OBTENER LA FECHA
char* getFecha (){
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char* fechahora = (char*) malloc(25);
	sprintf(fechahora,"%02d/%02d/%d %d:%02d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min);
	return fechahora;
}
/*BLOQUEA LAS SEÑALES*/
void setSignal(){
	/*******************SEÑALES*****************/
	sigset_t conjsen; // El conjunto de señales.
	struct sigaction sa;
	//Se preparan las señales para ser bloqueadas
	sigemptyset(&conjsen);
	int _signal;
	for(_signal=1;_signal<65;_signal++)
		if(_signal!=SIGCHLD)
			sigaddset(&conjsen, _signal);
	//Se bloquean las señales menos la SIGCHLD
	//Se agregan las señales al manejador de señales
	sigprocmask(SIG_SETMASK, &conjsen, NULL);
	sigfillset(&conjsen);
	sa.sa_mask=conjsen;
	sa.sa_flags=SA_SIGINFO;
	sa.sa_sigaction=&manejar_senal;
	sigaction(SIGCHLD, &sa, NULL);
}
//BUSCA UN PROCESO ACTIVO POR PID Y PS
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
//FUNCION PARA CREAR SEMAFOROS
void creandoSemaforos(){
        semFicheroMutex = crearSemaforo("semFicheroMutex",1);       
}
//FUNCION LIBERADORA DE SEMAFOROS
void liberandoSemaforos(){
        procesoLiberaSemaforo(semFicheroMutex);
}
//FUNCION ELIMINADORA DE SEMAFOROS
void eliminandoSemaforos(){
        eliminarSemaforo("semFicheroMutex");
}
void crearMemoriaCompartida(){
        create_and_map((void *)&sockets,1+sizeof(int)*SIZE,"/memorySockets");//MEMORIA SOCKETS//SERVIDOR Y DEMONIO
	create_and_map((void *)&memory_players,sizeof(Player)*SIZE,"/memoryPlayers");
}
//ELIMINA MAPEO Y MEMORIA COMPARTIDA
void eliminarMemoriaCompartida(){
        unmap_and_delete((void*)sockets,1+sizeof(int)*SIZE,"/memorySockets");
	unmap_and_delete((void*)memory_players,sizeof(Player)*SIZE,"/memoryPlayers");
}
