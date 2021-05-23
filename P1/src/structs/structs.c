#include "stdio.h"
#include "structs.h"
#include "../util/bits.c"

void os_strerror(enum os_error error){
  switch (error)
  {
  case invalid_mount_partition:
    printf("ERROR: Invalid Mount Partition. La Partición seleccionada para montar es inválida.\n");
    break;
  case invalid_delete_partition:
    printf("ERROR: Invalid Delete Partition. La Partición seleccionada para borrar es inválida.\n");
    break;
  case invalid_read_file:
    printf("ERROR: Invalid Read File. El Archivo seleccionado para leer es inválido.\n");
    break;
  case invalid_delete_file:
    printf("ERROR: Invalid Delete File. El Archivo seleccionado para borrar es inválido.\n");
    break;
  case invalid_read_bitmap:
    printf("ERROR: Invalid Read Bitmap. El Bitmap seleccionado para leer es inválido.\n");
    break;
  case invalid_file_name_size:
    printf("ERROR: Invalid File Name Size. El Nombre del Archivo a escribir tiene un tamaño inválido. Debe ser entre 1 y 20 caracteres.\n");
    break;
  case full_disk:
    printf("ERROR: Disco lleno. En este estado no se pueden crean nuevas particiones ni escribir más archivos.\n");
    break;
  case mbt_init_error:
    printf("ERROR: Error al leer la mbt, el largo no corresponde al tamaño del buffer.\n");
    break;
  default:
    break;
  }
};


/** Crea y retorna la MasterBootTable

 **/
Mbt* init_mbt(FILE* disk){
  Mbt* mbt = malloc(sizeof(Mbt));
  *mbt = (Mbt) {
    .entry_quantity = 128,
    .entry_container = malloc(128*sizeof(Entry**)),
  };

  // leer archivo (;´∀｀)
  for (int entry_id = 0; entry_id < mbt->entry_quantity; entry_id++) {
    // int fseek(FILE *stream, long int offset, int whence)
    // TODO: leer archivo
    // 8 bytes (1 byte-> is_valid + id, 2-4 -> location, 5-8 -> size)

    // * Cosas para leer archivo
    // fijamos el puntero del archivo al inicio del mismo
    int xd = fseek(disk, 0, SEEK_SET);
    char buffer[1024];

    size_t xd2 = fread((void*) buffer, 1, 1024, disk); // FIXME: Si no funca, aca es
    // fseek con constantes SEEK_SET, SEEK_END, SEEK_CUR.
    // fwrite y fread para leer y escribir bytes.

    //leer1byte
    // FIXME: como se castea el 0 a uint8 ??
    uint8_t bitarray1[] = {0};
    bool is_valid = bt_get(bitarray1, 0);
    uint8_t id = get_partition_id(bitarray1[0]);

    //leer3bytes
    uint8_t bitarray2[] = {0, 0, 1, 2};
    uint32_t location = last_3_bytes_of_4(bitarray2);

    //leer4bytes
    uint8_t bitarray3[] = {0, 0, 0, 0};
    uint32_t size = read_4_bytes(bitarray3);

    Entry* entry = init_entry(is_valid, id, location, size);
    mbt->entry_container[entry_id] = entry;
  };
  return mbt;
};

Entry* init_entry(bool is_valid, uint8_t id, uint32_t location, uint32_t size){
  Entry* entry = malloc(sizeof(Entry));
  *entry = (Entry) {
    .is_valid = is_valid,
    .id = id,
    .location = location,
    .size = size
  };
  return entry;
}

// Funcion para transformar el contenido del testdisk del Pablo
int hex_to_int(char* input){
  return (int)strtol(input, NULL, 16);
};