#pragma once
#include <stdlib.h>

#include "clases.h"
#include "comunication.h"
#include "texts.h"


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
// Daña 1000 a enemigo aleatorio, y deja sangrado de 500 infinito
char* f_estocada(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  if (!len_enemigos) printf("Va a fallar\n");
  //int objective = rand() % len_enemigos; //Según enunciado creo que debería ser a un objetivo arbitrario.
  int objective = enemy_selector(&aliados[posicion_yo], len_enemigos); 
  enemigos[objective].hp -= 1000;
  if (enemigos[objective].hp <= 0)
  {
    enemigos[objective].alive = false;
  }
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

// CAZADOR n[1]
// Daña 3000 a enemigo aleatorio
char* f_corte_cruzado(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  if (!len_enemigos) printf("Va a fallar\n");
  //int objective = rand() % len_enemigos;
  //int objective = request_int(aliados[posicion_yo].socket, 0, len_enemigos); 
  int objective = 0;
  enemigos[objective].hp -= 1000;
  if (enemigos[objective].hp <= 0)
  {
    enemigos[objective].alive = false;
  }

  return NULL;
}

// CAZADOR n[2]
// Distrae al monstruo, haciendo que este ataque al último cazador en distraerlo
//TODO: FALTA IMPLEMENTAR
char* f_distraer(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  if (!len_enemigos) printf("Va a fallar\n");
  //int objective = rand() % len_enemigos; 
  int objective = request_int(aliados[posicion_yo].socket, 0, len_enemigos); 
  enemigos[objective].effect_type = 'd';  // distraer
  enemigos[objective].effect_value = posicion_yo;
  return NULL;
}

#pragma endregion CAZADOR


#pragma region MEDICO
// ------ FUNCIONES MEDICO ------

// MEDICO n[0]
// Cura 2000 a aliado específico (puede ser sí mismo)
//TODO: Falta implementar
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

// MEDICO n[1]
// Asumimos que se puede curar a si mismo
// Daña entre 750 y 2000 a enemigo aleatorio y cura ceil(daño) a aliado
char* f_destello(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  // entre 0 y 1250, luego le damos offset 750 para [750, 2000]
  int damage = rand() % 1251;
  int pos_to_heal = rand() % len_aliados;
  int pos_to_dmg = enemy_selector(&aliados[posicion_yo], len_enemigos);
  damage += 750;

  // divide en la mitad y redondea hacia arriba (ej: 1001 / 2 + 1001 % 2 = 500 + 1 = 501)
  int heal = damage / 2 + (damage % 2);
  // curamos al aliado
  Entity* aliado = &aliados[pos_to_heal];
  aliado->hp += heal;
  if (aliado->hp > aliado->max_hp) aliado->hp = aliado->max_hp;
  // dañamos al enemigo
  enemigos[pos_to_dmg].hp -= damage;
  if (enemigos[pos_to_dmg].hp <= 0)
  {
    enemigos[pos_to_dmg].alive = false;
  }

  return NULL;
}

// MEDICO n[2]
// Daña 2 * (vida_max - vida) a enemigo aleatorio
char* f_descarga(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){

  Entity* aliado = &aliados[posicion_yo];
  int damage = (aliado->max_hp - aliado->hp) * 2;
  //int enemy_pos = rand() % len_enemigos;
  int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos); 
  enemigos[enemy_pos].hp -= damage;
  if (enemigos[enemy_pos].hp <= 0)
  {
    enemigos[enemy_pos].alive = false;
  }

  return NULL;
}

#pragma endregion MEDICO


#pragma region HACKER
// ------ FUNCIONES HACKER ------

// HACKER n[0]
// Asumimos que puede darse efecto a si mismo
// Duplica el daño de un aliado, por 2 turnos
char* f_inyeccion(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){

  int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos); 
  Entity* aliado = &aliados[enemy_pos];
  aliado->effect_type = 'q'; // S'Q'L
  aliado->duracion_efecto = 2;
  return NULL;
}

// HACKER n[1]
// Daña 1500 a enemigo aleatorio
char* f_ddos(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos); 
  enemigos[enemy_pos].hp -= 1500;
 
  return NULL; 
}

// HACKER n[2]
// 
char* f_fuerzabruta(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  
  Entity* yo = &aliados[posicion_yo];
  int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos); 

  if (yo->bruteforce < 3){
    yo->bruteforce += 1;
  } else {
    enemigos[enemy_pos].hp -= 10000;
    yo->bruteforce = 0;
  }
  
  return NULL;
}

#pragma endregion HACKER


#pragma region JAGRUZ
// ------ FUNCIONES JAGRUZ ------

// JAGRUZ n[0]
// 
char* f_ruzgar(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){

  int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos);
  Entity* enemy = &aliados[enemy_pos];
  enemy->hp -= 1000;
  
  return NULL;
}

// JAGRUZ n[1]
// 
char* f_coletazo(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  for (int n_enemy = 0; n_enemy < len_enemigos; n_enemy++){
    Entity* enemy = &enemigos[n_enemy];
    enemy->hp -= 500;
  }
  return NULL;
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
    int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos);
    Entity* enemy = &enemigos[enemy_pos];
    enemy->hp -= 1500;
    aliados[posicion_yo].jumped = true;
  }

  return NULL;
}

