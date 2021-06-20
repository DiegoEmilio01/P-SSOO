#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
//#include <sys/select.h>
#include "comunication.h"
#include "conection.h"
#include "bits.h"
#include "clases.h"
#include "game.h"

/**
 * Después sacar:
 * AZUL: \e[0;94m
 * RESET: \e[0m
 * AMARILLO: \e[0;93m
 * MORADO: \e[1;35m
 * ROJO: \e[1;91m
*/


char *NO_TXT = "\0";
const uint8_t TXT_ONLY = 0;

char 
  *x_admin_req_monster_succ = "\e[0;93mEscoge un mounstro\e[0m\n",
  *x_admin_req_monster_fail = "\e[0;93mSe decidió que NO haya mounstro\e[0m\n",
  *x_admin_req_monster = "\e[0;93m¿Deseas que haya un mounstro?\n \e[0m[1]>Si\n [2]>No\n",
  *x_req_nombre = "\e[0;93mA continuación, ingresa tu nombre. Se truncará a 12 caracteres\n",
  *x_req_class = "\e[0;93m¿Qué clases deseas ser?\n \e[0m[0]>Cazador\n [1]>Médico\n [2]>Hacker\n",
  *x_req_class_ex = " [3]>Monstruo\n"
;


int main(int argc, char **argv)
{
  // Se define una IP y un puerto
  if (argc != 5 || strcmp(argv[1], "-i") || strcmp(argv[3], "-p")){
    printf("\e[1;91mERROR: INPUT INVÁLIDO.\nEl correcto uso del programa es: %s -i <IP> -p [PORT]\e[0m\n", argv[0]);
    return 1;
  }
  char * IP = argv[2];
  int PORT = atoi(argv[4]);

  Game game;
  memset(&game, 0, sizeof(Game));
  game.n_alive = 1;
  game.n_dead = 0;
  game.rondas = 0;
  game.can_have_monster = false;
  game.pos_admin = 0;
  game.jugadores_inicializados_totalmente = 1;
  game.game_start = false;
  game.pos_monster = -1;

  
  printf("\e[0;94mServidor encendido\n");
  
  // Cosas X

  fd_set readfds;
  int max_sd;
  int maximo_clientes = 4;
  int activity, valread, addrlen=0 , new_socket;
  char buffer[1025];
  struct sockaddr_in address = {0};
  game_entities_init(game, maximo_clientes);
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
  int clientes = 0;
  int server_socket = init_sockets(IP, PORT);
  printf("\e[0;94mServidor escuchando\n");
  
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
      printf("\e[0;94mNew connection , socket fd is %d , ip is : %s , port : %d\n " , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

      //send new connection greeting message
      char welcome[100];
      sprintf(welcome, "\e[0;93mBienvenido Cliente %d!!\n", clientes);
      clientes += 1;
      send_txt(new_socket, welcome);
      // if (send(new_socket, welcome, strlen(welcome), 0) != strlen(welcome))
      // {
      //   perror("send");
      // }

      puts("\e[0;94mWelcome message sent successfully");

      //add new socket to array of sockets
      game.players[0].socket = new_socket;
      printf("\e[0;94mAdding to list of sockets as 0\n"); //Le da al admin el primer socket
      game.players[0].has_name = false;
      game.players[0].class = -1;
      game.players[0].alive = true;
      
      // Acá se inicializan atributos del admin, preguntar NOMBRE - CLASE y nada más. Hasta acá hay un if sin cerrar.
      if(game.players[0].has_name == false)
      {
        game.players[0].has_name = true;
        send_txt(game.players[0].socket, x_req_nombre);
        char *pname = request_txt(game.players[0].socket);
        strncpy(game.players[0].playername, pname, 12);
        free(pname);
        game.players[0].playername[12] = '\0';
        printf("nombre usuario: %s\n", game.players[0].playername);
      }
        
      // le preguntamos por su clase al cliente
      if (game.players[0].class == -1)
      {
        int option_high_b = 2;
        send_txt(game.players[0].socket, x_req_class);
        if (game.can_have_monster && (game.pos_monster == -1))
        {
          send_txt(game.players[0].socket, x_req_class_ex);
          option_high_b++;
        }
        choice = request_int(game.players[0].socket, 0, option_high_b);
        game.players[0].class = choice; //Clase seteada
        //Ahora inicializamos la clase correspondiente para el jugador
        class_def(choice, &game.players[0]);
        //printf("\e[1;91m[DEBUG]: VIDA DEL RECIEN CREADO %d\n", game.players[0].hp);
      }


      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Borrar esto luego
      while (clientes < maximo_clientes && game.game_start == false ) //Cantidad máxima clientes 
      {
        for (int i = 0; i < maximo_clientes; i++)
        {
          //printf("Iterando aca\n");
          //if position is empty
          
          if (game.players[i].socket == 0)
          {
            if (FD_ISSET(server_socket, &readfds))
            {
              if ((new_socket = accept(server_socket,
                                      (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
              {
                //perror("accept");
                exit(EXIT_FAILURE);
              }

              //inform user of socket number - used in send and receive commands
              printf("\e[0;94mNew connection , socket fd is %d , ip is : %s , port : %d\n " , new_socket , inet_ntoa(address.sin_addr) , ntohs
                    (address.sin_port));

              //send new connection greeting message
              char welcome[1024];
              sprintf(welcome, "\e[0;93mBienvenido Cliente %d!!\n", clientes);
              clientes += 1;
              send_txt(new_socket, welcome);
              // if (send(new_socket, welcome, strlen(welcome), 0) != strlen(welcome))
              // {
              //   perror("send");
              // }

              puts("\e[0;94mWelcome message sent successfully");
            
              //add new socket to array of sockets
              for (int i = 0; i < maximo_clientes; i++)
              {
                //if position is empty
                if (game.players[i].socket == 0)
                {
                  game.players[i].socket = new_socket;
                  printf("\e[0;94mAdding to list of sockets as %d\n", i);
                  // le preguntamos por su nombre al cliente
                  if(game.players[i].has_name == false)
                  {
                    game.players[i].has_name = true;
                    send_txt(game.players[i].socket, x_req_nombre);
                    char *pname = request_txt(game.players[i].socket);
                    strncpy(game.players[i].playername, pname, 12);
                    free(pname);
                    game.players[i].playername[12] = '\0';
                    printf("Nombre usuario: %s\n", game.players[i].playername);
                  }
                  
                  // le preguntamos por su clase al cliente
                  printf("NOMBRE: %s\n", game.players[i].playername);
                  printf("CLASE: %d\n", game.players[i].class);
                  
                  int option_high_b = 2;
                  send_txt(game.players[i].socket, x_req_class);
                  if (game.can_have_monster && (game.pos_monster == -1))
                  {
                    send_txt(game.players[i].socket, x_req_class_ex);
                    option_high_b++;
                  }
                  choice = request_int(game.players[i].socket, 0, option_high_b);
                  game.players[i].class = choice; //Clase seteada
                  //Ahora inicializamos la clase correspondiente para el jugador
                  class_def(choice, &game.players[i]);
                  //printf("\e[1;91m[DEBUG]: VIDA DEL RECIEN CREADO %d\n", game.players[i].hp);
                  

                  if(i > 0){
                    //Enviamos la información al admin
                    char auxiliar[100];

                    sprintf(auxiliar, "\e[1;35mEl jugador %s ha escogido la clase <%s>\n", game.players[i].playername, class_def(choice, &game.players[i]));
                    send_txt(game.players[0].socket, auxiliar);
                  }
                  game.players[i].alive = true;
                  game.jugadores_inicializados_totalmente += 1;
                  game.n_alive += 1;
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
                  printf("\e[0;94mHost disconnected , ip %s , port %d \n",
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
          
          char auxiliar[300];
          sprintf(auxiliar, "\e[0;93m¿Iniciar partida?\n\e[0m[0] Esperar\n[1] Iniciar\n");
          send_txt(game.players[0].socket, auxiliar);
          int begin = request_int(game.players[0].socket, 0, 1);
          printf("CLIENTES %d| JUGADORES %d\n", clientes, game.jugadores_inicializados_totalmente);
          if( clientes != game.jugadores_inicializados_totalmente  && begin == 1)
          {
            sprintf(auxiliar, "Aún no se puede iniciar partida, faltan jugadores!\n");
            send_txt(game.players[0].socket, auxiliar);
            game.game_start = false;
            
          }
          else if(clientes == game.jugadores_inicializados_totalmente  && begin == 0)
          {
            sprintf(auxiliar, "Esperando.\n");
            send_txt(game.players[0].socket, auxiliar);
            game.game_start = false;
          }else if(clientes == game.jugadores_inicializados_totalmente && begin == 1)
          {
            sprintf(auxiliar, "\e[1;91mIniciando partida!\e[0m\n");
            send_txt(game.players[0].socket, auxiliar);
            game.game_start = true;
            break;
          }else if(clientes == game.jugadores_inicializados_totalmente && begin == 0)
          {
            sprintf(auxiliar, "A quién vas a esperar we, ya están todos...\n");
            send_txt(game.players[0].socket, auxiliar);
            game.game_start = false;
          }
        
          if(game.game_start){
            break;
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
        send_txt(game.players[i].socket, x_admin_req_monster_succ);
        game.can_have_monster = true;
        game.monsters[0].alive = true;
        //Acá hay que inicializar al monstruo.
        char buffer_monstruo[100];
        sprintf(buffer_monstruo, "[3] GreatJagRuz\n[4] Ruzalos\n[5] Ruiz\n");
        send_txt(game.players[i].socket, buffer_monstruo);
        int opcion_monstruo = request_int(game.players[i].socket, 3, 5);
        class_def(opcion_monstruo, &game.monsters[0]);

        //printf("Terminé todas las opciones\n");
        
        if (FD_ISSET(game.players[i].socket, &readfds))
        {
          //Check if it was for closing , and also read the
          //incoming message
          if ((valread = read(game.players[i].socket, buffer, 1024)) == 0)
          {
            //Somebody disconnected , get his details and print
            getpeername(game.players[i].socket, (struct sockaddr *)&address,
                        (socklen_t *)&addrlen);
            printf("\e[0;94mHost disconnected , ip %s , port %d \n",
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
        for(;;){
          if(game.game_start == true)
          {
            printf("\e[0;94mJugando\n");
            game_start(&game);
            printf("JUGADORES ALIVE: %d\n", game.n_alive);
            if(game.n_alive <= 0){
              printf("Juego terminado\n");
              //printf("Juego Terminado, shao.\n");          
              return 0;
            }
          }else break;          
        }
        
        //printf("Terminé todas las opciones FINAL\n");
      }
    }
 
  }
  
  return 0;
}
