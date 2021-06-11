#pragma once
#include <stdlib.h>

#include "clases.h"
#include "comunication.h"
#include "texts.h"

#pragma region CAZADOR
// ------ FUNCIONES CAZADOR ------

// CAZADOR n°0
// Sangrado se stackea, hasta 3 veces
// Daña 1000 a enemigo aleatorio, y deja sangrado de 500 infinito
char* f_estocada(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
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

// CAZADOR n°1
// Daña 3000 a enemigo aleatorio
char* f_corte_cruzado(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  if (!len_enemigos) printf("Va a fallar\n");
  int objective = rand() % len_enemigos;
  enemigos[objective].hp -= 1000;

  return NULL;
}

// CAZADOR n°2
// Distrae al monstruo, haciendo que este ataque al último cazador en distraerlo
char* f_distraer(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  if (!len_enemigos) printf("Va a fallar\n");
  int objective = rand() % len_enemigos;
  enemigos[objective].effect_type = 'd';  // distraer
  enemigos[objective].effect_value = posicion_yo;
  return NULL;
}

#pragma endregion CAZADOR


#pragma region MEDICO
// ------ FUNCIONES MEDICO ------

// MEDICO n°0
// Cura 2000 a aliado específico (puede ser sí mismo)
char* f_curar(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){

  if (!len_enemigos) printf("Va a fallar\n");
  int objective = len_aliados;
  if (objective != 1){
    objective = (int)request_int(aliados[posicion_yo].socket, 0, objective - 1);
  }
  aliados[objective].hp += 2000;
  if (aliados[objective].hp > aliados[objective].max_hp)
    aliados[objective].hp = aliados[objective].max_hp;
  return NULL;
}

// MEDICO n°1
// Asumimos que se puede curar a si mismo
// Daña entre 750 y 2000 a enemigo aleatorio y cura ceil(daño) a aliado
char* f_destello(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
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

// MEDICO n°2
// Daña 2 * (vida_max - vida) a enemigo aleatorio
char* f_descarga(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
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

// HACKER n°0
// Asumimos que puede darse efecto a si mismo
// Duplica el daño de un aliado, por 2 turnos
char* f_inyeccion(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  if (!len_enemigos) printf("Va a fallar\n");

  int pos_aliado = rand() % len_aliados;
  Entity* aliado = &aliados[pos_aliado];
  aliado->effect_type = 'q'; // S'Q'L

  return NULL;
}

// HACKER n°1
// Daña 1500 a enemigo aleatorio
char* f_ddos(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  if (!len_enemigos) printf("Va a fallar\n");

  int enemy_pos = rand() % len_enemigos;
  enemigos[enemy_pos].hp -= 1500;
}

// HACKER n°2
// Daña 1500 a enemigo aleatorio
char* f_fuerzabruta(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  if (!len_enemigos) printf("Va a fallar\n");
  
  Entity* yo = &aliados[posicion_yo];
  int enemy_pos = rand() % len_enemigos;

  if (yo->effect_type != 'f'){
    yo->effect_contador = 1;
    yo->effect_value = 0;
  }
  yo->effect_type = 'f'; // fuerzaBruta
  yo->effect_contador += 1;
  if (yo->effect_contador == 3){
    enemigos[enemy_pos].hp -= 10000;
    yo->effect_contador = 0;
  }
  return NULL;
}

#pragma endregion HACKER


#pragma region JAGRUZ
// ------ FUNCIONES JAGRUZ ------

// JAGRUZ
char* f_ruzgar(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){

}

// JAGRUZ
char* f_ruzgar(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){

}

#pragma endregion JAGRUZ


#pragma region RUZALOS
// ------ FUNCIONES RUZALOS ------

// RUZALOS n°0
char* f_salto(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){

}

// RUZALOS n°1
char* f_espina(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){

}

#pragma endregion RUZALOS



/* void extras_handler(Entity* aliados, int len_aliados, Entity* enemigos, int len_enemigos){
  for (int n_enemigo = 0; n_enemigo < len_enemigos; n_enemigo++){
    if (enemigos[n_enemigo].effect_type == 's'){
      Entity* enemigo = &enemigos[n_enemigo];
      enemigo->hp -= enemigo->effect_value;
    }
  }
} */
/* 
EFECTOS ESPECIALES:

TODO: if del hacker, duplica daño.
TODO: En turno de monstruo, si type = 's', sangrado.
TODO: Forzar que monstruo ataque por 'd' - "distraer".
TODO: revisar si aliados y monstruos tienen HP <= 0.

Estocada:     's': realizar sangrado
Distraer:     'd': forzar ataque por distracción
SQL:          'q': duplicar daño de aliado
FuerzaBruta:  'f': si se usa 3 veces, causa 10.000 de daño
Reprobaton:   

*/