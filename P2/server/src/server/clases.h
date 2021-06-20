#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>




enum classname {
  Cazador, Medico, Hacker, GreatJagRuz, Ruzalos, Ruiz
};

struct entity;


// typedef return_type (*ALIAS) (argtype1, ..., argtype n)
typedef char* (*ENT_FUNC) (struct entity*, int, int, struct entity*, int, int);
// entities1, len_entities1, yo, entities2, len_entities2, auxiliar

typedef struct entity{
  /* FIJO */
  char playername[13];
  int socket;
  int is_monster;

  /* Respecto a la clase, debe resetearse */
  int hp;
  int max_hp;
  ENT_FUNC func[3];
  int n_funciones;
  bool has_name; //Para no preguntar más de una vez
  enum classname class;
  bool alive; // Variable para descartar que hagan algo cuando se tenga que revisar los efectos especiales.

  /* efectos especiales, debe resetearse */
  int pos_focused;
  char bleed; // sangrado por estocada 's', o espinas 'e'
  int bleed_counter;
  bool distracted; // Si fue distraido por el cazador o no.
  int bruteforce; // contador de veces ejecutadas por fuerzabruta
  bool jumped; // intercalador de jump (no se resetea en main)
  int buffed; //  >0 si hay efecto activo por hacker, parte en 2 y se resta 1 cada fin de turno
  int reprobado; // >0 si hay efecto activo por reprobaton, parte en 2 y se resta 1 cada fun de turno


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

// JAGRUZ
/*  */
char* f_ruzgar(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar);
char* f_coletazo(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar);

// RUZALOS
/*  */
char* f_salto(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar);
char* f_espina(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar);
// RUIZ
/*  */
char* f_copia(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar);
char* f_reprobaton(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar);
char* f_rm(Entity* aliados, int len_aliados, int posicion_yo, Entity* enemigos, int len_enemigos, int auxiliar);

/* ---- Extras ---- */
char* class_def(enum classname clases, Entity *entity);
