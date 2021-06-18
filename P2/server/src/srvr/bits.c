
#pragma once
// https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit
#include <stdint.h>
#include "bits.h"
#include <assert.h>

typedef uint8_t uint8;
// big endian -> valor mas alto en menor posicion

int pos, offset;

void bt_set(uint8_t *byte, uint8 position, bool value){
  assert(position < 8);
  pos = sizeof(uint8)*8 - 1 - position;
  (*byte) = ((*byte) & ~((uint8)1 << pos)) | (value << pos);
}


/** función que retorna el valor de un bit dada su posición, dentro de un byte 
 * @param byte (bitarray?)
 * @param position (uint8 desde 0)
 * @return value_bit (int 0 o 1)
 * 
 */
bool bt_get(uint8 *byte, uint8 position){
  assert(position < 8);
  pos = sizeof(uint8)*8 - 1 - position;
  return ((*byte) >> pos) & (uint8_t)1;
}
