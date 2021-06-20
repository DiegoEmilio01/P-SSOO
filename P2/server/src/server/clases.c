#pragma once
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "clases.h"
#include "comunication.h"
#include "texts.h"
#include "clases_extra.h"

char 
  *x_c_cazador = "Cazador",
  *x_c_medico = "Medico",
  *x_c_hacker = "Hacker",
  *x_c_gjr = "GreatJagRuz",
  *x_c_ruzalos = "Ruzalos",
  *x_c_ruiz = "Ruiz"
;

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
  int objective = enemy_selector(&aliados[posicion_yo], len_enemigos); 
  int dano_realizado = attack(&aliados[posicion_yo], &enemigos[objective], 1000);
  
  if (enemigos[objective].bleed != 's'){
    enemigos[objective].bleed_counter = 0;
  }
  enemigos[objective].bleed_counter += 1;

  char* sms_raw = "\e[1;35m%s ha dado una estocada a %s y lo ha dejado sangrando con nivel %d.\nHa realizado %d de daño\n\e[0m";
  char* sms = malloc(sizeof(char) * 300);
  sprintf(sms, sms_raw, aliados[posicion_yo].playername, enemigos[objective].playername, enemigos[objective].bleed_counter, dano_realizado);
  return sms;
}

// CAZADOR n[1]
// Daña 3000 a enemigo
char* f_corte_cruzado(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int objective = enemy_selector(&aliados[posicion_yo], len_enemigos);
  
  int damage = attack(&aliados[posicion_yo], &enemigos[objective], 3000);

  char* sms_raw = "\e[1;35m%s ha realizado un corte a %s. \nHa realizado %d daño.\n\e[0m";
  char* sms = malloc(sizeof(char) * 300);
  sprintf(sms, sms_raw, aliados[posicion_yo].playername, enemigos[objective].playername, damage);
  return sms;
}

// CAZADOR n[2]
// Distrae al monstruo, haciendo que este ataque al último cazador en distraerlo
char* f_distraer(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int objective = enemy_selector(&aliados[posicion_yo], len_enemigos);
  enemigos[objective].distracted = true;  // distraer
  enemigos[objective].pos_focused = posicion_yo;

  char* sms_raw = "\e[1;35m%s ha distraido a %s.\n\e[0m";
  char* sms = malloc(sizeof(char) * 300);
  sprintf(sms, sms_raw, aliados[posicion_yo].playername, enemigos[objective].playername);
  return sms;
}

#pragma endregion CAZADOR


#pragma region MEDICO
// ------ FUNCIONES MEDICO ------

// MEDICO n[0]
// Cura 2000 a aliado específico (puede ser sí mismo)
char* f_curar(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){

  // elige al aliado, con la func del selector
  int objective = ally_selector(&aliados[posicion_yo], len_aliados);
  
  int helaed = heal(&aliados[posicion_yo], &aliados[objective], 2000);

  char* sms_raw = "\e[1;35m%s ha curado a %s. \nHa recuperado %d de HP\n\e[0m";
  char* sms = malloc(sizeof(char) * 300);
  sprintf(sms, sms_raw, aliados[posicion_yo].playername, aliados[objective].playername, healed);
  return sms;
}

// MEDICO n[1]
// Asumimos que se puede curar a si mismo
// Daña entre 750 y 2000 a enemigo y cura ceil(daño) a aliado aleatorio
char* f_destello(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  // entre 0 y 1250, luego le damos offset 750 para [750, 2000]
  int damage = rand() % 1251;
  damage += 750;

  int pos_to_heal = rand() % len_aliados;
  int pos_to_dmg = enemy_selector(&aliados[posicion_yo], len_enemigos);

  // ! dañamos al enemigo
  damage = attack(&aliados[posicion_yo], &enemigos[pos_to_dmg], damage);

  // divide en la mitad y redondea hacia arriba (ej: 1001 / 2 + 1001 % 2 = 500 + 1 = 501)
  int healing = damage / 2 + (damage % 2);
  // ! curamos al aliado
  healing = heal(&aliados[posicion_yo], &aliados[pos_to_heal], healing);


  char* sms_raw = "\e[1;35m%s ha usado Destello Regenerador. \nHa realizado %d daño a %s. %s ha recuperado %d de HP.\n\e[0m";
  char* sms = malloc(sizeof(char) * 300);
  sprintf(sms, sms_raw, aliados[posicion_yo].playername, damage, enemigos[pos_to_dmg].playername, aliados[pos_to_heal].playername, healing);
  return sms;
}

