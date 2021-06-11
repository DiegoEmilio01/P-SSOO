#pragma once
#include <stdint.h>
#include <stdio.h>

enum classname {
  Cazador, Medico, Hacker, GreatJagRuz, Ruzalos, Ruiz
};

struct entity;


// typedef return_type (*ALIAS) (argtype1, ..., argtype n)
typedef char* (*ENT_FUNC) (struct entity*, int, int, struct entity*, int, int);
// yo, entities1, len_entities1, entities2, len_entities1

typedef struct entity{
  int hp;
  int max_hp;
  int self_contador;  // contador de habilidades propias, que cambian con su cantidad de usos
  int effect_contador;  // contador de cambios de status
  int effect_value;  // valor del efecto
  char effect_type;  // que fue afectado (puede ser en vez un enum)
  ENT_FUNC func[3];
  int n_funciones;
  // enum classname cls;
  char* classname;
  // int usos_estocada;
  // int usos_fza_bruta;
  // int atq_buf;
  // int turns_atq_buf_left;
  char playername[13];
  int is_monster;
  int socket;
} Entity;

// CAZADOR
char* f_estocada(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo);
char* f_corte_cruzado(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo);
char* f_distraer(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo);

// MÉDICO
char* f_curar(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo);
char* f_destello(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo);
char* f_descarga(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo);

// HACKER
char* f_inyeccion(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int socket_yo);
