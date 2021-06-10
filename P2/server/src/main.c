#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "comunication.h"
#include "conection.h"
#include "bits.h"
#include "texts.h"

char * NO_TXT = "\0";
const uint8_t TXT_ONLY = 0;
char texto_opciones[] = "¿Qué desea hacer?\n   [1]Enviar mensaje al servidor\n   [2]agregar otro cliente\n";


char * revert(char * message){
  //Se invierte el mensaje

  int len = strlen(message) + 1;
  char * response = malloc(len);

  for (int i = 0; i < len-1; i++)
  {
    response[i] = message[len-2-i];
  }
  response[len-1] = '\0';
  return response;
}

int main(int argc, char *argv[]){
  // Se define una IP y un puerto
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
  send_txt(sockets_array[0], TXT_ONLY, x_admin_req_monster);
  int choice = request_int(sockets_array[0]);
  if (choice == 1)
    send_txt(sockets_array[0], TXT_ONLY, x_admin_req_monster_succ);
  else
    send_txt(sockets_array[0], TXT_ONLY, x_admin_req_monster_fail);


  // Le enviamos al primer cliente un mensaje de bienvenida
  char welcome[24];
  sprintf(welcome, "Bienvenido Cliente %d!!", clientes+1);
  printf("cliente conectado\n");

  // Guardaremos los sockets en un arreglo e iremos alternando a quién escuchar.
  int my_attention = 0;
  for(;;)
  {
    send_txt(sockets_array[0], TXT_ONLY, x_req_nombre);  // en lugar equivocado, cambiar luego
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
