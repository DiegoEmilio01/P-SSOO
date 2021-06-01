#include <stdbool.h>
#include <stdint.h>
#pragma once

enum os_error{
  invalid_mount_partition,
  invalid_delete_partition,
  invalid_read_file,
  invalid_delete_file,
  invalid_read_bitmap,
  invalid_file_name_size, // Usar para input vacio o muy largo
  full_disk,
  mbt_init_error
};

void os_strerror(enum os_error error);

/*
Cada entrada lleva:
  - Tamaño de entrada 8 Bytes
  - Bit de validez (Primer bit)
  - Identificador único de partición (7 bits)
  - Identificador absoluto (3 Bytes) --> Bloque directorio
  - Cantidad de bloques de partición (16384 - 131072)
*/
typedef struct entry
{
  // 0: partición invalida, 1: partición válida
  bool is_valid;
  // id de la partición
  uint8_t id;
  // posición absoluta del primer bloque de la partición
  uint32_t location;
  // cantidad de bloques de la partición
  uint32_t size;
  //uint8_t first_byte; // 1 bit Validez // 7 identificador único
  //Directory* directory_block; 
  //uint32_t block_partition_quantity; // Últimos 4 bytes
} Entry;

Entry* init_entry(bool is_valid, uint8_t id, uint32_t location, uint32_t size);

typedef struct temporal_entry
{
  uint32_t location;
  uint32_t size;
  struct temporal_entry* next;
} TEntry;

TEntry* init_tentry(TEntry* tentry, uint32_t location, uint32_t size);
void destroy_tentry(TEntry* tentry);

typedef struct mbt
{
  int entry_quantity; // 128
  Entry** entry_container;  
} Mbt;

void init_mbt(FILE* file);
void destroy_mbt();


typedef struct directory
{
	/* data */
  int is_valid; // 0 o 1
  int relative_index;
  char* filename; // ej: archivo.txt, máximo 28 bytes (ASCII). Se rellena con 0x00
} Directory;

typedef struct bitmap
{
  // será necesario?
} Bitmap;

typedef struct osfile{

} osFile;


int hex_to_int(char* input);
