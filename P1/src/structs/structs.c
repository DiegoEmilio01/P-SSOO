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

    fread(buffer, sizeof(uint8_t), (size_t)8, disk);
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

/*
  unsigned long int file_size; // 5 Bytes para el tamaño del archivo (unsigned aguanta hasta 8).
  int *punteros; // 2043 Bytes (Arreglo para 681 punteros) Apuntan a los bloques de datos.
  unsigned char *array_bytes;
  int *array_bits;
  int partition;
*/
IndexBlock* indexblock_init(FILE* disk, uint64_t posicion_indexblock){ 
  // Nos falta agregar la partición a la que pertenece
  IndexBlock *bloque_retorno = malloc(sizeof(IndexBlock));  
  bloque_retorno->entry_quantity = 681;
  bloque_retorno->punteros = calloc(681, sizeof(uint32_t)); //Hacemos espacio para los 681 bloques
  
  uint8_t buffer_size[5];
  fseek(disk, posicion_indexblock, SEEK_SET);
  fread(buffer_size, sizeof(uint8_t), (size_t)5, disk);
  printf("file size (RAW): %d %d %d %d %d\n", buffer_size[0], buffer_size[1], buffer_size[2], buffer_size[3], buffer_size[4]);
  bloque_retorno->file_size = get_index_size(buffer_size);
  printf("file size: %ld\n", bloque_retorno->file_size);

  uint8_t buffer_pointer[3];
  // Ahora procedemos a rellenar los bloques de datos
  for (int bloque_actual = 0; bloque_actual < 681; bloque_actual++)
  { // Cada bloque hace hasta 2043 Bytes
    //Vamos leyendo de a 3 bytes para rellenar cada bloque
    fread(buffer_pointer, sizeof(uint8_t), (size_t)3, disk);
    bloque_retorno->punteros[bloque_actual] = three_bytes_to_int(buffer_pointer);
    // printf("pos %d >> %d\n", bloque_actual, bloque_retorno->punteros[bloque_actual]);
  }
  //Ahora nuestro IndexBlock tiene los punteros a los DataBlocks, solo falta llenarlos.
  //El index block 
  return bloque_retorno;
}



void datablocks_init(FILE* disk, osFile* osfile){
  int cantidad_bloques = (osfile->index_block->file_size % 2048 ? 1 : 0) + osfile->index_block->file_size / 2048 ; //Cada bloque es de 2048 bytes
  osfile->data_blocks = malloc(cantidad_bloques*sizeof(DataBlock));
  printf("CANTIDAD DE BLOQUES %d\nSIZE %ld\n", cantidad_bloques, osfile->index_block->file_size);
  int bloque_actual = 0;
  // esto está cursed, pq 2 variables?
  for (int puntero_actual = 0; puntero_actual < cantidad_bloques; puntero_actual++)
  {
    //printf("PUNTERO %d \n", 1024 + osfile->index_block->punteros[puntero_actual] * 2048);
    fseek(disk, 1024 + osfile->index_block->punteros[puntero_actual] * 2048 + mbt->entry_container[partition]->location * 2048 , SEEK_SET); //Movemos el puntero a un bloque específico para que sea leído y guardado
    osfile->data_blocks[bloque_actual].array_bytes = malloc(2048*sizeof(uint8_t));
    osfile->data_blocks[bloque_actual].entry_quantity = 0;
    fread(osfile->data_blocks[bloque_actual].array_bytes, sizeof(uint8_t), (size_t)2048, disk);

    
    
    bloque_actual++;
  }
  printf("NOMBRE A IMPRIMIR %s\n", osfile->filename);
  FILE* test_out = fopen(osfile->filename, "wb");
  bloque_actual = 0;
  int bytes_restantes = osfile->index_block->file_size;
  for (int uwu = 0; uwu < cantidad_bloques; uwu++)
  {
    //printf("PUNTERO %d \n", osfile->index_block->punteros[0]);
    fwrite(osfile->data_blocks[bloque_actual].array_bytes, sizeof(uint8_t), 
    (size_t)(bytes_restantes < 2048 ? bytes_restantes : 2048), test_out);
    bloque_actual += 1;
    if (!bytes_restantes) break;
  }
  fclose(test_out);
  
  
}