// MEDICO n[2]
// Daña 2 * (vida_max - vida) a enemigo
char* f_descarga(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){

  Entity* aliado = &aliados[posicion_yo];
  int damage = (aliado->max_hp - aliado->hp) * 2;
  //int enemy_pos = rand() % len_enemigos;
  int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos); 
  damage = attack(&aliados[posicion_yo], &enemigos[enemy_pos], damage);

  
  char* sms_raw = "\e[1;35m%s ha usado Descarga Vital.\n%s recibe %d daño\n\e[0m";
  char* sms = malloc(sizeof(char) * 300);
  sprintf(sms, sms_raw, aliados[posicion_yo].playername, enemigos[enemy_pos].playername, damage);
  return sms;
}

#pragma endregion MEDICO


#pragma region HACKER
// ------ FUNCIONES HACKER ------

// HACKER n[0]
// Asumimos que puede darse efecto a si mismo
// Duplica el daño de un aliado, por 2 turnos
char* f_inyeccion(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){

  int enemy_pos = ally_selector(&aliados[posicion_yo], len_aliados);
  Entity* aliado = &aliados[enemy_pos];
  aliado->buffed = 2; // S'Q'L

  char* sms_raw = "\e[1;35m%s ha usado Inyección SQL.\n%s recibe doble ataque por 2 turnos\n\e[0m";
  char* sms = malloc(sizeof(char) * 300);
  sprintf(sms, sms_raw, aliados[posicion_yo].playername, aliado->playername);

  return sms;
}

// HACKER n[1]
// Daña 1500 a enemigo
char* f_ddos(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos);
  int damage = attack(&aliados[posicion_yo], &enemigos[enemy_pos], 1500);
 
  char* sms_raw = "\e[1;35m%s ha lanzado ataque DDOs a %s. \nHa realizado %d daño\n\e[0m";
  char* sms = malloc(sizeof(char) * 300);
  sprintf(sms, sms_raw, aliados[posicion_yo].playername, enemigos[enemy_pos].playername, damage);
  return sms;
}

// HACKER n[2]
// 
char* f_fuerzabruta(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  
  Entity* yo = &aliados[posicion_yo];
  int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos); 

  // en la práctica, == 2 es la tercera vez en ejecutar
  if (yo->bruteforce == 2){
    int damage = attack(yo, &enemigos[enemy_pos], 10000);
    yo->bruteforce = 0;
  } else {
    yo->bruteforce += 1;
  }
  
  char* sms_raw = "\e[1;35m%s ha utilizado Fuerza Bruta contra %s. \nHa realizado %d daño\n\e[0m";
  char* sms = malloc(sizeof(char) * 300);
  sprintf(sms, sms_raw, aliados[posicion_yo].playername, enemigos[enemy_pos].playername, damage);
  return sms;
}

#pragma endregion HACKER


#pragma region JAGRUZ
// ------ FUNCIONES JAGRUZ ------

// JAGRUZ n[0]
// 
char* f_ruzgar(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){

  int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos);
  Entity* enemy = &enemigos[enemy_pos];
  int damage = attack(&aliados[posicion_yo], enemy, 1000);
  
  
  char* sms_raw = "\e[1;35m%s ha utilizado ruzgar contra %s. \nHa realizado %d daño.\n\e[0m";
  char* sms = malloc(sizeof(char) * 300);
  sprintf(sms, sms_raw, aliados[posicion_yo].playername, enemigos[enemy_pos].playername, damage);
  return sms;
}

// JAGRUZ n[1]
// 
char* f_coletazo(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  char *str1 = malloc(150*sizeof(char));
  char *str_atqs = calloc(450, sizeof(char));
  for (int n_enemy = 0; n_enemy < len_enemigos; n_enemy++){
    Entity* enemy = &enemigos[n_enemy];
    int dano = attack(&aliados[posicion_yo], enemy, 500);
    srintf(str1, "%s recive %d de daño.\n", enemigo->playername, dano);
    strcat(str_atqs, str1);
  }
  char* sms_raw = "\e[1;35m%s ha utilizado coletazo.\n%s\n\e[0m";
  char* sms = malloc(sizeof(char) * 500);
  sprintf(sms, sms_raw, aliados[posicion_yo].playername, str_atqs);
  free(str1);
  free(str_atqs);
  return sms;
}

#pragma endregion JAGRUZ


#pragma region RUZALOS
// ------ FUNCIONES RUZALOS ------

