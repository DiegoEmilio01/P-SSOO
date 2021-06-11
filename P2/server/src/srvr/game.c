#include "game.h"
#include "comunication.h"
/** Envia un mensaje a todos los jugadores instanciados
 * 
 * 
 */
void send_txt_all(Game game, char* msg){
  int counter_guerreros = 0;
  for (int i=0; i<5; i++){
    switch (game.active_players[i])
    {
    case 1:
      send_txt(game.players[counter_guerreros].socket, msg);
      counter_guerreros++;
      break;
    default:
      send_txt(game.monsters[0].socket, msg);
      break;
    }
  }
}

/** Envia un mensaje a todos los jugadores instanciados menos a uno
 * 
 * @param socket_evict Valor del socket a evitar
 */
void send_txt_exclude(Game game, char* msg, int socket_evict){
  int counter_guerreros = 0;
  int socket_to_send;
  for (int i=0; i<5; i++){
    switch (game.active_players[i])
    {
    case 1:
      socket_to_send = game.players[counter_guerreros].socket;
      if (socket_to_send = socket_evict)
        send_txt(socket_to_send, msg);
      counter_guerreros++;
      break;
    case -1:
      socket_to_send = game.monsters[0].socket;
      if (socket_to_send = socket_evict)
        send_txt(socket_to_send, msg);
      send_txt(socket_to_send, msg);
      break;
    }
  }
}
