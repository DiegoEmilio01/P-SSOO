#include <unistd.h>
#include <stdio.h>
#include "conection.h"
#include "comunication.h"
#include "bits.h"

char * get_input(){
  char * response = malloc(14);
  int pos=0;
  while (1){
    char c = getchar();
    if (c == '\n') break;
    if (pos > 12) continue;
    response[pos] = c;
    if (pos == 12) response[13] = '\0';
    pos++;
  }
  if (pos < 12)
    response[pos] = '\0';
  return response;
}

char *NO_TXT = "\0";

int main (int argc, char *argv[]){
  //Se obtiene la ip y el puerto donde está escuchando el servidor (la ip y puerto de este cliente da igual)
  char * IP = "0.0.0.0";
  int PORT = 8080;

  // Se prepara el socket
  int server_socket = prepare_socket(IP, PORT);

  // Se inicializa un loop para recibir todo tipo de paquetes y tomar una acción al respecto
  while (1){
    uint8_t msg_code = client_receive_id(server_socket);
    // printf("server RCV:, %d\n", msg_code);

    if (bt_get(&msg_code, 1)) { // servidor pide char
      char * message = client_receive_payload(server_socket);
      client_send_message(server_socket, (uint8_t)0, NO_TXT);
      printf("%s", message);
      free(message);

      char option[2];
      option[0] = getchar();
      option[1] = '\0';
      getchar();
      
      client_send_message(server_socket, (uint8_t)0, option);      
    }
    else if (bt_get(&msg_code, 0)) { 
      // servidor pide string de vuelta
      char * message = client_receive_payload(server_socket);
      client_send_message(server_socket, (uint8_t)0, NO_TXT);
      printf("%s", message);
      free(message);

      char * response = get_input();
      client_send_message(server_socket, (uint8_t)0, response);
      free(response);
      
    }
    else if (msg_code == 0) { //Recibimos un mensaje que proviene del servidor
      char * message = client_receive_payload(server_socket);
      client_send_message(server_socket, (uint8_t)0, NO_TXT);
      printf("%s", message);
      free(message);
      // client_send_message(server_socket, (uint8_t)0, NO_TXT);
    }
    // printf("------------------\n");
  }

  // Se cierra el socket
  close(server_socket);
  free(IP);

  return 0;
}
