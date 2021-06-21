#pragma once

#include "clases.h"
#include "game.h"

int enemy_selector(Entity* yo, int len_enemigos);
int ally_selector(Entity* yo, int len_enemigos, Entity* aliados);

void extras_handler(Entity* entes, int len_entes, Game game);

int heal(Entity* healer, Entity* objetivo, int heal_base);
int attack(Entity* atacante, Entity* objetivo, int dano_base);