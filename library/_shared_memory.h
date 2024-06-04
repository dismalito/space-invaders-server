#ifndef _shared_memory
#define _shared_memory
#include <sys/mman.h>
#include <fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>

void create_and_map(void **addr,int size,char*name);
void open_and_map(void **addr,int size,char*name);
void unmap_and_delete (void *addr,int size,char*name);

#endif
