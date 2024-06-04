#ifndef _SDL
#define _SDL
#include<SDL/SDL.h>
#include<SDL/SDL_ttf.h>
#define BTN_STOP 0
#define BTN_STOP_FOCUS 1
#define BTN_STOP_PRESS 2
#define BTN_EXIT 3
#define BTN_EXIT_FOCUS 4
#define UNION 5
#define DEFAULT 0
#define BOX 1
#define POSITION 2
#define BOTH 3


//// CONTROLA LOS EVENTOS ////
SDL_Event event;

//ESTRUCTURA PARA MANEJAR LOS OBJETOS VISIBLES
typedef struct{
        SDL_Rect position;
        SDL_Rect box;
        int visible;
}Frame;

typedef struct{
        SDL_Surface *imageText;
        SDL_Rect position;
}FrameText;

//// VARIABLES PARA EL DIBUJO ////
SDL_Surface *SDLBuffer,*SDLSprites,*SDLScreen;  // EL BUFFER VA A SER EL LIENZO DONDE SE DIBUJARAN LOS DEMAS OBJETOS
Frame frameNave,frameBala;             // NAVE Y BALA QUE SE DIBUJARAN SOBRE EL BUFFER
Frame game_union;
int tamFuente;
Frame btn_exit,btn_stop;
FrameText ttext;
TTF_Font *fuente;
SDL_Color fuenteColor;
SDL_Color screenColor;

void SDLRun(int x, int y, int bits, char* title);
void showScreen();
void addScreen(Frame *frame,int mode);
void setPosition(Frame *frame, int x, int y);
void setBox(Frame *frame, int x, int y, int w, int h);
void setSprite(Frame *frame, int nave);
void SDLWrite(char *string, int x,int y);
void SDLClear();

#endif
