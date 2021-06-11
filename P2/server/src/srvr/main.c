#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "comunication.h"
#include "conection.h"
#include "bits.h"
#include "texts.h"
#include "clases.h"

char * NO_TXT = "\0";
const uint8_t TXT_ONLY = 0;
char texto_opciones[] = "¿Qué desea hacer?\n   [1]Enviar mensaje al servidor\n   [2]agregar otro cliente\n";

typedef struct game{
  bool jugadores_activos[5];
  int rondas;
  bool can_have_monster;
  // posicion admin y mounstro
  int pos_admin;
  int pos_monster;
  Entity players[4];
  Entity monsters[1];
} Game;

int main(int argc, char *argv[]){
  // Se define una IP y un puerto
  Game game = (Game){
    .jugadores_activos = {false, false, false, false, false},
    .rondas = 0, .can_have_monster=false,
    .pos_admin = 0, .pos_monster = -1
  };
  char * IP = "0.0.0.0";
  int PORT = 8080;
  printf("Servidor encendido\n");

  // Se crea el servidor y se obtienen los sockets de ambos clientes.
  // Se inicializa una estructura propia para guardar los n°s de sockets de los clientes.
  int sockets_array[5];
  int clientes = 0;
  int server_socket = init_sockets(IP, PORT);
  get_client(server_socket, sockets_array, 0);

  printf("Servidor escuchando\n");
  // le damos la opción al admin si desea jugar con monstruo
  send_txt(sockets_array[0], x_admin_req_monster);
  int choice = request_int(sockets_array[0]);
  printf("choice: %d\n", choice);
  if (choice == 1){
    send_txt(sockets_array[0], x_admin_req_monster_succ);
    game.can_have_monster = true;
  }
  else
    send_txt(sockets_array[0], x_admin_req_monster_fail);
  
  // le preguntamos por su nombre al cliente
  send_txt(sockets_array[0], x_req_nombre);
  char *pname = request_txt(sockets_array[0]);
  strncpy(game.players[0].playername, pname, 12);
  free(pname);
  game.players[0].playername[12] = '\0';
  printf("nombre usuario: %s\n", game.players[0].playername);

  // Le enviamos al primer cliente un mensaje de bienvenida
  char welcome[24];
  sprintf(welcome, "Bienvenido Cliente %d!!", clientes+1);
  printf("cliente conectado\n");

  // Guardaremos los sockets en un arreglo e iremos alternando a quién escuchar.
  int my_attention = 0;
  for(;;)
  {
    int choice = request_int(sockets_array[0]);
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

  return 0;
}
