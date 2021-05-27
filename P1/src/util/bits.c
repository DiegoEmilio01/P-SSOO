// https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "bits.h"

typedef uint8_t uint8;
// big endian -> valor mas alto en menor posicion

int pos, offset;

void bt_set(uint8_t *bitarray, uint8 position, bool value){
  offset = position / (sizeof(uint8)*8);
  pos = sizeof(uint8)*8 - 1 - (position % (sizeof(uint8)*8));
  bitarray += offset;
  (*bitarray) = ((*bitarray) & ~((uint8)1 << pos)) | (value << pos);
}


/** función que retorna el valor de un bit dada su posición, dentro de un byte 
 * @param byte (bitarray?)
 * @param position (uint8 desde 0)
 * @return value_bit (int 0 o 1)
 * 
 */
bool bt_get(uint8 *bitarray, uint8 position){
  offset = position / (sizeof(uint8)*8);
  pos = sizeof(uint8)*8 - 1 - (position % (sizeof(uint8)*8));
  bitarray += offset;
  return ((*bitarray) >> pos) & (uint8_t)1;
}

/** función que retorna un int dado un bitarray y un rango de valores
 * @param byte (bitarray?)
 * @param position (uint8 de 1 a 8)
 * @return retorna valores de char[1:8]
 */
uint8 get_partition_id(uint8 byte){
  return ((uint8_t)(byte << 1)) >> 1;
}

/** entrega los ultimos 3 bytes de 4. Sitve para MBT y bloque de directorio
 * @param puntero a inicio del bloque de directorio
 * @return retorna tamaño del archivo indicado por el bloque indice
 */
uint32_t last_3_bytes_of_4(uint8 *bitarray){
  uint32_t t = (uint8_t)bitarray[3];
  uint32_t ret = t;
  t = (uint8_t)bitarray[2];
  ret |= (t<<8);
  t = (uint8_t)bitarray[1];
  ret |= (t<<8*2);
  return ret;
}

/** Funcion que da el tamaño del bloque indice. 
 * @note Puedes hacer un arreglo de uint8 con ceros y solo leer los pimeros 5
 * @param puntero a inicio del bloque indice, favor tamaño >= 64 bits
 * @return retorna tamaño del archivo indicado por el bloque indice
 */
uint64_t get_index_size(uint8 *bitarray){
  uint64_t t = (uint8_t)bitarray[4];
  uint64_t ret = t;
  t = (uint8_t)bitarray[3];
  ret |= (t<<8);
  t = (uint8_t)bitarray[2];
  ret |= (t<<8*2);
  t = (uint8_t)bitarray[1];
  ret |= (t<<8*3);
  t = (uint8_t)bitarray[0];
  ret |= (t<<8*4);
  return ret;
}

uint32_t int_from_4_bytes(uint8 *bitarray){
  uint32_t t = (uint8_t)bitarray[3];
  uint32_t ret = t;
  t = (uint8_t)bitarray[2];
  ret |= (t<<8);
  t = (uint8_t)bitarray[1];
  ret |= (t<<8*2);
  t = (uint8_t)bitarray[0];
  ret |= (t<<8*3);
  return ret;
}


// // ejemplo de uso: uwu
// #include <stdio.h>
// #include <stdlib.h>
// int main(int argc, char const **argv)
// {
//   // https://stackoverflow.com/questions/22059189/read-a-file-as-byte-array
//   FILE *fileptr;
//   char *buffer;
//   long filelen;

//   fileptr = fopen("testdisk.bin", "rb");  // Open the file in binary mode
//   fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
//   filelen = ftell(fileptr);             // Get the current byte offset in the file
//   rewind(fileptr);                      // Jump back to the beginning of the file

//   buffer = (char *)malloc(filelen * sizeof(char)); // Enough memory for the file
//   fread(buffer, filelen, 1, fileptr); // Read in the entire file
//   printf("first bit: %d\n", bt_get(buffer,0));
//   printf("partition id: %d\n", get_partition_id(buffer[0]));
//   printf("Idenfiticador absoluto: %d\n", last_3_bytes_of_4(buffer));
//   fclose(fileptr); // Close the file
// }

