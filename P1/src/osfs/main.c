#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "../structs/structs.h"
#include "../structs/os_API.h"

Mbt* mbt;

int main(int argc, char **argv)
{
  mbt = malloc(sizeof(Mbt));
  os_mount(argv[1], 1);

  for (int i = 0; i < mbt->entry_quantity; i++)
    if (mbt->entry_container[i])
      free(mbt->entry_container[i]);
  free(mbt->entry_container);
  free(mbt);
  return 0;
}
