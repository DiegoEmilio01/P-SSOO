#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "comunication.h"
#include "conection.h"
#include "bits.h"
#include "texts.h"
#include "game.h"

// const uint8_t TXT_ONLY = 0;
// char texto_opciones[] = "¿Qué desea hacer?\n   [1]Enviar mensaje al servidor\n   [2]agregar otro cliente\n";

int main(int argc, char *argv[]){
  // Se define una IP y un puerto
  Game game = (Game){
    .active_players = {0, 0, 0, 0, 0},
    .rounds = 0, .can_have_monster=false,
    .pos_admin = 0, .pos_monster = -1
  };
  char * IP = "0.0.0.0";
  int PORT = 8080;
  printf("Servidor encendido\n");

  // Se crea el servidor y se obtienen los sockets de ambos clientes.
  // Se inicializa una estructura propia para guardar los n°s de sockets de los clientes.
  // guarda la información del jugador mientras se construye el jugador
  Entity BuildPlayer;
  int clientes = 0;
  int server_socket = init_sockets(IP, PORT);
  printf("Servidor escuchando\n");

  char welcome[24];

  // CONEXION INICIAL
  
  // guarda decicion del jugador
  int choice =-1;
  while (choice != 2){
    // conectar nuevo jugador
    get_client(server_socket, &BuildPlayer.socket);
    clientes++;
    if (clientes == 1){
      send_txt(BuildPlayer.socket, x_admin_start);
    }else{
      send_txt(BuildPlayer.socket, x_member_start);
    }
    // TODO: Dar bienvenida
    printf("cliente conectado\n");
    // avisar llegada a todos
    send_txt_all(game, x_member_joined);

    if (clientes == 1){
      // le damos la opción al admin si desea jugar con monstruo
      game.admin_socket = BuildPlayer.socket;
      send_txt(BuildPlayer.socket, x_admin_req_monster);
      choice = request_int(BuildPlayer.socket, 1, 2);
      printf("choice: %d\n", choice);
      if (choice == 1){
        send_txt(BuildPlayer.socket, x_admin_req_monster_succ);
        game.can_have_monster = true;
      }
      else
        send_txt(BuildPlayer.socket, x_admin_req_monster_fail);
    }
    
    // le preguntamos por su nombre al cliente
    send_txt(BuildPlayer.socket, x_req_nombre);
    char *pname = request_txt(BuildPlayer.socket);
    strncpy(BuildPlayer.playername, pname, 12);
    free(pname);
    BuildPlayer.playername[12] = '\0';
    printf("nombre usuario: %s\n", BuildPlayer.playername);

    // le preguntamos por su clase al cliente
    int option_high_b = 3;
    send_txt(BuildPlayer.socket, x_req_class);
    if (game.can_have_monster && (game.pos_monster == -1)){
      send_txt(BuildPlayer.socket, x_req_class_ex);
      option_high_b++;
    }
    choice = request_int(BuildPlayer.socket, 1, option_high_b);

    // seteamos los valores de la clase


    // cargamos jugador en el juego
    if (choice == 4){  // mounstro
      game.monsters[0] = BuildPlayer;
      game.active_players[clientes-1] = -1;
    }else{
      int guerreros=0;
      for (int i=0; i<=4; i++){
        if (game.active_players[i] == 1)
          guerreros++;
      }
      game.players[guerreros] = BuildPlayer;
      game.active_players[clientes-1] = 1;
    }
    // preguntar si se desean mas jugadores
    send_txt(game.admin_socket, x_req_add_or_start);
    choice = request_int(game.admin_socket, 1, 2);
  }

  int my_attention = 0;
  // Todo el juego
  for(;;){
    break;
    // Pelea contra un mounstro
    for(;;)
    {
      break;
      // int choice = request_int(t_client_socket);
      /*
      // enviarle texto al cliente pidiendo la instruccion
      server_send_and_wait(sockets_array[0], TXT_ONLY, texto_opciones);
      server_send_message(sockets_array[my_attention], REQ_CHAR, NO_TXT);

      // Se obtiene el paquete del cliente 1
      int msg_code = server_receive_id(sockets_array[0]);
      char * char_action = server_receive_payload(sockets_array[my_attention]);
      printf("Recibido mensaje de cliente\n");
      int action = char_action[0] - '0';
      if (action > 9 || action < 0){
        // este es un error, pedirle al cliente input de nuevo
        continue;
      }

      if (action == 1) //El cliente me envió un mensaje a mi (servidor)
      {
        char * client_message = server_receive_payload(sockets_array[my_attention]);
        printf("El cliente %i dice: %s\n", my_attention+1, client_message);

        // Le enviaremos el mismo mensaje invertido jeje
        char * response = revert(client_message);
        server_send_and_wait(sockets_array[my_attention], TXT_ONLY, response);
      }
      if (action == 2){ //El cliente le envía un mensaje al otro cliente
        // char * client_message = server_receive_payload(sockets_array[my_attention]);
        // printf("Servidor traspasando desde %d a %d el mensaje: %s\n", my_attention+1, ((my_attention+1)%2)+1, client_message);
        char * client_message = "Esperando a otro cliente";
        server_send_and_wait(sockets_array[my_attention], 0, client_message);
        clientes++;
        get_client(server_socket, sockets_array, clientes);
        sprintf(welcome, "Bienvenido Cliente %d!!", clientes+1);
        server_send_and_wait(sockets_array[clientes], 0, welcome);

        sprintf(welcome_all, "LLegó el cliente %d!!", clientes+1);
        for (int client_n=0; client_n < clientes; client_n++){
          server_send_and_wait(sockets_array[client_n], TXT_ONLY, welcome_all);
        }
        
      }
      //*/
      printf("------------------\n");
    }
  }
  return 0;
}