Directory* directory_init(FILE* disk, int posicion_particion){ // Falta completar
  Directory *bloque_retorno = malloc(sizeof(Directory));
  bloque_retorno->entry_quantity = 64;
  bloque_retorno->entries = malloc(64*sizeof(DirectoryEntry));
  uint8_t buffer[4];
  char char_buffer[29]; // recordar char vacio, chantar '\0' al final 
  // Coloca el puntero del disco al inicio
  fseek(disk, posicion_particion, SEEK_SET);
  // Leemos el primer bloque y obtenemos las entradas para el Directory
  for (int entrada_rellenada = 0; entrada_rellenada < 64; entrada_rellenada++) {
    
    fread(buffer, sizeof(uint8_t), (size_t)4, disk);
    // leer 1 byte
    bloque_retorno->entries[entrada_rellenada].is_valid = bt_get(buffer, 7);
    //printf("relative index %d %d %d\n", buffer[1], buffer[2],buffer[3]);
    bloque_retorno->entries[entrada_rellenada].relative_index = last_3_bytes_of_4(buffer);
    //printf("relative index %d\n", bloque_retorno->entries[entrada_rellenada].relative_index);
    fread(char_buffer, sizeof(char), (size_t)28, disk);
    char_buffer[28] = '\0';  /* chantar '\0' al final */
    
    bloque_retorno->entries[entrada_rellenada].filename = malloc(29*sizeof(char));
    strcpy(bloque_retorno->entries[entrada_rellenada].filename, char_buffer);
    //Hasta acá hacemos 32, en la siguiente iteración
  }
  return bloque_retorno;
}

void destroy_directory(Directory *bloque_directory){
  for (int entrada_rellenada = 0; entrada_rellenada < 64; entrada_rellenada++) {
    free(bloque_directory->entries[entrada_rellenada].filename);
  }
  free(bloque_directory->entries);
  free(bloque_directory);
}

void print_file(osFile* file){
  printf("-------------------------------------------------\n");
  printf("Nombre archivo: %s  Tamaño archivo: %lu Bytes  \n", file -> filename, file -> file_size);
  //printf("Read index: %lu\n", file->relative_index->entry_quantity);
  printf("-------------------------------------------------\n");
}

Bitmap* init_bitmap(){
  int bitmap_blocks = mbt->entry_container[partition]->size / 16384;
  if (mbt->entry_container[partition]->size % 16384 != 0){
    bitmap_blocks ++;
  }

  int inicio_bitmap = (mbt->entry_container[partition]->location)*2048 + 1024 + 2048;
  FILE* disk = fopen(path_disk, "r+b");
  fseek(disk, 0, inicio_bitmap);
  
  Bitmap* bitmap = malloc(sizeof(Bitmap));
  bitmap->n_blocks = bitmap_blocks;
  bitmap->start = inicio_bitmap;
  bitmap->empty_blocks = 0;
  bitmap->full_blocks = 0;
  bitmap->bytes = calloc(bitmap->n_blocks * 2048, sizeof(uint8_t));

  uint8_t buffer[1];
  for (int byte = 0; byte < bitmap->n_blocks * 2048; byte++){
    fread(buffer, sizeof(uint8_t), (size_t)1, disk);
    bitmap->bytes[byte] = buffer[0];
  }

  fclose(disk);
  return bitmap;
}

void close_bitmap(Bitmap* bitmap){
  FILE* disk = fopen(path_disk, "r+b");
  fseek(disk, 0, bitmap->start);
  uint8_t buffer[1];
  for (int byte = 0; byte < bitmap->n_blocks * 2048; byte++){
    buffer[0] = bitmap->bytes[byte];
    fwrite(buffer, sizeof(uint8_t), 1, disk);
  }

  fclose(disk);
  free(bitmap->bytes);
  free(bitmap);
}
