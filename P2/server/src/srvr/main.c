#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
//#include <sys/select.h>
#include "comunication.h"
#include "conection.h"
#include "bits.h"
#include "texts.h"
#include "clases.h"
#include "game.h"

char *NO_TXT = "\0";
const uint8_t TXT_ONLY = 0;
char texto_opciones[] = "\e[1;33m¿Qué desea hacer?\n   \e[0m[1]Enviar mensaje al servidor\n   [2]agregar otro cliente\n";


int main(int argc, char *argv[])
{
  // Se define una IP y un puerto
  Game game = (Game){
      .active_players = {false, false, false, false, false},
      .dead_players = {false, false, false, false},
      .n_alive = 0,
      .n_dead = 0,
      .rondas = 0,
      .can_have_monster = false,
      .pos_admin = 0,
      .game_start = false,
      .pos_monster = -1};
  char *IP = "0.0.0.0";
  int PORT = 8080;
  printf("\e[1;34mServidor encendido\n");

  // Cosas X

  fd_set readfds;
  int max_sd;
  int maximo_clientes = 4;
  int activity, valread, addrlen , new_socket;
  char buffer[1025];
  struct sockaddr_in address;
  /*
  fd_set readfds;
  // Clear an fd_set
  FD_ZERO(&readfds);  
  // Add a descriptor to an fd_set
  FD_SET(master_sock, &readfds);   
  // Remove a descriptor from an fd_set
  FD_CLR(master_sock, &readfds); 
  //If something happened on the master socket , then its an incoming connection  
  FD_ISSET(master_sock, &readfds);
  */

  // Se crea el servidor y se obtienen los sockets de ambos clientes.
  // Se inicializa una estructura propia para guardar los n°s de sockets de los clientes.
  // guarda la información del jugador mientras se construye el jugador
  //Entity BuildPlayer;
  int clientes = 0;
  int server_socket = init_sockets(IP, PORT);
  printf("\e[1;34mServidor escuchando\n");
  
  // CONEXION INICIAL
  // guarda decisión del jugador
  int choice;
  for (;;)
  {
    //clear the socket set
    FD_ZERO(&readfds);

    //add master socket to set
    FD_SET(server_socket, &readfds);
    max_sd = server_socket;
    //Acá creo que tengo que mover esto a 3/4
    //get_client(server_socket, &BuildPlayer.socket);

    for (int i = 0; i < maximo_clientes; i++)
    {
      //if valid socket descriptor then add to read list
      if (game.players[i].socket > 0)
        FD_SET(game.players[i].socket, &readfds);

      //highest file descriptor number, need it for the select function
      //max_sd es el socket de servidor
      if (game.players[i].socket > max_sd)
        max_sd = game.players[i].socket;
    }

    //wait for an activity on one of the sockets , timeout is NULL ,
    //so wait indefinitely
    activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
    if ((activity < 0) && (errno != EINTR))
    {
      printf("select error");
    }
    //RECIÉN AGREGADO
    //If something happened on the master socket ,
    //then its an incoming connection
    if (FD_ISSET(server_socket, &readfds))
    {
      if ((new_socket = accept(server_socket,
                               (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
      {
        perror("accept");
        exit(EXIT_FAILURE);
      }

      //inform user of socket number - used in send and receive commands
      printf("\e[1;34mNew connection , socket fd is %d , ip is : %s , port : %d\n " , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

      //send new connection greeting message
      char welcome[100];
      sprintf(welcome, "\e[1;33mBienvenido Cliente %d!!\n", clientes);
      clientes += 1;
      //char hola[] = "Bienvenido Cliente 0!!\n";
      send_txt(new_socket, welcome);
      // if (send(new_socket, welcome, strlen(welcome), 0) != strlen(welcome))
      // {
      //   perror("send");
      // }

      puts("\e[1;34mWelcome message sent successfully");

      //add new socket to array of sockets
      game.players[0].socket = new_socket;
      printf("\e[1;34mAdding to list of sockets as 0\n"); //Le da al admin el primer socket
      game.players[0].has_name = false;
      game.players[0].class = -1;
      
      while (clientes < maximo_clientes  ) //Cantidad máxima clientes 
      {
      for (int i = 0; i < maximo_clientes; i++)
      {
        //printf("Iterando aca\n");
        //if position is empty
        
        if (game.players[i].socket == 0)
        {
          
          game.players[i].has_name = false;
          game.players[i].class = -1;
          if (FD_ISSET(server_socket, &readfds))
          {
            if ((new_socket = accept(server_socket,
                                     (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
            {
              perror("accept");
              exit(EXIT_FAILURE);
            }

            //inform user of socket number - used in send and receive commands
            printf("\e[1;34mNew connection , socket fd is %d , ip is : %s , port : %d\n " , new_socket , inet_ntoa(address.sin_addr) , ntohs
                   (address.sin_port));

            //send new connection greeting message
            char welcome[1024];
            sprintf(welcome, "\e[1;33mBienvenido Cliente %d!!\n", clientes);
            clientes += 1;
            // printf("cliente conectado\n");
            send_txt(new_socket, welcome);
            // if (send(new_socket, welcome, strlen(welcome), 0) != strlen(welcome))
            // {
            //   perror("send");
            // }

            puts("\e[1;34mWelcome message sent successfully");
          
            //add new socket to array of sockets
            for (int i = 0; i < maximo_clientes; i++)
            {
              //if position is empty
              if (game.players[i].socket == 0)
              {
                game.players[i].socket = new_socket;
                printf("\e[1;34mAdding to list of sockets as %d\n", i);
                break;
                
              }
            }
          }

          //else its some IO operation on some other socket
          for (int i = 0; i < maximo_clientes; i++)
          {
            if (FD_ISSET(game.players[i].socket, &readfds))
            {
              //Check if it was for closing , and also read the
              //incoming message
              if ((valread = read(game.players[i].socket, buffer, 1024)) == 0)
              {
                //Somebody disconnected , get his details and print
                getpeername(game.players[i].socket, (struct sockaddr *)&address,
                            (socklen_t *)&addrlen);
                printf("\e[1;34mHost disconnected , ip %s , port %d \n",
                       inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                //Close the socket and mark as 0 in list for reuse
                close(game.players[i].socket);
                game.players[i].socket = 0;
              }

              //Echo back the message that came in
              else
              {
                //set the string terminating NULL byte on the end
                //of the data read
                buffer[valread] = '\0';
                send(game.players[i].socket, buffer, strlen(buffer), 0);
              }
            }
          
          
          break;
        }
        }
      }
      }
    }

    //else its some IO operation on some other socket
    // Acá se mete la lógica? Sí
    for(;;)
    {

    
    for (int i = 0; i < maximo_clientes; i++)
    {
      // BuildPlayer.socket ==
      
      printf("ITERANDO ACÁ\n");
      if (i==0 && game.players[0].has_name == false){ //Pregunta por monstruo
        if(game.players[i].has_name == false)
        {
          game.players[i].has_name = true;
          send_txt(game.players[i].socket, x_req_nombre);
          char *pname = request_txt(game.players[i].socket);
          strncpy(game.players[i].playername, pname, 12);
          free(pname);
          game.players[i].playername[12] = '\0';
          printf("nombre usuario: %s\n", game.players[i].playername);
        }
        send_txt(game.players[i].socket, x_admin_req_monster);
        choice = request_int(game.players[i].socket, 1, 2);
        printf("choice: %d\n", choice);
        if (choice == 1)
        {
          send_txt(game.players[i].socket, x_admin_req_monster_succ);
          game.can_have_monster = true;
        }
        else{
          send_txt(game.players[i].socket, x_admin_req_monster_fail);
        }
      }
        
      if(i != 0 && game.game_start == false)
      {
        char auxiliar[300];
        sprintf(auxiliar, "\e[1;33m¿Iniciar partida?\n\e[0m[0] Esperar\n[1] Iniciar\n");
        send_txt(game.players[0].socket, auxiliar);
        int begin = request_int(game.players[0].socket, 0, 1);
        if( i < 4 && game.players[3].has_name == false && begin == 1)
        {
          sprintf(auxiliar, "Aún no se puede iniciar partida, faltan jugadores!\n");
          send_txt(game.players[0].socket, auxiliar);
          game.game_start = false;
          
        }
        else if(i < 4 && game.players[3].has_name == false && begin == 0)
        {
          sprintf(auxiliar, "Esperando.\n");
          send_txt(game.players[0].socket, auxiliar);
          game.game_start = false;
        }else if(i < 4 && game.players[3].has_name == true && begin == 1)
        {
          sprintf(auxiliar, "Iniciando partida!\n");
          send_txt(game.players[0].socket, auxiliar);
          game.game_start = true;
        }else if(i < 4 && game.players[3].has_name == true && begin == 0)
        {
          sprintf(auxiliar, "A quién vas a esperar we, ya están todos...\n");
          send_txt(game.players[0].socket, auxiliar);
          game.game_start = false;
        }
      }
      // le preguntamos por su nombre al cliente
      if(game.players[i].has_name == false)
      {
        game.players[i].has_name = true;
        send_txt(game.players[i].socket, x_req_nombre);
        char *pname = request_txt(game.players[i].socket);
        strncpy(game.players[i].playername, pname, 12);
        free(pname);
        game.players[i].playername[12] = '\0';
        printf("nombre usuario: %s\n", game.players[i].playername);
      }
      
      // le preguntamos por su clase al cliente
      if (game.players[i].class == -1)
      {
        int option_high_b = 2;
        send_txt(game.players[i].socket, x_req_class);
        if (game.can_have_monster && (game.pos_monster == -1))
        {
          send_txt(game.players[i].socket, x_req_class_ex);
          option_high_b++;
        }
        choice = request_int(game.players[i].socket, 0, option_high_b);
        game.players[i].class = choice; //Clase seteada
        if(i > 0){
          //Enviamos la información al admin
          char auxiliar[100];

          sprintf(auxiliar, "\e[1;32mEl jugador %s ha escogido la clase <%s>\n", game.players[i].playername, class_def(choice, &game.players[i]));
          send_txt(game.players[0].socket, auxiliar);
        }
      }
      printf("Terminé todas las opciones\n");
      
      if (FD_ISSET(game.players[i].socket, &readfds))
      {
        //Check if it was for closing , and also read the
        //incoming message
        if ((valread = read(game.players[i].socket, buffer, 1024)) == 0)
        {
          //Somebody disconnected , get his details and print
          getpeername(game.players[i].socket, (struct sockaddr *)&address,
                      (socklen_t *)&addrlen);
          printf("\e[1;34mHost disconnected , ip %s , port %d \n",
                 inet_ntoa(address.sin_addr), ntohs(address.sin_port));

          //Close the socket and mark as 0 in list for reuse
          close(game.players[i].socket);
          game.players[i].socket = 0;
        }

        //Echo back the message that came in
        else
        {
          //set the string terminating NULL byte on the end
          //of the data read
          buffer[valread] = '\0';
          send(game.players[i].socket, buffer, strlen(buffer), 0);
        }
      }
    
      //Si Pone que sí, y todo bien, entonces empezamos la lógica the rial
      printf("GAME START %d\n", game.game_start);
      if(game.game_start == true)
      {
        if (!game_start(game, maximo_clientes))
        {
          printf("Juego Terminado, shao.\n");
        }
        
        
        printf("\e[1;34mJugando\n");
      }
      printf("Terminé todas las opciones FINAL\n");
    }
    }

   
  }
  printf("Salí de acá\n");
  int my_attention = 0;
  // Todo el juego
  for (;;)
  {
    break;
    // Pelea contra un mounstro
    for (;;)
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
