#pragma once
#include <stdlib.h>
#include <assert.h>

#include "clases.h"
#include "comunication.h"
#include "texts.h"

char 
  *x_c_cazador = "Cazador",
  *x_c_medico = "Medico",
  *x_c_hacker = "Hacker",
  *x_c_gjr = "GreatJagRuz",
  *x_c_ruzalos = "Ruzalos",
  *x_c_ruiz = "Ruiz"
;

int enemy_selector(Entity* yo, int len_enemigos, bool is_attack);

char* class_def(enum classname clases, Entity *entity){
  switch (clases)
  {
    case Cazador:
      entity->func[0] = f_estocada;
      entity->func[1] = f_corte_cruzado;
      entity->func[2] = f_distraer;
      entity->hp = 5000;
      entity->max_hp = 5000;
      return x_c_cazador;
    case Medico:
      entity->func[0] = f_curar;
      entity->func[1] = f_destello;
      entity->func[2] = f_descarga;
      entity->hp = 3000;
      entity->max_hp = 3000;
      return x_c_medico;
    case Hacker:
      entity->func[0] = f_inyeccion;
      entity->func[1] = f_ddos;
      entity->func[2] = f_fuerzabruta;
      entity->hp = 2500;
      entity->max_hp = 2500;
      return x_c_hacker;
    case GreatJagRuz:
      strncpy(entity->playername, "GreatJagRuz", 12);
      entity->func[0] = f_ruzgar;
      entity->func[1] = f_coletazo;
      entity->func[2] = NULL;
      entity->hp = 10000;
      entity->max_hp = 10000;
      return x_c_gjr;
    case Ruzalos:
      strncpy(entity->playername, "Ruzalos", 12);
      entity->func[0] = f_salto;
      entity->func[1] = f_espina;
      entity->func[2] = NULL;
      entity->hp = 20000;
      entity->max_hp = 20000;
      return x_c_ruzalos;
    case Ruiz:
      strncpy(entity->playername, "Ruiz", 12);
      entity->func[0] = f_copia;
      entity->func[1] = f_reprobaton;
      entity->func[2] = f_rm;
      entity->hp = 25000;
      entity->max_hp = 25000;
      return x_c_ruiz;
    default:
      return "AAAAAA ERRORRRR MALA CLASE";
  }
}

      
#pragma region CAZADOR
// ------ FUNCIONES CAZADOR ------

// CAZADOR n[0]
// Sangrado se stackea, hasta 3 veces
// Daña 1000 a enemigo, y deja sangrado de 500 infinito
char* f_estocada(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int objective = enemy_selector(&aliados[posicion_yo], len_enemigos, true); 
  attack(&aliados[posicion_yo], &enemigos[objective], 1000);
  
  if (enemigos[objective].bleed != 's'){
    enemigos[objective].bleed_counter = 0;
  }
  enemigos[objective].bleed_counter += 1;

  return "\e[1;35mHas dado una estocada al monstruo y lo has dejado sangrando, buen movimiento\e[0m";
}

// CAZADOR n[1]
// Daña 3000 a enemigo
char* f_corte_cruzado(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int objective = enemy_selector(&aliados[posicion_yo], len_enemigos, true);
  
  attack(&aliados[posicion_yo], &enemigos[objective], 3000);

  return "\e[1;35mRealizaste un corte al monstruo, honorable cazador\e[0m";
}

// CAZADOR n[2]
// Distrae al monstruo, haciendo que este ataque al último cazador en distraerlo
char* f_distraer(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int objective = enemy_selector(&aliados[posicion_yo], len_enemigos, true);
  enemigos[objective].distracted = true;  // distraer
  enemigos[objective].pos_focused = posicion_yo;

  return "\e[1;35mAcabas de distraer al monstruo\e[0m";
}

#pragma endregion CAZADOR


#pragma region MEDICO
// ------ FUNCIONES MEDICO ------

// MEDICO n[0]
// Cura 2000 a aliado específico (puede ser sí mismo)
char* f_curar(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){

  // elige al aliado, con la func del selector
  int objective = enemy_selector(&aliados[posicion_yo], len_aliados, false);
  
  heal(&aliados[posicion_yo], &aliados[objective], 2000);

  return "\e[1;35mHas curado a uno de tus compañeros\e[0m";
}

// MEDICO n[1]
// Asumimos que se puede curar a si mismo
// Daña entre 750 y 2000 a enemigo aleatorio y cura ceil(daño) a aliado
char* f_destello(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  // entre 0 y 1250, luego le damos offset 750 para [750, 2000]
  int damage = rand() % 1251;
  damage += 750;

  int pos_to_heal = rand() % len_aliados;
  int pos_to_dmg = enemy_selector(&aliados[posicion_yo], len_enemigos, true);

  // ! dañamos al enemigo
  damage = attack(&aliados[posicion_yo], &enemigos[pos_to_dmg], damage);

  // divide en la mitad y redondea hacia arriba (ej: 1001 / 2 + 1001 % 2 = 500 + 1 = 501)
  int healing = damage / 2 + (damage % 2);
  // ! curamos al aliado
  healing = heal(&aliados[posicion_yo], &aliados[pos_to_heal], healing);

  return "\e[1;35mAcabas de lanzar un poderoso destello regenerador\e[0m";
}

