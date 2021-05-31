#include <stdint.h>
#pragma once
typedef uint8_t uint8;

void bt_set(uint8 *bitarray, uint8 position, bool value);
bool bt_get(uint8 *bitarray, uint8 position);
uint8 get_partition_id(uint8 byte);
uint8 bit_and_id_to_byte(bool bit, uint8_t id);
uint32_t last_3_bytes_of_4(uint8 *bitarray);
void insert_location_to_buffer(uint8_t *buffer, uint32_t location);
uint64_t get_index_size(uint8 *bitarray);
uint32_t int_from_4_bytes(uint8 *bitarray);
void insert_size_to_buffer(uint8_t *buffer, uint32_t size);
