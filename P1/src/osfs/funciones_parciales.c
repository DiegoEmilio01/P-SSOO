#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "main.h"
#include "../structs/structs.h"
#include "../structs/os_API.h"


int main(int argc, char **argv)
{
  os_mount(argv[1], 2);
  os_ls();
  
  osFile *archivo = os_open("nene.txt", 'r');
  extract_file(archivo); //extract_file usa os_read y recupera el archivo.
  os_close(archivo);
  
  os_rm("nene.txt"); // destroy_directory(bloque_directory); para línea 531 de os_API.c
  printf("\n");
  
  os_mount(argv[1], 2);
  os_ls();
  
  os_rm("nene.txt"); // intenta eliminarlo de nuevo pero como no existe arroja error
  printf("\n");
  destroy_mbt();
  return 0;
}
