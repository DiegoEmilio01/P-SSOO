#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "main.h"
#include "../structs/structs.h"
#include "../structs/os_API.h"


int main(int argc, char **argv)
{

  os_mount(argv[1], 4); // Montamos el disco y probamos que funciona mediante 
  os_mbt(); // la visualización de las particiones válidas
  printf("La partición montada %d \n", partition);
  os_reset_mbt();
  printf("Intentar montar alguna de las particiones que eran validas debería dar error.\n");
  os_mount(argv[1], 2);
  os_mount(argv[1], 3);
  os_mount(argv[1], 4);

  printf("\nNo deberían haber particiones válidas.\n");
  os_mbt();
  destroy_mbt();

  return 0;
}
