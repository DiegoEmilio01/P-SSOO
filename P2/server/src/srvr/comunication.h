#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdint.h>

int server_receive_id(int client_socket);
char * server_receive_payload(int client_socket);
void send_txt(int client_socket, uint8_t pkg_id, char * message);
int request_int(int client_socket);
