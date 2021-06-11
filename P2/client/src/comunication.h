#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

static char *NO_TXT = "\0";

void client_send_message(int client_socket, int pkg_id, char * message);
int client_receive_id(int client_socket);
char * client_receive_payload(int client_socket);
void client_receive_txt_only(int client_socket, uint8_t msg_code);
