#include <stdbool.h>
#include <stdlib.h>
#include "structs.h"


// Funciones Generales

Mbt* os_mount(char*, int);

void os_bitmap(unsigned int);

bool os_exists(char* filename);

void os_ls();

// Funciones MBT

void os_mbt();

void os_create_partition(int id, int size);

void os_delete_partition(int id);

void os_reset_mbt();

// Funciones de manejo de archivos

void os_open(char* filename, char mode);

void os_read(osFile* file_desc, void* buffer, int nbytes);

void os_write(osFile* file_desc, void* buffer, int nbytes);

void os_close(osFile* file_desc);

void os_rm(char* filename);