#include <stdint.h>
#pragma once
typedef uint8_t uint8;

void bt_set(uint8 *bitarray, uint8 position, bool value);
bool bt_get(uint8 *bitarray, uint8 position);
uint8 get_partition_id(uint8 byte);
uint32_t last_3_bytes_of_4(uint8 *bitarray);
uint64_t get_index_size(uint8 *bitarray);
uint32_t int_from_4_bytes(uint8 *bitarray);
