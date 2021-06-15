#pragma once
#include <stdlib.h>

#include "clases.h"
#include "comunication.h"
#include "texts.h"

#pragma region CAZADOR
// ------ FUNCIONES CAZADOR ------

// CAZADOR
// Sangrado se stackea, hasta 3 veces
// Daña 1000 a enemigo aleatorio, y deja sangrado de 500 infinito
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

// CAZADOR
// Daña 3000 a enemigo aleatorio
char* f_corte_cruzado(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo){
  if (!len_enemigos) printf("Va a fallar\n");
  int objective = rand() % len_enemigos;
  enemigos[objective].hp -= 1000;
  return NULL;
}

// CAZADOR
// Distrae al monstruo, haciendo que este ataque al último cazador en distraerlo
char* f_distraer(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo){
  if (!len_enemigos) printf("Va a fallar\n");
  int objective = rand() % len_enemigos;
  enemigos[objective].effect_type = 'd';  // distraer
  enemigos[objective].effect_value = posicion_yo;
  return NULL;
}

#pragma endregion CAZADOR


#pragma region MEDICO
// ------ FUNCIONES MEDICO ------

// MEDICO
// Cura 2000 a aliado específico (puede ser sí mismo)
char* f_curar(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo){

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

// MEDICO
// Asumimos que se puede curar a si mismo
// Daña entre 750 y 2000 a enemigo aleatorio y cura ceil(daño) a aliado
char* f_destello(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo){
  if (!len_enemigos) printf("Va a fallar\n");
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

// MEDICO
// Daña 2 * (vida_max - vida) a enemigo aleatorio
char* f_descarga(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo){
  if (!len_enemigos) printf("Va a fallar\n");

  Entity* aliado = &aliados[posicion_yo];
  int damage = (aliado->max_hp - aliado->hp) * 2;
  int enemy_pos = rand() % len_enemigos;
  enemigos[enemy_pos].hp -= damage;

  return NULL;
}

#pragma endregion MEDICO


#pragma region HACKER
// ------ FUNCIONES HACKER ------

// HACKER
// Asumimos que puede darse efecto a si mismo
// Duplica el daño de un aliado, por 2 turnos
char* f_inyeccion(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo){
  if (!len_enemigos) printf("Va a fallar\n");

  int pos_aliado = rand() % len_aliados;
  Entity* aliado = &aliados[pos_aliado];
  aliado->effect_type = 'q';

  return NULL;
}

// HACKER
// Daña 1500 a enemigo aleatorio
char* f_ddos(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo){
  if (!len_enemigos) printf("Va a fallar\n");

  int enemy_pos = rand() % len_enemigos;
  enemigos[enemy_pos].hp -= 1500;
}

#pragma endregion HACKER


#pragma region JAGRUZ
// ------ FUNCIONES JAGRUZ ------

// JAGRUZ
char* f_ruzgar(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo){
  return NULL;
  }

// JAGRUZ
char* f_coletazo(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo){
  return NULL;
  }

#pragma endregion JAGRUZ


#pragma region RUZALOS
// ------ FUNCIONES RUZALOS ------

// RUZALOS
char* f_salto(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo){
  return NULL;
  }

// RUZALOS
char* f_espina(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo){
  return NULL;
  }

#pragma endregion RUZALOS
/* 
EFECTOS ESPECIALES:

TODO: if del hacker, duplica daño
TODO: revisar si aliados y monstruos tienen HP <= 0

Estocada: 's': realizar sangrado
Distraer: 'd': forzar ataque por distracción
SQL: 'q': duplicar daño de aliado
Reprobaton-9000:

 */