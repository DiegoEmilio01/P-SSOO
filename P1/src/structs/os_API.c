#include <stdbool.h>
#include <stdio.h>

#include "os_API.h"
#include "structs.h"
#include "../osfs/main.h"


/** monta el os
 * @param
 * @returns Mbt*
*/
void os_mount(char* diskname, int partition_id){

  char* path_disk = diskname;
  int partition = partition_id;
  // OJO: rb+ para leer y escribir en binario (?)
  FILE* disk = fopen(diskname, "r+b");  // intentar con rb+
  printf("archivo abierto\n");
  init_mbt(disk);
  printf("Vamos a cerrar\n");
  fclose(disk);
}

void os_bitmap(unsigned num){
  // https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit
  printf("Imprimir bitmap\n"); 
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
  for (int i = 0; i < mbt->entry_quantity; i++){
    if (mbt->entry_container[i]->is_valid){
      printf("    id: %d\n", mbt->entry_container[i]->id);
      printf("    location: %d\n", mbt->entry_container[i]->location);
      printf("    size: %d\n\n", mbt->entry_container[i]->size);
    }
  }
}

void os_create_partition(int id, int size){
  printf("Creando partición %d de tamaño %d\n", id, size);
}

// TODO: lanzar OS_ERROR invalid_delete_partition
void os_delete_partition(int delete_id){
  printf("Eliminando partition %d de la MBT\n", delete_id);

  // Crea el puntero al archivo disk, en modo read write 
  FILE* disk = fopen(path_disk, "r+b");
  fseek(disk, 0, SEEK_SET);
  bool raise_error = true;
  uint8_t buffer[8];

  for (int entry_id = 0; entry_id < mbt->entry_quantity; entry_id++){

    // Obtenemos el id de partición y el bit de validez
    size_t ret = fread(buffer, sizeof(uint8_t), (size_t)8, disk);
    bool is_valid = bt_get(buffer, 0);
    int partition_id = get_partition_id(buffer[0]);

    if (is_valid && (partition_id == delete_id)){
      // si la partición es válida e id es correcto, cambia el primer byte de la entrada a 0.
      // es decir, el bit de validez y el id pasa a ser 0.
      uint8_t zero_buffer[1] = {0};
      fwrite(buffer, sizeof(uint8_t), 1, disk);
      mbt->entry_container[entry_id]->is_valid = 0;

      raise_error = false;
      break;
    }
    // Avanzamos hacia el siguiente trozo de 8 bytes, solución pobre
    fseek(disk, 8 * (entry_id - 1), SEEK_SET);
  }

  if (raise_error){
    // RAISE ERROR
  }

  fclose(disk);
}

void os_reset_mbt(){
  printf("Eliminando particiones de la MBT\n");

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
    fseek(disk, 8 * (entry_id - 1), SEEK_SET);
  }

  // tal vez un raise_error pero no se me ocurre qué.

  fclose(disk);

}

void os_open(char* filename, char mode){
  printf("Abriendo archivo %s\n", filename);
}

void os_read(osFile* file_desc, void* buffer, int nbytes){
  printf("Leyendo %d bytes del archivo\n", nbytes);
}

void os_write(osFile* file_desc, void* buffer, int nbytes){
  printf("Escribiendo archivo\n");
}

void os_close(osFile* file_desc){
  printf("Cerrando archivo\n");
}

void os_rm(char* filename){
  printf("Borrando archivo %s\n", filename);
}