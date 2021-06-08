#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "main.h"
#include "../structs/structs.h"
#include "../structs/os_API.h"



/*
4.00 pts. Funciones de biblioteca.

• 0.30 pts. os bitmap.

• 0.20 pts. os delete partition.

• 0.20 pts. os open.
• 0.50 pts. os read.
• 0.30 pts. os close.
• 0.30 pts. os rm.
*/
int main(int argc, char **argv)
{

  os_mount(argv[1], 2); // Montamos el disco y probamos que funciona mediante 
  printf("La partición montada %d \n", partition);
  os_mbt(); // la visualización de las particiones válidas
  os_ls();
  os_exists("nene.txt");
  printf("Resultado os_exists: %d\n\n", os_exists("no_existo.txt")); // Retorna 0 si no encuentra
  os_bitmap(1); 
  printf("\n");

  os_create_partition(5, 20000);
  os_mount(argv[1], 5);
  printf("Resultado os_exists: %d\n\n", os_exists("no_existo.txt")); // Retorna 0 si no encuentra
  // 0xE0 en bit es 11100000 Esto es justo los dos bitmap + el directorio que se necesitan para el tamaño de la partición.
  os_bitmap(0); // Se cuentan los bloques libres representados por el bitmap, no los que realmente tiene la partición.
  /* Último Bitmap no debería llenarse! */
  destroy_mbt();

  return 0;
}
