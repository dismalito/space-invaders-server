#include"_SDL.h"

//FUNCION PARA INICIAR SDL Y LA VENTANA
void SDLRun(int x, int y, int bits, char* title){
        if(SDL_Init(SDL_INIT_VIDEO)<0){printf("No se pudo inicializar SDL: %s\n",SDL_GetError());exit(EXIT_FAILURE);}
        //INICIALIZO FUENTES Y ABRO ORIGEN
        TTF_Init();
        tamFuente = 12;
        fuente=TTF_OpenFont("resources/cour.ttf",tamFuente);
        //SETEO COLOR DE LA FUENTE
        fuenteColor.r = 237;
        fuenteColor.g = 28;
        fuenteColor.b = 36;
        screenColor.r=0;
        screenColor.g=0;
        screenColor.b=0;
       
        //// CARGO LAS IMAGENES ////
        SDLSprites = SDL_LoadBMP("resources/sprites.bmp");//NAVES
        SDLScreen = SDL_LoadBMP("resources/screen2.bmp");//FONDO

        //SETEO EL COLOR TRANSPARENTE
        SDL_SetColorKey(SDLScreen,SDL_SRCCOLORKEY|SDL_RLEACCEL,SDL_MapRGB(SDLScreen->format,255,0,255));
        SDL_SetColorKey(SDLSprites,SDL_SRCCOLORKEY|SDL_RLEACCEL,SDL_MapRGB(SDLSprites->format,255,0,255));
        
        //// CREO LA VENTANA CON LAS DIMENSIONES DADAS ////
        SDLBuffer = SDL_SetVideoMode(x, y, bits, SDL_HWSURFACE );
        //// VERIFICO SI SE PUDO CREAR LA VENTANA ////
        if(SDLBuffer==NULL){printf("Error al entrar a modo grafico: %s\n",SDL_GetError());SDL_Quit();exit(EXIT_FAILURE);}
        //// SETEO EL TITULO DE VENTANA ////
        SDL_WM_SetCaption(title, title);
        setSprite(&game_union,UNION);
}

//PASA LO DEL BUFFER A LA VENTANA
void showScreen(){
        //// DIBUJO LO QUE HAY EN EL BUFFER ////
        SDL_Flip(SDLBuffer);
}

//AGREGA UN FRAME AL BUFFER
//DEFAULT:  AGREGA EL FRAME COMPLETO SOBRE EL BUFFER DESDE EL (0,0)
//BOX:      AGREGA UN TROZO DEL FRAME DADO POR frame.box SOBRE EL BUFFER DESDE EL (0,0)
//POSITION: AGREGA EL FRAME COMPLETO SOBRE EL BUFFER DESDE LA POSICION DADA POR frame.position
//BOTH:     AGREGA UN TROZO DEL FRAME DADO POR frame.box SOBRE EL BUFFER DESDE LA POSICION DADA POR frame.position
//ASCII:    AGREGA FRAMES DE LETRAS
void addScreen(Frame *frame,int mode){
        switch (mode){
                case DEFAULT:   SDL_BlitSurface(SDLScreen,NULL,SDLBuffer,NULL);break;
                case BOX:       SDL_BlitSurface(SDLSprites,&(*frame).box,SDLBuffer,NULL);break;
                case POSITION:  SDL_BlitSurface(SDLSprites,NULL,SDLBuffer,&(*frame).position);break;
                case BOTH:      SDL_BlitSurface(SDLSprites,&(*frame).box,SDLBuffer,&(*frame).position);break;
                default:        break;
        }
}

//SETEA LA POSICION DEL FRAME
void setPosition(Frame *frame, int x, int y){(*frame).position.x=x;(*frame).position.y=y;}
//SETEA EL TROZO DEL FRAME
void setBox(Frame *frame, int x, int y, int w, int h){(*frame).box.x=x;(*frame).box.y=y;(*frame).box.w=w;(*frame).box.h=h;}

//SETEA LOS BOX DE LOS SPRITES
void setSprite(Frame *frame, int nave){
        switch (nave){
                case BTN_STOP:           setBox(frame,0,0,249,23);break;
                case BTN_STOP_FOCUS:     setBox(frame,0,23,249,23);break;
                case BTN_STOP_PRESS:     setBox(frame,0,46,249,23);break;
                case BTN_EXIT:           setBox(frame,0,69,249,23);break;
                case BTN_EXIT_FOCUS:     setBox(frame,0,92,249,23);break;
                case UNION:              setBox(frame,0,115,22,19);break;                
                default: break;
        }
}


//LIMPIA LA PANTALLA
void SDLClear(){
        SDL_Rect PosScr;
        PosScr.x = 0;
        PosScr.y = 0;
        PosScr.w = SDLBuffer->w;
        PosScr.h = SDLBuffer->h;
        SDL_FillRect(SDLBuffer, &PosScr, SDL_MapRGB(SDLBuffer->format,0, 0, 0));
}
