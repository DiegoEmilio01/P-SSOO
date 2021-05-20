#include "stdio.h"
#include "structs.h"

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
  default:
    break;
  }
};


/** Crea y retorna la MasterBootTable

 **/
Mbt* init_mbt(FILE* file){
  Mbt* mbt = malloc(sizeof(Mbt));
  *mbt = (Mbt) {
    .entry_quantity = 128,
    .entry_container = malloc(128*sizeof(Entry*)),
  };

  char* ඞ = "sus";
  // leer archivo (;´∀｀)
  for (int entry_id = 0; entry_id < mbt->entry_quantity; entry_id+8) // maybe +8
  {
    leer1byte
    leer3bytes
    leer4bytes
  };
  return mbt;
};