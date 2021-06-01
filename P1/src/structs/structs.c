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
void init_mbt(FILE* disk){
  *mbt = (Mbt) {
    .entry_quantity = 128,
    .entry_container = calloc(128, sizeof(Entry**)),
  };

  uint8_t buffer[8];
  // Coloca el puntero del disco al inicio
  fseek(disk, 0, SEEK_SET);
          
  for (int entry_id = 0; entry_id < mbt->entry_quantity; entry_id++) {

    size_t ret = fread(buffer, sizeof(uint8_t), (size_t)8, disk);
    // fseek con constantes SEEK_SET, SEEK_END, SEEK_CUR.
    // fwrite y fread para leer y escribir bytes.

    // leer 1 byte
    bool is_valid = bt_get(buffer, 0);
    uint8_t id = get_partition_id(buffer[0]);

    // leer 3 bytes
    uint32_t location = last_3_bytes_of_4(buffer);

    // leer 4 bytes
    uint32_t size = int_from_4_bytes(buffer+4);
    // printf("%d %" PRIu8 " %" PRIu32 " %" PRIu32 "\n", is_valid, id, location, size);

    if (is_valid){
      Entry* entry = init_entry(is_valid, id, location, size);
      mbt->entry_container[id] = entry;
    }
  }
};

void destroy_mbt()
{
  if (mbt)
  {
    for (int i = 0; i < mbt->entry_quantity; i++)
      if (mbt->entry_container[i])
        free(mbt->entry_container[i]);
    free(mbt->entry_container);
    free(mbt);
  }
}

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

// Funciones para Temporal Entry

TEntry* init_tentry(TEntry* tentry, uint32_t location, uint32_t size){
  TEntry* new = malloc(sizeof(TEntry));
  *new = (TEntry) {
    .location = location,
    .size = size,
    .next = NULL
  };
  if (tentry){
    if (tentry->location > new->location){
      new->next = tentry;
      tentry = new;
    }
    else{
      TEntry* current = tentry;
      while (current){
        if (current->next){
          if (current->next->location > new->location){
            new->next = current->next;
            current->next = new;
            return tentry;
          }
        }
        else{
          current->next = new;
          return tentry;
        }
        current = current->next;
      }
    }
  }
  return new;
}

void destroy_tentry(TEntry* tentry)
{
  TEntry* next = tentry -> next;
  if (next)
  {
    destroy_tentry(next);
  }
  free(tentry);
}

// Funcion para transformar el contenido del testdisk del Pablo
int hex_to_int(char* input){
  return (int)strtol(input, NULL, 16);
};