// RUZALOS n[1]
char* f_espina(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos);
  Entity* enemy = &enemigos[enemy_pos];

  aliados[posicion_yo].jumped = false;

  if (enemy->bleed == 'e'){
    enemy->hp -= 500;
    enemy->effect_contador -= 1;
  }else {
    enemy->bleed = 'e';
    enemy->bleed_counter = 3;
  }

  return NULL;

}

#pragma endregion RUZALOS


#pragma region RUIZ

// ------ FUNCIONES RUIZ ------

// RUIZ n[0]
// Copia una habilidad de un enemigo a elección, para usarla en contra
char* f_copia(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int enemy_pos = enemy_selector(&alados[posicion_yo], len_enemigos);
  int habilidad_a_elegir = rand() % 3;
  ENT_FUNC fn = enemigos[enemy_pos].func[habilidad_a_elegir];
  char* el_pepe = fn(aliados, len_aliados, posicion_yo, enemigos, len_enemigos, auxiliar);

  return NULL;
}

// RUIZ n[1]
char* f_reprobaton(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos);
  aliados[enemy_pos].reprobado = true;

  return NULL;

}

// RUIZ n[2]
// TODO: resetear contador de rondas a 0
// RESETEAR AFUERA
char* f_rm(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int damage = auxiliar;
  for(int pos_enemigo = 0; pos_enemigo < len_aliados; pos_enemigo++){
    Entity* enemigo = &aliados[pos_enemigo];
    enemigo->hp -= damage;
  }

  return NULL;
}

#pragma endregion RUIZ

// Debe ser ejecutado DESPUES del turno
void extras_handler(Entity* aliados, int len_aliados, Entity* enemigos, int len_enemigos){
  /*
  Para
  */
  for (int n_enemigo = 0; n_enemigo < len_enemigos; n_enemigo++){
    //Verificamos si es que el monstrue posee sangrado. Si lo tiene multiplicamos por el núm
    if (enemigos[n_enemigo].effect_type == 's'){
      Entity* enemigo = &enemigos[n_enemigo];
      enemigo->hp -= (enemigo->effect_value) * (enemigos[n_enemigo].accumulative_blood_counter); 
      enemigo->duracion_efecto -= 1;
    }
    if (enemigos[n_enemigo].hp <= 0)
    {
      enemigos[n_enemigo].alive = false;
      printf("Ete m3n ya murió\n");
      continue;
    }
    if(enemigos[n_enemigo].effect_type == 'd' && enemigos[n_enemigo].target_id >= 0 ){
      printf("Enemigo distraído, debe atacar al jugador con índice %d\n", enemigos[n_enemigo].target_id);
      //TODO: Atacar al aliando con índice target_id
      enemigos[n_enemigo].target_id = -1; //Volvemos a setear el target_id en -1 para que no entre a la condición
    }
  }
  //Ahora iteramos sobre los aliados para definir sus efectos
  for (int n_aliado = 0; n_aliado < len_aliados; n_aliado++)
  {
    if (aliados[n_aliado].hp <= 0)
    {
      aliados[n_aliado].alive = false;
      printf("Ete m3n ya murió\n");
      continue;
    }
    // Ahora verificamos si ya pasaron los turnos que duraba el efecto y v
    if(aliados[n_aliado].effect_type == 'q' && aliados[n_aliado].duracion_efecto <= 0)
    {
      aliados[n_aliado].multiplier = 1; //Volvemos a setear el multiplicador en 1
    }
    //Primero revisamos si es que el personaje posee efecto de intección SQL
    if (aliados[n_aliado].effect_type == 'q' && aliados[n_aliado].duracion_efecto > 0)
    {
      // Si tenía multiplicador, entonces determinamos que su multiplicador será == 2
      aliados[n_aliado].multiplier = 2; //TODO:Estoy asumiendo que este multiplicador es para esto, checkear con Chris
      aliados[n_aliado].duracion_efecto -= 1; //Restamos 1 a la cantidad de turnos que durará el efecto
      
    }
    //Si el aliado está distrayendo, el monstruo lo atacará a él en el próximo turno
    if (aliados[n_aliado].distracted)
    {
      //TODO: Setear en -1 cuando ya se haya usado.
      enemigos[0].target_id = n_aliado; // Le damos índice para que ataque después.
    }
  }
  
}

int enemy_selector(Entity* yo, int len_enemigos){

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
    enemy_pos = rand() % len_enemigos; 
    printf("Se eligió atacar a %i\n", enemy_pos);
  }

  return enemy_pos;
}

/**
 *
 * @return cantidad de daño realizado
 */
int atacar(Entity* atacante, Entity* objetivo, int dano_base){
  int dano_final = dano_base;
  if (atacante->buffed)
    dano_final <<= 1; // duplica daño
  if (atacante->reprobado)
    dano_final >>= 1; // infinge mitad de daño
  
}

// TODO: concatenar strings para retornar

// bleeds (1: 's' o 'e')
// distraido (1: 'd')
// reprobaton (??)
// contadores (2: bruteforce y jump)
// buff (1: sql)

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
