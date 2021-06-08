#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "main.h"
#include "../structs/structs.h"
#include "../structs/os_API.h"


int main(int argc, char **argv)
{
  os_mount(argv[1], 0);
  os_mount(argv[1], -10);
  os_mount(argv[1], 2);
  printf("\n\n");
  os_create_partition(-1, 17000);
  os_create_partition(0, 1);
  os_create_partition(0, 140000);

  os_delete_partition(2);
  printf("\n\n");
  os_delete_partition(127); // como no va a encontrar estas entradas en la MBT, no borrará nada
  os_delete_partition(-5);

  os_exists("");    // No genera ningún error.

  osFile *archivo = os_open("nene.txt", 'w');
  os_close(archivo);

  destroy_mbt();
  return 0;
}
