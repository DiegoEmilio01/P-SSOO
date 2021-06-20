#pragma once
#include "clases.h"

int enemy_selector(Entity* yo, int len_enemigos);
int ally_selector(Entity* yo, int len_enemigos);

void extras_handler(Entity* entes, int len_entes);

int heal(Entity* healer, Entity* objetivo, int heal_base);
int attack(Entity* atacante, Entity* objetivo, int dano_base);