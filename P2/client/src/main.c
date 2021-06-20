#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
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

char get_char(){
  char response = 'A';
  for(;;){
    char c = getchar();
    if (c == '\n') break;
    response = c;
  }
  return response;
}


int main (int argc, char *argv[]){
  //Se obtiene la ip y el puerto donde está escuchando el servidor (la ip y puerto de este cliente da igual)
  if (argc != 5 || strcmp(argv[1], "-i") || strcmp(argv[3], "-p")){
    printf("\e[1;91mERROR: INPUT INVÁLIDO.\nEl correcto uso del programa es: %s -i <IP> -p [PORT]\e[0m\n", argv[0]);
    return 1;
  }
  char * IP = argv[2];
  int PORT = atoi(argv[4]);

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
      option[0] = get_char();
      option[1] = '\0';
      
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
    else if (bt_get(&msg_code, 2)) { //Recibimos un mensaje que proviene del servidor
      // char * message = client_receive_payload(server_socket);
      // client_send_message(server_socket, (uint8_t)0, NO_TXT);
      // printf("%s", message);
      // free(message);
      client_receive_txt_only(server_socket, msg_code);
    }
    else if (bt_get(&msg_code, 6)){
      char * message = client_receive_payload(server_socket);
      printf("%s", message);
      free(message);
      break;
    }
    // printf("------------------\n");
  }

  // Se cierra el socket
  close(server_socket);
  // free(IP);

  return 0;
}
