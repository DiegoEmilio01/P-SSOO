#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdint.h>

int server_receive_id(int client_socket);
// char * server_receive_payload(int client_socket);
void send_txt(int client_socket, char * message);
int request_int(int client_socket, int low_b, int high_b);
char* request_txt(int client_socket);
