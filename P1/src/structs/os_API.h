#include <stdbool.h>
#include <stdlib.h>
#include "structs.h"


// Funciones Generales

void os_mount(char* path, int holi);

void os_bitmap(unsigned int holi);

bool os_exists(char* filename);

IndexBlock* indexblock_init(FILE* disk, uint64_t inicio_directorio /* REVISAREIS*/);

void os_ls();

// Funciones MBT

void os_mbt();

void os_create_partition(int id, int size);

void write_new_partition(int id, int location, int size);

void create_bitmap(int id);

void os_delete_partition(int delete_id);

void os_reset_mbt();

// Funciones de manejo de archivos

osFile* os_open(char* filename, char mode);

int os_read(osFile* file_desc, void* buffer, int nbytes);

void os_write(osFile* file_desc, void* buffer, int nbytes);

int os_close(osFile* file_desc);

void os_rm(char* filename);
