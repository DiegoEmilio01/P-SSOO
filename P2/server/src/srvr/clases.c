#pragma once
#include <stdlib.h>

#include "clases.h"
#include "comunication.h"
#include "texts.h"

// CAZADOR
char* f_estocada(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo){
  if (!len_enemigos) printf("Va a fallar\n");
  int objective = rand() % len_enemigos;
  enemigos[objective].hp -= 1000;
  if (enemigos[objective].effect_type != 's'){
    enemigos[objective].effect_contador = 0;
    enemigos[objective].effect_value = 0;
  }else if (enemigos[objective].effect_contador == 3){
    return NULL;
  }
  enemigos[objective].effect_type = 's';  // sangrado
  enemigos[objective].effect_value += 500;
  enemigos[objective].effect_contador += 1;
  return NULL;
}

char* f_corte_cruzado(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo){
  if (!len_enemigos) printf("Va a fallar\n");
  int objective = rand() % len_enemigos;
  enemigos[objective].hp -= 1000;
  return NULL;
}

char* f_distraer(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo){
  if (!len_enemigos) printf("Va a fallar\n");
  int objective = rand() % len_enemigos;
  enemigos[objective].effect_type = 'd';  // distraer
  enemigos[objective].effect_value = posicion_yo;
  return NULL;
}

// MEDICO
char* f_curar(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo){

  if (!len_enemigos) printf("Va a fallar\n");
  int objective = len_aliados;
  if (objective != 1){
    objective = (int)request_int(socket_yo);
  }
  aliados[objective].hp += 2000;
  if (aliados[objective].hp > aliados[objective].max_hp)
    aliados[objective].hp = aliados[objective].max_hp;
  return NULL;
}

char* f_destello(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo){
  // entre 0 y 1250, luego le damos offset 750 para [750, 2000]
  int damage = rand() % 1251;
  int pos_to_heal = rand() % len_aliados;
  int pos_to_dmg = rand() % len_enemigos;
  damage += 750;
  // divide en la mitad y redondea hacia arriba (ej: 1001 / 2 + 1001 % 2 = 500 + 1 = 501)
  int heal = damage / 2 + (damage % 2);
  // curamos al aliado
  Entity* aliado = &aliados[pos_to_heal];
  aliado->hp += heal;
  if (aliado->hp > aliado->max_hp) aliado->hp = aliado->max_hp;
  // dañamos al enemigo
  enemigos[pos_to_dmg].hp -= damage;

  return NULL;
}

char* f_descarga(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo){
  Entity* aliado = &aliados[posicion_yo];
  int damage = (aliado->max_hp - aliado->hp) * 2;
  int enemy_pos = rand() % len_enemigos;
  enemigos[enemy_pos].hp -= damage;

  return NULL;
}

// tomamos que puede ser efecto a si mismo
char* f_inyeccion(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo){
  int pos_aliado = rand() % len_aliados;
  Entity* aliado = &aliados[pos_aliado];
  aliado->effect_type = 'q';

  return NULL;
}


/* 
EFECTOS ESPECIALES:

TODO: if del hacker
Estocada:
SQL:
Reprobaton-9000:

 */