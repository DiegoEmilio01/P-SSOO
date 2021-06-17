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
      return x_c_cazador;
    case Medico:
      entity->func[0] = f_curar;
      entity->func[1] = f_destello;
      entity->func[2] = f_descarga;
      return x_c_medico;
    case Hacker:
      entity->func[0] = f_inyeccion;
      entity->func[1] = f_ddos;
      entity->func[2] = f_fuerzabruta;
      return x_c_hacker;
    case GreatJagRuz:
      entity->func[0] = f_ruzgar;
      entity->func[1] = f_coletazo;
      entity->func[2] = NULL;
      return x_c_gjr;
    case Ruzalos:
      entity->func[0] = f_salto;
      entity->func[1] = f_espina;
      entity->func[2] = NULL;
      return x_c_ruzalos;
    case Ruiz:
      entity->func[0] = f_copia;
      entity->func[1] = f_reprobaton;
      entity->func[2] = f_rm;
      return x_c_ruiz;
    default:
      return "AAAAAA ERRORRRR MALA CLASE";
  }
}
/* 
TODO:
- cambiar las weas de null (arriba -> nombre de func)
- avanzar en JAGRUZ
 */
      
#pragma region CAZADOR
// ------ FUNCIONES CAZADOR ------

// CAZADOR n[0]
// Sangrado se stackea, hasta 3 veces
// Daña 1000 a enemigo aleatorio, y deja sangrado de 500 infinito
char* f_estocada(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  if (!len_enemigos) printf("Va a fallar\n");
  //int objective = rand() % len_enemigos; //Según enunciado creo que debería ser a un objetivo arbitrario.
  int objective = request_int(aliados[posicion_yo].socket, 0, len_enemigos); 
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

// CAZADOR n[1]
// Daña 3000 a enemigo aleatorio
char* f_corte_cruzado(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  if (!len_enemigos) printf("Va a fallar\n");
  //int objective = rand() % len_enemigos;
  int objective = request_int(aliados[posicion_yo].socket, 0, len_enemigos); 
  enemigos[objective].hp -= 1000;

  return NULL;
}

// CAZADOR n[2]
// Distrae al monstruo, haciendo que este ataque al último cazador en distraerlo
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

// MEDICO n[2]
// Daña 2 * (vida_max - vida) a enemigo aleatorio
char* f_descarga(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  if (!len_enemigos) printf("Va a fallar\n");

  Entity* aliado = &aliados[posicion_yo];
  int damage = (aliado->max_hp - aliado->hp) * 2;
  //int enemy_pos = rand() % len_enemigos;
  int enemy_pos = request_int(aliados[posicion_yo].socket, 0, len_enemigos); 
  enemigos[enemy_pos].hp -= damage;

  return NULL;
}

#pragma endregion MEDICO


#pragma region HACKER
// ------ FUNCIONES HACKER ------

// HACKER n[0]
// Asumimos que puede darse efecto a si mismo
// Duplica el daño de un aliado, por 2 turnos
char* f_inyeccion(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  if (!len_enemigos) printf("Va a fallar\n");

  //int pos_enemigo = rand() % len_aliados;
  int pos_enemigo = request_int(aliados[posicion_yo].socket, 0, len_aliados); 
  Entity* aliado = &aliados[pos_enemigo];
  aliado->effect_type = 'q'; // S'Q'L
  aliado->duracion_efecto = 2;
  return NULL;
}

// HACKER n[1]
// Daña 1500 a enemigo aleatorio
char* f_ddos(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  if (!len_enemigos) printf("Va a fallar\n");
  //Ahora vamos a darle al jugador que va a usar esta opción, la posibilidad de escoger el enemigo
  if (len_enemigos == 1)
  {
    printf("Se ataca al único enemigo disponible\n");
    //int enemy_pos = rand() % len_enemigos;
    int enemy_pos = 0; // Solo hay un enemigo disponible
    enemigos[enemy_pos].hp -= 1500;
  }else{
    int enemigo_objetivo = request_int(aliados[posicion_yo].socket, 0, len_enemigos); 
    enemigos[enemigo_objetivo].hp -= 1500;
  }
  return NULL; //TODO: Qué debería retornar esta funcion?
  
  
  
}

// HACKER n[2]
// 
char* f_fuerzabruta(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  if (!len_enemigos) printf("Va a fallar\n");
  
  Entity* yo = &aliados[posicion_yo];
  int enemy_pos = request_int(aliados[posicion_yo].socket, 0, len_enemigos); 
  //int enemy_pos = rand() % len_enemigos;

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

// JAGRUZ n[0]
// 
char* f_ruzgar(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int enemy_pos = rand() % len_enemigos;
  Entity* enemy = &enemigos[enemy_pos];
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
// j de jump xd
char* f_salto(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  
  if (aliados[posicion_yo].jumped){
    // print "no puedes usarla dos veces seguidas!"
    aliados[posicion_yo].jumped = false;
  } else {
    int enemy_pos = rand() % len_enemigos;
    Entity* enemy = &enemigos[enemy_pos];
    enemy->hp -= 1500;
    aliados[posicion_yo].jumped = true;
  }

  return NULL;
}

// RUZALOS n[1]
char* f_espina(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int enemy_pos = rand() % len_enemigos;
  Entity* enemy = &enemigos[enemy_pos];

  aliados[posicion_yo].jumped = false;

  if (enemy->effect_type == 'e'){
    enemy->hp -= 500;
    enemy->effect_contador -= 1;
  } else {
    enemy->effect_type = 'e';
    enemy->effect_value = 400;
    enemy->effect_contador = 3;
  }

  return NULL;

}

#pragma endregion RUZALOS


#pragma region RUIZ

// ------ FUNCIONES RUIZ ------

// RUIZ n[0]
// Copia una habilidad de un enemigo a elección, para usarla en contra
char* f_copia(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  // int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos);
  int enemy_pos = rand() % len_enemigos;
  int habilidad_a_elegir = rand() % 3;
  // TODO: a quién ataca?
  ENT_FUNC fn = enemigos[enemy_pos].func[habilidad_a_elegir];
  // TODO: concatenar strings para retornar
  fn(aliados, len_aliados, posicion_yo, enemigos, len_enemigos, auxiliar);

  return NULL;
}

// RUIZ n[1]
char* f_reprobaton(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  
  // int enemy_pos = enemy_selector(&aliados[posicion_yo], len_enemigos);
  int enemy_pos = rand() % len_enemigos;

  enemigos[enemy_pos].effect_type = 'r';
  enemigos[enemy_pos].effect_contador = 0;
  enemigos[enemy_pos].effect_value = 0;

  return NULL;
}

// RUIZ n[2]
// TODO: resetear contador de rondas a 0
// RESETEAR AFUERA
char* f_rm(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar){
  int damage = auxiliar;
  for(int pos_enemigo = 0; pos_enemigo < len_enemigos; pos_enemigo++){
    Entity* enemigo = &enemigos[pos_enemigo];
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
    if (enemigos[n_enemigo].effect_type ==s ''){
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
    if (aliados[n_aliado].distracting)
    {
      //TODO: Setear en -1 cuando ya se haya usado.
      enemigos[0].target_id = n_aliado; // Le damos índice para que ataque después.
    }
  }
  
}

int enemy_selector(Entity* yo, int len_enemigos){
  // TODO: distraer
  int socket = yo->socket;
  int enemy_pos;
  if (len_enemigos == 1)
  {
    printf("Se ataca al único enemigo disponible\n");
    enemy_pos = 0; // Solo hay un enemigo disponible
  }else{
    enemy_pos = request_int(socket, 0, len_enemigos); 
  }

  return enemy_pos;
}

/* 
TODO: inicializar clases, es decir volver las variables en 0 (?), porque los structs están en stack (??)
TODO: cambiar elección de enemigo (en caso que enemigos > 1) 

EFECTOS ESPECIALES:
Estocada:     's': realizar sangrado
Distraer:     'd': forzar ataque por distracción
SQL:          'q': duplicar daño de aliado
FuerzaBruta:  'f': si se usa 3 veces, causa 10.000 de daño
Reprobaton:   'r':
Espina:       'e': hace daño por 3 turnos
*/

// #pragma region


// #pragma endregion