#include <stdbool.h>
#include "clases.h"


typedef struct game{
  // 1 si es jugador, -1 si es mounstro, 0 si no existe
  int active_players[5];
  int rounds;
  bool can_have_monster;
  // posicion admin y mounstro
  int pos_admin;  // -1 si es mounstro
  int pos_monster;  // -1 si no existe
  Entity players[4];
  Entity monsters[1];
  int admin_socket;  // para mayor facilidad de acceso, se guarda aqui
} Game;

void send_txt_all(Game game, char* msg);

void send_txt_exclude(Game game, char* msg, int socket_evict);
