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
