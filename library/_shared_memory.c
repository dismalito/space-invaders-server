#include"_shared_memory.h"

void create_and_map(void **addr,int size,char*name){
        if((size=shm_open (name,O_RDWR|O_CREAT,0666)) == -1)
                perror ("error in shm_open\n");
        if(ftruncate(size,100) == -1)
                perror ("error in ftruncate\n");
        if((*addr=mmap(0,100,PROT_READ|PROT_WRITE,MAP_SHARED,size,0))== MAP_FAILED)
                perror ("error in mmap\n");
}

void open_and_map(void **addr,int size,char*name){
        if((size=shm_open (name,O_RDWR,0666)) == -1)
                perror ("error in shm_open\n");
        if(ftruncate(size,100) == -1)//tamaño de la memoria
                perror ("error in ftruncate\n");
        if((*addr=mmap(0,100,PROT_READ|PROT_WRITE,MAP_SHARED,size,0))== MAP_FAILED)
                perror ("error in mmap\n");
}

void unmap_and_delete (void *addr,int size,char*name){
        if(munmap(addr,100)== -1)
                perror ("error in munmap\n");
        if(shm_unlink(name) == -1)
                if (errno != ENOENT)
                        perror("error in shm_unlink\n");
}
