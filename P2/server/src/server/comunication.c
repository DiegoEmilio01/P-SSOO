#pragma once
#include <string.h>
#include "comunication.h"
#include "bits.h"
#include "texts.h"

char 
  *x_input = "$> ",
  *x_req_char_err = "\e[1;91mOpcion incorrecta, \e[0mrecuerda que debe ser entre %i y %i, inclusive\n"
;

//LINKS REFERENCIAS
//https://pubs.opengroup.org/onlinepubs/009695399/functions/recv.html
//https://pubs.opengroup.org/onlinepubs/009695399/functions/send.html

int server_receive_id(int client_socket){
  // Se obtiene solamente el ID del mensaje
  int id = 0;
  recv(client_socket, &id, 1, 0);
  return id;
}

char * server_receive_payload(int client_socket){
  // Se obtiene el largo del payload
  int len = 0;
  recv(client_socket, &len, 1, 0);
  // Se obtiene el payload
  char * payload = malloc(len);
  int received = recv(client_socket, payload, len, 0);
  // Se retorna
  return payload;
}

void server_send_message(int client_socket, int pkg_id, char * message){
  int payloadSize = strlen(message) + 1;
  //printf("payload size: %d\n", payloadSize);
  // Se arma el paquete
  char msg[1+1+payloadSize];
  msg[0] = pkg_id;
  msg[1] = payloadSize;
  memcpy(&msg[2], message, payloadSize);
  // Se envía el paquete
  send(client_socket, msg, 2+payloadSize, 0);
}

// espera a que el cliente acuse recibo para seguir
void server_send_and_wait(int client_socket, int pkg_id, char * message){
  server_send_message(client_socket, pkg_id, message);
  server_receive_id(client_socket);
  free(server_receive_payload(client_socket));
}


/** Envía un string de largo cualquiera de a poco. Al momento de este comentario, el cliente no sabe recibir largos mayores a 255
 * 
 */
void send_txt(int client_socket, char * message){
  uint8_t pkg_id = 0;
  // indicar solo envío de texto
  bt_set(&pkg_id, 2, 1);
  // separar el texto en grupos de 255 elementos
  int len = strlen(message) + 1;
  if (len > 255)  // recordar el largo de linea
    bt_set(&pkg_id, 7, 1);
  char msg_buffer[255];
  while (len > 255){
    strncpy(msg_buffer, message, 254);
    msg_buffer[255] = '\0';
    server_send_and_wait(client_socket, pkg_id, msg_buffer);
    message += 254;
    len -= 254;
  }
  bt_set(&pkg_id, 7, 0);
  server_send_and_wait(client_socket, pkg_id, message);
}

/** Pide un char del usuario, debe ser tal que low_b <= opcion <= high_b
 * @param low_b Indica el valor valido minimo inclusive
 * @param high_b Indica el valor valido maximo inclusive
 * @returns int que corresponde al numero indicado por el usuario
 */
int request_int(int client_socket, int low_b, int high_b){
  uint8_t pkg_id=0;
  bt_set(&pkg_id, 1, 1);
  int opcion = -10;
  while (!(low_b <= opcion && opcion <= high_b)){
    server_send_and_wait(client_socket, pkg_id, x_input);
    server_receive_id(client_socket);
    char * recv = server_receive_payload(client_socket);
    printf("recv[0]: %d\n", recv[0]);
    opcion = recv[0]-'0';
    free(recv);
    if (!(low_b <= opcion && opcion <= high_b)){
      char *err_msg = malloc(255);
      sprintf(err_msg, x_req_char_err, low_b, high_b);
      send_txt(client_socket, err_msg);
      free(err_msg);
    }

  }
  return opcion;
}

char* request_txt(int client_socket){
  uint8_t pkg_id=0;
  bt_set(&pkg_id, 0, 1);
  server_send_and_wait(client_socket, pkg_id, x_input);
  server_receive_id(client_socket);
  char * recv = server_receive_payload(client_socket);
  return recv;
}