// MEDICO n[2]
// Daña 2 * (vida_max - vida) a enemigo aleatorio
char* f_descarga(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){

  Entity* aliado = &aliados[posicion_yo];
  int damage = (aliado->max_hp - aliado->hp) * 2;
  //int enemy_pos = rand() % len_enemigos;
  int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos, true); 
  attack(&aliados[posicion_yo], &enemigos[enemy_pos], damage);

  return "\e[1;35mLiberaste todo tu dolor en una descarga vital contra el monstruo\e[0m";
}

#pragma endregion MEDICO


#pragma region HACKER
// ------ FUNCIONES HACKER ------

// HACKER n[0]
// Asumimos que puede darse efecto a si mismo
// Duplica el daño de un aliado, por 2 turnos
char* f_inyeccion(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){

  int enemy_pos = enemy_selector(&aliados[posicion_yo], len_aliados, false);
  Entity* aliado = &aliados[enemy_pos];
  aliado->buffed = 2; // S'Q'L

  return "\e[1;35mDuplicaste el daño de un aliado con una Inyección SQL\e[0m";
}

// HACKER n[1]
// Daña 1500 a enemigo aleatorio
char* f_ddos(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos, true);
  attack(&aliados[posicion_yo], &enemigos[enemy_pos], 1500);
 
  return "\e[1;35mLanzaste un veloz ataque DDOS, hiciste 1500 de daño al mosntruo\e[0m"; 
}

// HACKER n[2]
// 
char* f_fuerzabruta(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  
  Entity* yo = &aliados[posicion_yo];
  int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos, true); 

  // en la práctica, == 2 es la tercera vez en ejecutar
  if (yo->bruteforce == 2){
    attack(yo, &enemigos[enemy_pos], 10000);
    yo->bruteforce = 0;
  } else {
    yo->bruteforce += 1;
  }
  
  return "\e[1;35mAcabas de utilizar Fuerza Bruta ¿conoces su complejidad?\e[0m";
}

#pragma endregion HACKER


#pragma region JAGRUZ
// ------ FUNCIONES JAGRUZ ------

// JAGRUZ n[0]
// 
char* f_ruzgar(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){

  int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos, true);
  Entity* enemy = &enemigos[enemy_pos];
  attack(&aliados[posicion_yo], enemy, 1000);
  
  return "\e[1;35mGreat JagRuz acaba de utilizar sus gaRaz\e[0m";
}

// JAGRUZ n[1]
// 
char* f_coletazo(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  for (int n_enemy = 0; n_enemy < len_enemigos; n_enemy++){
    Entity* enemy = &enemigos[n_enemy];
    attack(&aliados[posicion_yo], enemy, 500);
  }
  return "\e[1;35mAcabas de ser golpeado por la cola de JagRuz\e[0m";
}

#pragma endregion JAGRUZ


#pragma region RUZALOS
// ------ FUNCIONES RUZALOS ------

// RUZALOS n[0]
// j de jump
char* f_salto(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  
  if (aliados[posicion_yo].jumped){
    // print "no puedes usarla dos veces seguidas!"
    aliados[posicion_yo].jumped = false;
  } else {
    int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos, true);
    Entity* enemy = &enemigos[enemy_pos];
    attack(&aliados[posicion_yo], enemy, 1500);
    aliados[posicion_yo].jumped = true;
  }

  return "\e[1;35mRuzalos ha saltado sobre un alido y ha caído brutalmente sobre elle\e[0m";
}

// RUZALOS n[1]
char* f_espina(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos, true);
  Entity* enemy = &enemigos[enemy_pos];

  aliados[posicion_yo].jumped = false;

  if (enemy->bleed == 'e' && enemy->bleed_counter > 0){
    attack(&aliados[posicion_yo], enemy, 500);
  }else {
    enemy->bleed = 'e';
    enemy->bleed_counter = 3;
  }

  return "\e[1;35mRuzalos usa la espina de su cola para intoxicar ¡Mucho cuidado!\e[0m";

}

#pragma endregion RUZALOS


#pragma region RUIZ

// ------ FUNCIONES RUIZ ------

// RUIZ n[0]
// Copia una habilidad de un enemigo a elección, para usarla en contra
char* f_copia(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos, true);
  int habilidad_a_elegir = rand() % 3;
  ENT_FUNC fn = enemigos[enemy_pos].func[habilidad_a_elegir];
  char* el_pepe = fn(aliados, len_aliados, posicion_yo, enemigos, len_enemigos, auxiliar);

  return "\e[1;35mRuiz acaba de romper el código de honor, este mosntruo no tiene principios\e[0m";
}

