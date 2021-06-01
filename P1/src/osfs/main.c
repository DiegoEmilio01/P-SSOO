#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "../structs/structs.h"
#include "../structs/os_API.h"


int main(int argc, char **argv)
{
  os_mount(argv[1], 2);
  
  os_mbt();
  os_bitmap(1);

  os_create_partition(5, 20000);    // 123506, 20000
  
  os_mount(argv[1], 5);
  os_mbt();
  
  os_delete_partition(5);
  os_mount(argv[1], 3);
  os_mbt();
  
  destroy_mbt();
  return 0;
}
