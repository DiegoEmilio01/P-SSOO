#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "comunication.h"
#include "conection.h"

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
  // Le enviamos a todos los clientes que se conectó uno nuevo
  char welcome_all[24];
  sprintf(welcome_all, "LLegó el cliente %d!!", clientes+1);
  server_send_message(sockets_array[0], 2, welcome_all);

  // Le enviamos al primer cliente un mensaje de bienvenida
  char welcome[24];
  sprintf(welcome, "Bienvenido Cliente %d!!", clientes+1);
  server_send_message(sockets_array[0], 1, welcome);
  printf("cliente conectado\n");


  // Guardaremos los sockets en un arreglo e iremos alternando a quién escuchar.
  int my_attention = 0;
  while (1)
  {
    // Se obtiene el paquete del cliente 1
    int msg_code = server_receive_id(sockets_array[0]);

    if (msg_code == 1) //El cliente me envió un mensaje a mi (servidor)
    {
      char * client_message = server_receive_payload(sockets_array[my_attention]);
      printf("El cliente %d dice: %s\n", my_attention+1, client_message);

      // Le enviaremos el mismo mensaje invertido jeje
      char * response = revert(client_message);

      // Le enviamos la respuesta
      server_send_message(sockets_array[my_attention], 1, response);
    }
    else if (msg_code == 2){ //El cliente le envía un mensaje al otro cliente
      // char * client_message = server_receive_payload(sockets_array[my_attention]);
      // printf("Servidor traspasando desde %d a %d el mensaje: %s\n", my_attention+1, ((my_attention+1)%2)+1, client_message);
      char * client_message = "Esperando a otro cliente";
      server_send_message(sockets_array[my_attention], 2, client_message);
      clientes++;
      get_client(server_socket, sockets_array, clientes);
      sprintf(welcome, "Bienvenido Cliente %d!!", clientes+1);
      server_send_message(sockets_array[clientes], 2, welcome);

      sprintf(welcome_all, "LLegó el cliente %d!!", clientes+1);
      for (int client_n=1; client_n < clientes; client_n++){
        server_send_message(sockets_array[client_n], 2, welcome_all);
      }
      // Mi atención cambia al otro socket
      // my_attention = (my_attention + 1) % 5;
      server_send_message(sockets_array[0], 1, welcome_all);

    }
    printf("------------------\n");
  }

  return 0;
}