// RUIZ n[1]
char* f_reprobaton(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos, true);
  aliados[enemy_pos].reprobado = 2;

  return "\e[1;35mRuiz acaba de reprobar a alguien ¿a ti?\e[0m";

}

// RUIZ n[2]
// TODO: resetear contador de rondas a 0
// RESETEAR AFUERA
char* f_rm(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int damage = auxiliar*100;
  for(int pos_enemigo = 0; pos_enemigo < len_enemigos; pos_enemigo++){
    Entity* enemigo = &enemigos[pos_enemigo];
    attack(&aliados[posicion_yo], enemigo, damage);
  }

  return "\e[1;35mSe han borrado todas las rondas anteriores, este Ruiz y sus trucos\e[0m";
}

#pragma endregion RUIZ

/** Aplica funciones necesarias al final del turno, se debe llamar una vez por cada lista de entidades (una aliada y una enemigia)
 * @param entes Lista de entes, puede ser aliado o enemigo
 * @param len_estes Largo de la lista de entes
*/
void extras_handler(Entity* entes, int len_entes){

  for (int n_ente = 0; n_ente < len_entes; n_ente++){
    Entity* ente = &entes[n_ente];
    // bleed por estocada
    if (ente->bleed == 's'){
      
      ente->hp -= (ente->bleed_counter) * 500;
      if (ente->hp <= 0){
        ente->alive = false;
        ente->hp = 0;
      }
    }
    // bleed por espina
    if(ente->bleed == 'e' && ente->bleed_counter > 0){
      ente->hp -= 400;
      ente->bleed_counter--;
      if (ente->hp <= 0){
        ente->alive = false;
        ente->hp = 0;
      }
    }
    // disminuir la duplicación de daño por sql
    if (ente->buffed) ente->buffed--;
    // disminuir el debuff por reprobado
    if (ente->reprobado) ente->reprobado--;
  }
}

int enemy_selector(Entity* yo, int len_enemigos, bool is_attack){
  if (!len_enemigos) printf("Va a fallar\n");
  // TODO: distraer
  int socket = yo->socket;
  int enemy_pos;
  if (len_enemigos == 1)
  {
    printf("Se ataca al único enemigo disponible\n");
    enemy_pos = 0; // Solo hay un enemigo disponible
  }else {
    /* en el caso que se haga el bonus donde el monstruo es un cliente: (necesitaría un if)
    enemy_pos = request_int(socket, 0, len_enemigos); 
    */
    if (is_attack && yo->distracted){
      enemy_pos = yo->pos_focused;
      yo->distracted = false;
    }else
      enemy_pos = rand() % len_enemigos;
    printf("Se eligió atacar a %i\n", enemy_pos);
  }

  return enemy_pos;
}

/** Realiza ataque
 * @param dano_base es el daño que se haría sin ningun cambio de status
 * @return cantidad de daño realizado
 */
int attack(Entity* atacante, Entity* objetivo, int dano_base){
  int dano_final = dano_base;
  if (atacante->buffed)
    dano_final *= 2; // duplica daño
  if (atacante->reprobado)
    dano_final /= 2; // inflige mitad de daño
  if (objetivo->reprobado)
    dano_final *= 2; // duplica daño
  // ver si se pasa del daño
  if (objetivo->hp < dano_final)
    dano_final = objetivo->hp;
  objetivo->hp -= dano_final;
  if(objetivo->hp <= 0)
    objetivo->alive = false;
  return dano_final;
}

/** Realiza curación
 * @param heal_base es el daño que se haría sin ningun cambio de status
 * @return cantidad de daño realizado
 */
int heal(Entity* healer, Entity* objetivo, int heal_base){
  int heal_final = heal_base;
  assert (objetivo->hp > 0);
  int heal_max = objetivo->max_hp - objetivo->hp;
  if (healer->reprobado){
    heal_final /= 2; // inflige mitad de heal
  }
  if (objetivo->reprobado){
    heal_final /= 2; // recibe mitad heal
  }
  if (heal_final > heal_max){
    heal_final = heal_max;
    objetivo->hp += heal_final;
  }
  return heal_final;
}



// TODO: concatenar strings para retornar
// TODO: init de variables

// bleeds (1: bleed ('s' 'e'))                                             
// distraido (2: distraido, pos_focused)                                             
// reprobaton (1: reprobado)                                             
// contadores (2: bruteforce y jumped)
// buff (1: buffed)

/* 

EFECTOS ESPECIALES:

? debuffs:

CAZADOR
estocada-sangrado 's': realizar sangrado

RUZALOS
espina 'e': hace daño por 3 turnos

CAZADOR
distraer 'd': forzar ataque por distracción

RUIZ
reprobaton 'r': enemigo recibe 50% más y hace 50% menos (curar y dañar)


? buffs:

HACKER
sql 'q': duplicar daño de aliado

? raros:

HACKER
bruteforce: 'b' debe contar 3 veces para usarse 

RUZALOS
salto: se debe usar interacalado

*/
