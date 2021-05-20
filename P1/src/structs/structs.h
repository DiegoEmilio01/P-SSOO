

enum os_error{
  invalid_mount_partition,
  invalid_delete_partition,
  invalid_read_file,
  invalid_delete_file,
  invalid_read_bitmap,
  invalid_file_name_size, // Usar para input vacio o muy largo
  full_disk
};

void os_strerror(enum os_error error);

typedef struct mbt
{
  int entry_quantity; // 128
  Entry* entry_container;  
} Mbt;

Mbt* init_mbt(FILE* file);


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
  int is_valid;
  // id de la partición
  int id;
  // posición absoluta del primer bloque de la partición
  int location;
  // cantidad de bloques de la partición
  int size;
  //uint8_t first_byte; // 1 bit Validez // 7 identificador único
  //Directory* directory_block; 
  //uint32_t block_partition_quantity; // Últimos 4 bytes
} Entry;

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

Mbt* create_mbt();