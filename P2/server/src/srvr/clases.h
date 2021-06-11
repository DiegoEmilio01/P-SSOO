#pragma once
#include <stdint.h>
#include <stdio.h>

enum classname {
  Cazador, Medico, Hacker, GreatJagRuz, Ruzalos, Ruiz
};

struct entity;


// typedef return_type (*ALIAS) (argtype1, ..., argtype n)
typedef char* (*ENT_FUNC) (struct entity*, int, int, struct entity*, int, int);
// F_FUNC (aliados, len_aliados, posicion_yo, enemigos, len_enemigos, auxiliar)


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

/* ----- CLASS FUNCTIONS ----- */
// CAZADOR
/*  */
char* f_estocada(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar);
char* f_corte_cruzado(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar);
char* f_distraer(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar);

// MÉDICO
/*  */
char* f_curar(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar);
char* f_destello(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar);
char* f_descarga(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar);

// HACKER
/*  */
char* f_inyeccion(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar);
char* f_ddos(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar);
char* f_fuerzabruta(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar);
