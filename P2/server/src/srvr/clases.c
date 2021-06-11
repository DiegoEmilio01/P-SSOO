#pragma once
#include <stdlib.h>
#include "clases.h"
#include "comunication.h"
#include "texts.h"



char* f_estocada(struct entity* aliados, int len_aliados, int posicion_yo, struct entity* enemigos, int len_enemigos, int socket_yo){
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

char* f_corte_cruzado(struct entity* aliados, int len_aliados, int posicion_yo, struct entity* enemigos, int len_enemigos, int socket_yo){
  if (!len_enemigos) printf("Va a fallar\n");
  int objective = rand() % len_enemigos;
  enemigos[objective].hp -= 1000;
  return NULL;
}

char* f_distraer(struct entity* aliados, int len_aliados, int posicion_yo, struct entity* enemigos, int len_enemigos, int socket_yo){
  if (!len_enemigos) printf("Va a fallar\n");
  int objective = rand() % len_enemigos;
  enemigos[objective].effect_type = 'f';  // focus
  enemigos[objective].effect_value = posicion_yo;
  return NULL;
}

char* f_curar(struct entity* aliados, int len_aliados, int posicion_yo, struct entity* enemigos, int len_enemigos, int socket_yo){

  if (!len_enemigos) printf("Va a fallar\n");
  int objective = len_aliados;
  if (objective != 1){
    objective = (int)request_int(socket_yo, 0, objective-1);
  }
  aliados[objective].hp += 2000;
  if (aliados[objective].hp > aliados[objective].max_hp)
    aliados[objective].hp = aliados[objective].max_hp;
  return NULL;
}