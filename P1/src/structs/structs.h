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

typedef struct directoy_entry
{
  /* data */
  int is_valid; // 0 o 1
  int relative_index; // 3 Bytes
  char* filename; // ej: archivo.txt, máximo 28 bytes (ASCII). Se rellena con 0x00
} DirectoryEntry;

typedef struct directory
{
	int entry_quantity; //Deberían ser 64 entradas
  DirectoryEntry *entries; 
} Directory;


typedef struct bitmap
{
  // será necesario?
} Bitmap;


typedef struct data_block //Siempre manejado en BigEndian
{
  unsigned char *array_bytes;
  int *array_bits;
  int entry_quantity;
} DataBlock;

 //Siempre manejado en BigEndian
typedef struct index_block
{
  unsigned long int file_size; // 5 Bytes para el tamaño del archivo (unsigned aguanta hasta 8).
  int *punteros; // 2043 Bytes (Arreglo para 681 punteros) Apuntan a los bloques de datos.
  unsigned char *array_bytes;
  int *array_bits;
  int partition; // Partición a la que corresponde
  int entry_quantity;
}IndexBlock;

typedef struct osfile{ // Representa un archivo abierto
  char* filename;
  unsigned long int file_size;
  unsigned int read_index; // Posición desde donde leer sus bytes
  Directory *relative_index; // Relacion con su directorio
  DataBlock *data_blocks; // Contenedor de punteros de los bloques con la información del archivo
  IndexBlock *index_block; // Bloque índice con la metadata
} osFile;



Directory* directory_init(FILE* disk, int posicion_particion);
void print_file(osFile* file);
int hex_to_int(char* input);
