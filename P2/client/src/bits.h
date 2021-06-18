#include <stdint.h>
#include <stdbool.h>
#pragma once
typedef uint8_t uint8;

void bt_set(uint8*, uint8, bool);
bool bt_get(uint8*, uint8);
