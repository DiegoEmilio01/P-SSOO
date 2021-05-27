#include <stdio.h>
#include <stdlib.h>

#include "structs.h"
#include "../osfs/main.h"
#include "../util/bits.h"
#include <inttypes.h>

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
  printf("Llegue a init_mb\n");
  // leer archivo (;´∀｀)
  uint8_t buffer[8];
  // int xd0 = fseek(disk, 0L, SEEK_SET); 

  for (int entry_id = 0; entry_id < mbt->entry_quantity; entry_id++) {
    
    // TODO: leer archivo
    // 8 bytes (1 byte-> is_valid + id, 2-4 -> location, 5-8 -> size)
    // * Cosas para leer archivo
    // fijamos el puntero del archivo al inicio del mismo
    //printf("Llegue a 2\n");


    //printf("Llegue a 3\n");
    // int fseek(FILE *stream, long int offset, int whence)funca, aca es
    printf("\ntell-bef: %ld ; ", ftell(disk));
    size_t xd1 = fread(buffer, sizeof(uint8_t), 8, disk); // FIXME: Si n
    printf("tell-aft: %ld\n", ftell(disk));
    for (int i=0; i<8; i++){
      for (int j=0; j<8; j++)
        printf("%d", bt_get(buffer, i*8+j));
      printf(" ");
    }
    printf("\n");
    
  
    
    // fseek con constantes SEEK_SET, SEEK_END, SEEK_CUR.
    // fwrite y fread para leer y escribir bytes.

    //leer1byte
    // FIXME: como se castea el 0 a uint8 ??
    // uint8_t bitarray1[] = {0};
    bool is_valid = bt_get(buffer, 0);
    uint8_t id = get_partition_id(buffer[0]);

    //leer3bytes
    // uint8_t bitarray2[] = {0, 0, 1, 2};
    uint32_t location = last_3_bytes_of_4(buffer);

    //leer4bytes
    // uint8_t bitarray3[] = {0, 0, 0, 0};
    uint32_t size = int_from_4_bytes(buffer+4);
    printf("%d %" PRIu8 " %" PRIu32 " %" PRIu32 "\n", is_valid, id, location, size);

    if (is_valid){
      Entry* entry = init_entry(is_valid, id, location, size);
      mbt->entry_container[id] = entry;
    }
    // int xd2 = fseek(disk, 8L, SEEK_CUR);
  }
  printf("sali\n");
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