#include "stdio.h"
#include "../structs/structs.h"
#include "../structs/os_API.h"

int main(int argc, char **argv)
{
  os_mount(argv[0], 1);

  return 0;
}