// RUZALOS n[0]
// j de jump
char* f_salto(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  char* sms = malloc(sizeof(char) * 300);


    sprintf(sms, sms_raw, aliados[posicion_yo].playername);
  if (aliados[posicion_yo].jumped){
    // print "no puedes usarla dos veces seguidas!"
    aliados[posicion_yo].jumped = false;
    char* sms_raw = "\e[1;35m%s intenta de usar salto, pero falla\e[0m";
  } else {
    int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos);
    Entity* enemy = &enemigos[enemy_pos];
    int damage = attack(&aliados[posicion_yo], enemy, 1500);
    aliados[posicion_yo].jumped = true;
    
    char* sms_raw = "\e[1;35m%s ha saltado sobre %s.\nHa realizado %d daño.\n\e[0m";
    sprintf(sms, sms_raw, aliados[posicion_yo].playername, enemigos[enemy_pos].playername, damage);
    return sms;

}

// RUZALOS n[1]
char* f_espina(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos);
  Entity* enemy = &enemigos[enemy_pos];

  aliados[posicion_yo].jumped = false;
  char* sms = malloc(sizeof(char) * 300);

  if (enemy->bleed == 'e' && enemy->bleed_counter > 0){
    int damage = attack(&aliados[posicion_yo], enemy, 500);
    char* sms_raw = "\e[1;35m%s ha usado espina venenosa contra %s.\nAl ya estar envenenado, recive %d de daño.\n\e[0m";
    sprintf(sms, sms_raw, aliados[posicion_yo].playername, enemigos[enemy_pos].playername, damage);
  }else {
    char* sms_raw = "\e[1;35m%s ha usado espina venenosa contra %s, quedando intoxicado.\n\e[0m";
    sprintf(sms, sms_raw, aliados[posicion_yo].playername, enemigos[enemy_pos].playername);
    enemy->bleed = 'e';
    enemy->bleed_counter = 3;
  }
  return sms;
}

#pragma endregion RUZALOS


#pragma region RUIZ

// ------ FUNCIONES RUIZ ------

// RUIZ n[0]
// Copia una habilidad de un enemigo a elección, para usarla en contra
char* f_copia(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos);
  int habilidad_a_elegir = rand() % 3;
  ENT_FUNC fn = enemigos[enemy_pos].func[habilidad_a_elegir];
  char* tx_otra_fn = fn(aliados, len_aliados, posicion_yo, enemigos, len_enemigos, auxiliar);

  char* sms_raw = "\e[1;35m%s ha copiado la habilidad numero %d de %s.\n%s\e[0m";
  char* sms = malloc(sizeof(char) * 400);
  sprintf(sms, sms_raw, aliados[posicion_yo].playername, habilidad_a_elegir+1, enemigos[enemy_pos].playername, tx_otra_fn);
  free(tx_otra_fn);
  return sms;
}

// RUIZ n[1]
char* f_reprobaton(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos);
  aliados[enemy_pos].reprobado = 2;

  char* sms_raw = "\e[1;35m%s ha reprobado a %s.\n\e[0m";
  char* sms = malloc(sizeof(char) * 300);
  sprintf(sms, sms_raw, aliados[posicion_yo].playername, enemigos[enemy_pos].playername);
  return sms;

}

// RUIZ n[2]
// TODO: resetear contador de rondas a 0
// RESETEAR AFUERA
char* f_rm(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int damage = auxiliar*100;
  char *str1 = malloc(150*sizeof(char));
  char *str_atqs = calloc(450, sizeof(char));
  for(int pos_enemigo = 0; pos_enemigo < len_enemigos; pos_enemigo++){
    Entity* enemigo = &enemigos[pos_enemigo];
    int dano = attack(&aliados[posicion_yo], enemigo, damage);
    srintf(str1, "%s recive %d de daño.\n", enemigo->playername, dano);
    strcat(str_atqs, str1);
  }

  char* sms_raw = "\e[1;35m%s ha borrado todas las rondas.\n%s\e[0m";
  char* sms = malloc(sizeof(char) * 500);
  sprintf(sms, sms_raw, aliados[posicion_yo].playername, str_atqs);
  free(str1);
  free(str_atqs);
  return sms;
}

#pragma endregion RUIZ


// TODO: concatenar strings para retornar
// TODO: init de variables

// bleeds (1: bleed ('s' 'e'))                                             
// distraido (2: distraido, pos_focused)                                             
// reprobaton (1: reprobado)                                             
// contadores (2: bruteforce y jumped)
// buff (1: buffed)
