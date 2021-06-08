#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "main.h"
#include "../structs/structs.h"
#include "../structs/os_API.h"


/*
4.00 pts. Funciones de biblioteca.
• 0.30 pts. os mount.
• 0.30 pts. os bitmap.
• 0.30 pts. os exists.
• 0.30 pts. os ls.
• 0.30 pts. os mbt.
• 0.30 pts. os create partition.
• 0.20 pts. os delete partition.
• 0.20 pts. os reset mbt.
• 0.20 pts. os open.
• 0.50 pts. os read.
• 0.50 pts. os write.
• 0.30 pts. os close.
• 0.30 pts. os rm.
*/

int main(int argc, char **argv)
{
  os_mount(argv[1], 2); // Montamos el disco y probamos que funciona mediante 
  os_mbt(); // la visualización de las particiones válidas
  printf("La partición montada es la %d.\n", partition);
  os_ls(); // La visualización de los archivos dentro de la particion
  
  os_mount(argv[1], 3);
  printf("La partición montada es la %d.\n", partition);
  os_ls();

  os_mount(argv[1], 4);
  printf("La partición montada es la %d.\n", partition);
  os_ls();
  

  os_create_partition(5, 20000);
  os_mbt();
  os_mount(argv[1], 5);
  printf("\nAl montar de nuevo se lee el disco, se comprueba la creación.\n");
  os_mbt();
  printf("La partición montada es la %d (la nueva). No debería tener archivos en ella.\n", partition);
  os_ls();

  os_mount(argv[1], 2);
  os_delete_partition(5);
  os_mount(argv[1], 2);
  printf("\nAl montar de nuevo se lee el disco, se comprueba la eliminación.\n");
  os_mbt();

  destroy_mbt();
  return 0;
}
