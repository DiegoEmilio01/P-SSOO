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
  FILE* disk = fopen(path_disk, "rb+");
  Mbt* mbt = init_mbt(disk);
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
  printf("Mostrando particiones válidas de la MTB\n");
}

void os_create_partition(int id, int size){
  printf("Creando partición %d de tamaño %d\n", id, size);
}

void os_delete_partition(int id){
  printf("Eliminando partition %d de la MTB\n", id);
}

void os_reset_mbt(){
  printf("Eliminando particiones de la MTB\n");
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