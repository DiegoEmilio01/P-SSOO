#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "main.h"
#include "../structs/structs.h"
#include "../structs/os_API.h"




int main(int argc, char **argv)
{

  os_mount(argv[1], 2); // Montamos el disco y probamos que funciona mediante 
  printf("La partición montada %d \n", partition);
  os_mbt(); // la visualización de las particiones válidas
  os_ls();

  // Vamos a buscar el archivo nene.txt para tenerlo con sus bloques generados:
  osFile *archivo = os_open("mystery.mp3", 'r');
  extract_file(archivo); //extract_file usa os_read y recupera el archivo.
  os_close(archivo);
  
  // os_rm("mystery.mp3");  Función incompleta
  // destroy_directory(bloque_directory); para línea 531 de os_API.c
  
  destroy_mbt();
  return 0;
}
