#include <stdbool.h>
#include "structs.h"


// Funciones Generales

Mbt* os_mount(char*, int);

void os_bitmap(unsigned int);

bool os_exists();

void os_ls();

// Funciones MBT

void os_mbt();

void os_create_partition();

void os_delete_partition();

void os_reset_mbt();

// Funciones de manejo de archivos

void os_open();

void os_read();

void os_write();

void os_close();

void os_rm();