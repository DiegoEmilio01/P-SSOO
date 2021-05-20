// https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint8_t uint8;
// big endian -> valor mas alto en menor posicion

int pos, offset;

void bt_set(int *bitarray, uint8 position, bool value){
  offset = position / sizeof(uint8);
  pos = sizeof(uint8)*8 - 1 - (position % sizeof(uint8));
  bitarray += offset;
  (*bitarray) ^= ((*bitarray) & ~(1UL << pos)) | (value << pos);
}


/** función que retorna el valor de un bit dada su posición, dentro de un byte 
 * @param byte (bitarray?)
 * @param position (uint8 de 1 a 8)
 * @return value_bit (int 0 o 1)
 * 
 */
bool bt_get(uint8 *bitarray, uint8 position){
  offset = position / sizeof(uint8);
  pos = sizeof(uint8)*8 - 1 - (position % sizeof(uint8));
  bitarray += offset;
  return ((*bitarray) >> pos) & 1UL;
}

/** función que retorna un int dado un bitarray y un rango de valores
 * @param byte (bitarray?)
 * @param position (uint8 de 1 a 8)
 * @return retorna valores de char[1:8]
 */
uint8 get_partition_id(uint8 byte){
  return ((byte) << 1) >> 1;
}

/** entrega los ultimos 3 bytes de 4. Sitve para MBT y bloque de directorio
 * @param puntero a inicio del bloque de directorio
 * @return retorna tamaño del archivo indicado por el bloque indice
 */
uint32_t last_3_bytes_of_4(uint8 *bitarray){
  uint32_t ret = *bitarray;
  ret <<= 8;
  ret >>= 8;
  return ret;
}

/** Funcion que da el tamaño del bloque indice. 
 * @note Puedes hacer un arreglo de uint8 con ceros y solo leer los pimeros 5
 * @param puntero a inicio del bloque indice, favor tamaño >= 64 bits
 * @return retorna tamaño del archivo indicado por el bloque indice
 */
uint64_t get_index_size(uint8 *bitarray){
  uint64_t ret = *bitarray;
  ret >>= 3*8;
  return ret;
}

uint32_t read_4_bytes(uint8 *bitarray){
  return *((int*) bitarray);
}


/* ejemplo de uso: uwu
#include <stdio.h>
int main()
{
    uint8 *a = calloc(100,sizeof(int));
    bt_set(a, 0, 1);
    printf("%d\n", bt_get(&a,0));
    printf("%d\n", sizeof(char));
    free(a);
    return 0;
}
*/
