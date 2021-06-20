#pragma once
#include <stdbool.h>
#include "clases.h"
#include <unistd.h>
#include <sys/select.h>
#include "game.h"

typedef struct game{
  // 1 si es jugador, -1 si es mounstro, 0 si no existe
  int active_players[5];
  int n_alive;
  int n_dead; // jugadores muertos
  int rondas;
  bool can_have_monster;
  // posicion admin y mounstro
  int pos_admin;  // -1 si es mounstro
  int pos_monster;  // -1 si no existe
  Entity players[4];
  Entity dead_players[4];
  Entity monsters[1];
  int jugadores_inicializados_totalmente;
  bool game_start; //regula el flujo
  int admin_socket;  // para mayor facilidad de acceso, se guarda aqui
} Game;

void game_start(Game *game);

void send_txt_all(Game game, char* msg);

void send_txt_exclude(Game game, char* msg, int socket_evict);

//Retorna las opciones de movimientos del usuario
char *objetivos_habilidad(Game game, int pos_jugador, int movimiento, char* buffer_aux);

char *movimientos_jugador(int class, char *buffer_aux);

void game_entities_init(Game game, int clientes_maximos);

void kill_player(Game *game, int player_id, int mode);

void play_again(Game *_game);
