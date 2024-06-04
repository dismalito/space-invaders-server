CC = gcc
LD = gcc
OBJETOS = cliente.o funciones.o
CFLAGS = -c
LFLAGS = -lpthread

cliente: $(OBJETOS)
	$(LD) $(OBJETOS) -o cliente $(LFLAGS)

cliente.o: cliente.c funciones.c
	$(CC) $(CFLAGS) cliente.c -o cliente.o $(LFLAGS)

funciones.o: funciones.c
	$(CC) $(CFLAGS) funciones.c -o funciones.o $(LFLAGS)

.PHONY : limpiar
limpiar:
	rm *.o cliente
