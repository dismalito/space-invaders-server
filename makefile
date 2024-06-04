all: space_invaders_daemon space_invaders_server main
CC = gcc
LD = gcc
LIBRARYFOLDER = library
OBJECTFOLDER = object
FUNCTIONS = $(LIBRARYFOLDER)/_functions
SEMAPHORE = $(LIBRARYFOLDER)/_semaphore
SERVER = source/space_invaders_server
DAEMON = source/space_invaders_daemon
MEMORY = $(LIBRARYFOLDER)/_shared_memory
VALIDATIONS = $(LIBRARYFOLDER)/_validations
SDL = $(LIBRARYFOLDER)/_SDL

OBJFUNCTIONS = $(OBJECTFOLDER)/_functions.o
OBJSEMAPHORE = $(OBJECTFOLDER)/_semaphore.o
OBJMEMORY = $(OBJECTFOLDER)/_shared_memory.o
OBJVALIDATIONS = $(OBJECTFOLDER)/_validations.o
OBJSERVER = $(OBJECTFOLDER)/space_invaders_server.o
OBJDAEMON = $(OBJECTFOLDER)/space_invaders_daemon.o
OBJSDL = $(OBJECTFOLDER)/_SDL.o

OBJECTS = $(OBJSERVER) $(OBJSDL) $(OBJFUNCTIONS) $(OBJSEMAPHORE) $(OBJMEMORY) $(OBJVALIDATIONS)
SOURCE = $(SERVER).c $(SDL).c $(FUNCTIONS).c $(SEMAPHORE).c $(MEMORY).c $(VALIDATIONS).c
CFLAGS = -c
LFLAGS = -lpthread -lrt -lSDL -lSDL_ttf

main:
	echo "Compilacion Terminada"

space_invaders_daemon: $(OBJECTFOLDER)/space_invaders_daemon.o
	$(LD) $(OBJDAEMON) -o space_invaders_daemon $(LFLAGS) #aca deberiamos setear donde guardar el ejecutable del demonio.. /TMP
	
object/space_invaders_daemon.o: source/space_invaders_daemon.c
	$(CC) $(CFLAGS) $(DAEMON).c -o $(OBJDAEMON) $(LFLAGS)
	
space_invaders_server: $(OBJECTS)
	$(LD) $(OBJECTS) -o space_invaders_server $(LFLAGS)

object/space_invaders_server.o: $(SOURCE)
	$(CC) $(CFLAGS) $(SERVER).c -o $(OBJSERVER) $(LFLAGS)

object/_SDL.o: $(SDL).c
	$(CC) $(CFLAGS) $(SDL).c -o $(OBJSDL) -lSDL -lSDL_ttf

object/_functions.o: $(FUNCTIONS).c $(SEMAPHORE).c $(SEMAPHORE).c $(VALIDATIONS).c
	$(CC) $(CFLAGS) $(FUNCTIONS).c -o $(OBJFUNCTIONS) $(LFLAGS)

object/_semaphore.o: $(SEMAPHORE).c
	$(CC) $(CFLAGS) $(SEMAPHORE).c -o $(OBJSEMAPHORE) $(LFLAGS)

object/_shared_memory.o: $(MEMORY).c
	$(CC) $(CFLAGS) $(MEMORY).c -o $(OBJMEMORY) $(LFLAGS)
	
object/_validations.o: $(VALIDATIONS).c
	$(CC) $(CFLAGS) $(VALIDATIONS).c -o $(OBJVALIDATIONS) $(LFLAGS)
	
.PHONY : clean
clean:
	rm space_invaders_server $(OBJECTFOLDER)/*.o space_invaders_daemon *.pid
