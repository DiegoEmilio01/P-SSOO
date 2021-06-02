#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "os_API.h"
#include "structs.h"
#include "../osfs/main.h"
#include "../util/bits.h"

char* path_disk;
int partition;
Mbt* mbt;

/** monta el os
 * @param
 * @returns Mbt*
*/
void os_mount(char* diskname, int partition_id){
  if (mbt){
    destroy_mbt();
  }
  mbt = malloc(sizeof(Mbt));

  path_disk = diskname;

  // OJO: rb+ para leer y escribir en binario (?)
  FILE* disk = fopen(diskname, "r+b");  // intentar con rb+
  init_mbt(disk);
  if (mbt->entry_container[partition_id] && mbt->entry_container[partition_id]->is_valid){
    partition = partition_id;
  } else{
    fprintf(stderr, "\e[1;31m [ERROR] \e[0m: partición inválida \n");
  }
  fclose(disk);
}

void os_bitmap(unsigned num){
  // instanciar bitmap
  Bitmap* bitmap = init_bitmap();

  if (num > bitmap->n_blocks || num < 0){
    fprintf(stderr, "\e[1;31m [ERROR] \e[0m: input fuera de rango \n");
    return;
  }

  fprintf(stderr, "\e[1;34m    Bitmap\e[0m\n\n");
  if (num == 0){
    uint8_t buffer[32];
    int counter = 0;
    for (int byte = 0; byte < bitmap->n_blocks * 2048; byte++){
      if (counter == 32){
        for (int i = 0; i < 32; i++){
          fprintf(stderr, "0x%02hhX ", buffer[i]);
        }
        fprintf(stderr, "\n");
        counter = 0;
      }
      buffer[counter] = bitmap->bytes[byte];
      counter++;
    }
  } else{
    uint8_t buffer[32];
    int counter = 0;
    for(int byte = 2048 * (num - 1); byte < 2048 * (num - 1) + 2048; byte++){
      if (counter == 32){
        for (int i = 0; i < 32; i++){
          fprintf(stderr, "0x%02hhX ", buffer[i]);
        }
        fprintf(stderr, "\n");
        counter = 0;
      }
      buffer[counter] = bitmap->bytes[byte];
      counter++;
    }
  }
  fprintf(stderr, "\n");

  close_bitmap(bitmap);
}

bool os_exists(char* filename){
  printf("Corroborando existencia de %s\n", filename);
  return false;
}

void os_ls(){
  printf("Listando elementos de la partición actual\n");
}

void os_mbt(){
  printf("Particiones válidas\n");
  for (int entry_id = 0; entry_id < mbt->entry_quantity; entry_id++){
    if (mbt->entry_container[entry_id] && mbt->entry_container[entry_id]->is_valid){
      printf("    \e[1;35m id: \e[0m %d\n", mbt->entry_container[entry_id]->id);
      printf("    \e[1;34m location: \e[0m %d\n", mbt->entry_container[entry_id]->location);
      printf("    \e[1;32m size: \e[0m %d\n", mbt->entry_container[entry_id]->size);
      printf("    --------------------------\n\n");
    }
  }
}

void os_create_partition(int id, int size){
  printf("Creando partición %d de tamaño %d\n\n", id, size);
  if (size < 16384 || size > 131072)
  {
    printf("ERROR: wrong size as argument. Size must be between 16384 and 131072.\n\n");
  }
  else if (id < 0 || id > 127)
  {
    printf("ERROR: wrong id as argument. Id must be between 0 and 128.\n\n");
  }
  else if (mbt->entry_container[id] && mbt->entry_container[id]->is_valid)
  {
    printf("ERROR: partition already created. If aditional partition is needed, please try another id.\n\n");
  }
  else
  {
    TEntry* tentry = NULL;
    for (int entry_id = 0; entry_id < mbt->entry_quantity; entry_id++){
      if (mbt->entry_container[entry_id] && mbt->entry_container[entry_id]->is_valid){
        tentry = init_tentry(tentry, mbt->entry_container[entry_id]->location, mbt->entry_container[entry_id]->size);
      }
    }

    if (!tentry || size < tentry->location)
    {
      Entry* entry = init_entry(true, id, 0, size);
      mbt->entry_container[id] = entry;
      write_new_partition(id, 0, size);
      printf("Partition created successfully.\n\n");
      destroy_tentry(tentry);
      return;
    }

    TEntry* current = tentry;
    while (current)
    {
      uint32_t posible_location = current->location + current->size;
      if (!current->next)
      {
        if (2097151 - posible_location + 1 >= size)
        {
          Entry* entry = init_entry(true, id, posible_location, size);
          mbt->entry_container[id] = entry;
          write_new_partition(id, posible_location, size);
          printf("Partition created successfully.\n\n");
        }
        else
        {
          printf("ERROR: partition cannot be allocated, please try a smaller size.\n\n");
        }
        destroy_tentry(tentry);
        return;
      }
      else if (current->next->location - posible_location >= size)
      {
        Entry* entry = init_entry(true, id, posible_location, size);
        mbt->entry_container[id] = entry;
        write_new_partition(id, posible_location, size);
        printf("Partition created successfully.\n\n");
        destroy_tentry(tentry);
        return;
      }
      current = current->next;
    }    
  }
}

