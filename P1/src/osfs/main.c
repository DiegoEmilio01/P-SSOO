#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "main.h"
#include "../structs/structs.h"
#include "../structs/os_API.h"


int main(int argc, char **argv)
{
  //os_exists("nene.txt");
  /*Archivos encontrados:
  - vtm.jpg
  - ploblpem.png
  - n.png
  - ubu.gif ?
  - mercedes_benz.mp4
  */
  /* os_mount(argv[1], 3);
  os_create_partition(5, 20000);    // 123506, 20000
  os_mbt();
  os_bitmap(1);


  os_mount(argv[1], 5);
  os_bitmap(1);



  os_delete_partition(5); */
  
  /*
  En la partición 2, el archivo nene.txt le encuentro su bloque
  índice en 25841 y después leo su tamaño de archivo que es 3521 Bytes
  */
  /* osFile *archivo_prueba = os_open("mercedes_benz.mp4",'r');
  uint16_t my_buffer[1000];
  int leido;
  leido = os_read(archivo_prueba, my_buffer, 1000);
  printf("leido: %d\n", leido);
  for (int i=0; i<10 && i<leido/2; i++)
    printf("%" PRIu16 "\t", my_buffer[i]);
  printf("\n");
  uint8_t my_buffer2[1000];
  leido = os_read(archivo_prueba, my_buffer2, 1000);
  printf("leido: %d\n", leido);
  
  for (int i=0; i<10 && i<leido; i++)
    printf("%" PRIu8 " ", my_buffer2[i]);
  printf("\n"); */

  //os_close(archivo_prueba);
  // os_create_partition(5, 20000);    // 123506, 20000
  
  // os_mount(argv[1], 1);
  // os_mbt();
  
  // os_delete_partition(5);
  // os_mount(argv[1], 1);
  // os_mbt();
  
  //destroy_mbt();
  //destroy_mbt();
  os_mount(argv[1], 2);
  
  
  //os_ls();
  os_create_partition(5, 20000);
  //printf("1--------\n");
  os_mbt();
  //printf("2--------\n");
  
  os_mount(argv[1], 5);
  os_mbt();
  os_bitmap(0);

  os_mount(argv[1], 2);
  os_delete_partition(5);


  // os_bitmap(0);
  // os_bitmap(1);
  // os_bitmap(2);
  // os_bitmap(3);
  // os_bitmap(4);
  
  
  //osFile *archivo_prueba = os_open("dog.2",'r');
  //extract_file(archivo_prueba);
  //int numero_random = os_close(archivo_prueba);


  destroy_mbt();

  return 0;
}
