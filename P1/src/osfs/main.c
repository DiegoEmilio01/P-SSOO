#include "stdio.h"
#include "../structs/structs.h"
#include "../structs/os_API.h"

enum os_error OS_ERROR;
Mbt* mbt;

int main(int argc, char const **argv)
{
  char* path_disk = argv[0];
  Mbt* mbt = malloc(sizeof(Mbt));
  
  os_mount(path_disk, 1);

  return 0;
}