void write_new_partition(int id, int location, int size) // falta crear los bloques de bitmap
{
  FILE* disk = fopen(path_disk, "r+b");
  fseek(disk, 0, SEEK_SET);
  uint8_t buffer[8];
  for (int entry_id = 0; entry_id < mbt->entry_quantity; entry_id++)
  {
    fread(buffer, sizeof(uint8_t), (size_t)8, disk);
    bool is_valid = bt_get(buffer, 0);
    if (!is_valid)
    {
      fseek(disk, -8, SEEK_CUR);
      uint8_t new_buffer[8];
      new_buffer[0] = bit_and_id_to_byte(1, (uint8_t)id);
      insert_location_to_buffer(new_buffer, (uint32_t)location);
      insert_size_to_buffer(new_buffer, (uint32_t)size);
      fwrite(new_buffer, sizeof(uint8_t), 8, disk);
      //create_bitmap(id);
      break;
    }
  }
  fclose(disk);
}

void create_bitmap(int id){
  int old_partition = partition;
  os_mount(path_disk, id);
  Bitmap* bitmap = init_bitmap();
  printf("n_blocks %d\n", bitmap->n_blocks);
  // modificar
  for (int i = 0; i < bitmap->n_blocks * 2048; i++)
  {
    bitmap->bytes[i] = 0;
  }

  for(int i = 0; i < bitmap->n_blocks + 1; i++){
    bt_set(bitmap->bytes, i, true);
  }

  close_bitmap(bitmap);
  os_mount(path_disk, old_partition);
}

// TODO: lanzar OS_ERROR invalid_delete_partition
void os_delete_partition(int delete_id){
  printf("Eliminando partición %d de la MBT\n\n", delete_id);

  // Crea el puntero al archivo disk, en modo read write 
  FILE* disk = fopen(path_disk, "r+b");
  fseek(disk, 0, SEEK_SET);
  bool raise_error = true;
  uint8_t buffer[8];

  for (int entry_id = 0; entry_id < mbt->entry_quantity; entry_id++){

    // Obtenemos el id de partición y el bit de validez
    fread(buffer, sizeof(uint8_t), (size_t)1, disk);
    bool is_valid = bt_get(buffer, 0);
    int partition_id = get_partition_id(buffer[0]);

    if (is_valid && (partition_id == delete_id)){
      // si la partición es válida e id es correcto, cambia el primer byte de la entrada a 0.
      // es decir, el bit de validez y el id pasa a ser 0.
      uint8_t zero_buffer[1] = {0};
      fseek(disk, -1, SEEK_CUR);
      fwrite(zero_buffer, sizeof(uint8_t), 1, disk);
      mbt->entry_container[entry_id]->is_valid = 0;

      raise_error = false;
      break;
    }
    // Avanzamos hacia el siguiente trozo de 8 bytes, solución pobre
    fseek(disk, 8 * (entry_id + 1), SEEK_SET);
  }

  if (raise_error){
    // RAISE ERROR
  }
  fclose(disk);
}

void os_reset_mbt(){
  printf("Eliminando particiones de la MBT\n\n");

  // Crea el puntero al archivo disk, en modo read write 
  FILE* disk = fopen(path_disk, "r+b");
  fseek(disk, 0, SEEK_SET);
  uint8_t zero_buffer[1] = {0};

  for (int entry_id = 0; entry_id < mbt->entry_quantity; entry_id++){
    // Cambia el primer byte de la entrada a 0.
    // es decir, el bit de validez y el id pasa a ser 0.
    fwrite(zero_buffer, sizeof(uint8_t), 1, disk);
    // Basta con hacer la entrada inválida, ya que en ese caso se sobreescribe lo demás
    mbt->entry_container[entry_id]->is_valid = 0;
    // No encontré si fwrite mueve el puntero en ningún lado :( p.d mi intuición dice que sí lo mueve
    // asi que tenemos la solución pobre:
    fseek(disk, 8 * (entry_id + 1), SEEK_SET);
  }

  // tal vez un raise_error pero no se me ocurre qué.

  fclose(disk);

}

void os_read(osFile* file_desc, void* buffer, int nbytes){
  printf("Leyendo %d bytes del archivo\n\n", nbytes);
}

void os_write(osFile* file_desc, void* buffer, int nbytes){
  printf("Escribiendo archivo\n\n");
}

void os_close(osFile* file_desc){
  printf("Cerrando archivo\n\n");
}

void os_rm(char* filename){
  printf("Borrando archivo %s\n\n", filename);
}


void os_open(char* filename, char mode){ //TODO: Falta rellenar bloque de data, y probar la inicializacion del Directory
  printf("Abriendo archivo %s\n", filename);
  FILE* disk = fopen(path_disk, "r+b");
  for (int entrada = 0; entrada < mbt->entry_quantity; entrada++)
  {
    if (mbt->entry_container[entrada]->is_valid)
    { //Si es que es válida vamos a revisar si es que el archivo está
      int inicio_directorio = (mbt->entry_container[entrada]->location)*2048 + 1024; // Inicio del directorio
      Directory *bloque_directory = directory_init(disk, inicio_directorio /* REVISAREIS*/);
      // Tengo que ir a la particion y rescatar el identificador relativo
      if(strcmp(bloque_directory->entries->filename, filename)){
        //Si el archivo era el que buscaba, procedo a rellenar sus bloques de datos y retorno
        int posicion_bloque_indice = 1024 + bloque_directory->entries[entrada].relative_index + mbt->entry_container[entrada]->size; // MBT + Particion + relative
        // Ahora rellenamos los bloques de datos
        IndexBlock *bloque_index = indexblock_init(disk, inicio_directorio /* REVISAREIS*/);
        
        break;
      }
    }
    
    /* Iteramos sobre la Mbt para buscar si es que el archivo está 
    en alguna de las particiones */
  }
};