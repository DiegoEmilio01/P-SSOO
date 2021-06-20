#include "game.h"
#include "comunication.h"
#include "clases_extra.h"
#include "clases.h"

/** Envia un mensaje a todos los jugadores instanciados
 * 
 * 
 */
void send_txt_all(Game game, char* msg){
  for(int i = 0; i < game.n_alive; i++)
    send_txt(game.players[i].socket, msg);
  for(int i = 0; i < game.n_dead; i++)
    send_txt(game.dead_players[i].socket, msg);
}

char *objetivos_habilidad(Game game, int pos_jugador, int movimiento, char* buffer_aux);

/** Envia un mensaje a todos los jugadores instanciados menos a uno
 * 
 * @param socket_evict Valor del socket a evitar
 */
void send_txt_exclude(Game game, char* msg, int socket_evict){
  int counter_guerreros = 0;
  int socket_to_send;
  for (int i=0; i<5; i++){
    switch (game.active_players[i])
    {
    case 1:
      socket_to_send = game.players[counter_guerreros].socket;
      if (socket_to_send == socket_evict)
        send_txt(socket_to_send, msg);
      counter_guerreros++;
      break;
    case -1:
      socket_to_send = game.monsters[0].socket;
      if (socket_to_send == socket_evict)
        send_txt(socket_to_send, msg);
      send_txt(socket_to_send, msg);
      break;
    }
  }
}


/*El líder del grupo es el primero en jugar*/
void game_start(Game *_game){
  Game game = *_game;
  game.rondas = -1;
  // Rondas
  for(;;)
  {
    game.rondas++;
    char buffer[100];
    sprintf(buffer, "\n\e[0;93mNueva Ronda, piensa cuidadosamente tu jugada...\n");
    send_txt_all(game, buffer);
    
    // turnos
    for(int i = 0; i < game.n_alive; i++)
    {
      send_txt_all(game, "\e[0m\n----------------------\n\e[1;35m[ENEMIGOS]\e[0m\n");
      sprintf(buffer, "- [%s] HP:%d/%d\n",game.monsters[0].playername, game.monsters[0].hp, game.monsters[0].max_hp);
      send_txt_all(game, buffer);    
      send_txt_all(game, "\e[1;35m[JUGADORES]\e[0m\n");
      for(int ply = 0; ply < game.n_alive; ply++ )
      {
        sprintf(buffer, "- [%s] HP:%d/%d\n",game.players[ply].playername, game.players[ply].hp, game.players[ply].max_hp);
        send_txt_all(game, buffer);
      }
    
      //Acá se muestra el estado a todos los jugadores, luego el admin hace el primer movimiento
      // char buffer_interfaz[100]; 
      int movimiento;
      // printf("ESTOY VIVO %d\n", game.players[i].alive);
      if(game.players[i].alive && game.players[i].hp > 0)
      {
        // Ahora le damos al jugador la opción de escoger su ataque y su objetivo
        char buffer_aux[500];
        send_txt(game.players[i].socket, movimientos_jugador(game.players[i].class, buffer_aux));
        //Ahora le pedimos la opción al jugador
        movimiento = request_int(game.players[i].socket, 0, 3);
        //char *nuevo = objetivos_habilidad(game, i, movimiento, buffer_aux);
        //send_txt(game.players[i].socket, movimientos_jugador(game.players[i].class, nuevo));

        if(movimiento == 0)
        {
          kill_player(&game, i, 0);
          i--;
        }else{
          char *accion = game.players[i].func[movimiento-1](game.players, game.n_alive, i, game.monsters, 1, 1);
          send_txt_all(game, accion);
          free(accion);
        }
      }
      if (!game.monsters[0].alive)
      {
        send_txt_all(game, "\e[0;93mFelicidades, vencieron al monstruo!\e[0m\n");
        printf("Ganaron uwu.\n");
        send_txt_all(game, "\e[1;91m - FIN DE LA PARTIDA -\e[0m\n");
        *_game = game;
        play_again(_game);
        return;
        // for(int numero_jugador = 0; numero_jugador < game.n_alive + 1; numero_jugador++){
        //   send_signal(game.players[numero_jugador].socket, 6);
        //   close(game.players[numero_jugador].socket);
        //   game.players[numero_jugador].socket = 0;
        // }
        // for(int numero_jugador = 0; numero_jugador < game.n_dead + 1; numero_jugador++){
        //   send_signal(game.players[numero_jugador].socket, 6);
        //   close(game.players[numero_jugador].socket);
        //   game.players[numero_jugador].socket = 0;
        // }
        //return;
      }

    }
    //MOVIMIENTO DEL MONSTRUO
    if (game.n_alive > 0)
    {
      //Ruiz tiene 3 movimientos.
      int movimiento_enemigo = rand() % 100;
      if(game.monsters[0].class == Ruiz)
      {
        if (movimiento_enemigo < 40)
          movimiento_enemigo = 0;
        else if (movimiento_enemigo < 60)
          movimiento_enemigo = 1;
        else 
          movimiento_enemigo = 2;
      }
      if (game.monsters[0].class == Ruzalos) {
        if (movimiento_enemigo < 40)
          movimiento_enemigo = 0;
        else
          movimiento_enemigo = 1;
      }
      else { //GreatJagRuz
        if (movimiento_enemigo < 50)
          movimiento_enemigo = 0;
        else
          movimiento_enemigo = 1;
      }
      // AQUÍ ATACA EL MONSTRUO!
      char *accion = game.monsters[0].func[movimiento_enemigo](game.monsters, 1, 0, game.players, game.n_alive, game.rondas);
      send_txt_all(game, accion);
      free(accion);
      if (movimiento_enemigo == 2)
        game.rondas = 0;
      //Para aliados
      for (int numero_jugador = 0; numero_jugador < game.n_alive; numero_jugador++)
        if (game.players[numero_jugador].hp <= 0){
          kill_player(&game, numero_jugador, 1);
          numero_jugador--;
        }
      extras_handler(game.players, game.n_alive);
      extras_handler(game.monsters, 1);
      if (!game.monsters[0].alive)
      {
        send_txt_all(game, "\e[0;93mFelicidades, vencieron al monstruo!\e[0m\n");
        printf("Ganaron uwu.\n");
        send_txt_all(game, "\e[1;91m - FIN DE LA PARTIDA -\e[0m\n");
        *_game = game;
        play_again(_game);
        return;
        // for(int numero_jugador = 0; numero_jugador < game.n_alive + 1; numero_jugador++){
        //   send_signal(game.players[numero_jugador].socket, 6);
        //   close(game.players[numero_jugador].socket);
        //   game.players[numero_jugador].socket = 0;
        // }
        // for(int numero_jugador = 0; numero_jugador < game.n_dead + 1; numero_jugador++){
        //   send_signal(game.players[numero_jugador].socket, 6);
        //   close(game.players[numero_jugador].socket);
        //   game.players[numero_jugador].socket = 0;
        // }
        //return;
      }
      for (int numero_jugador = 0; numero_jugador < game.n_alive; numero_jugador++)
        if (game.players[numero_jugador].hp <= 0){
          kill_player(&game, numero_jugador, 1);
          numero_jugador--;
        }
    }
    
    if(game.n_alive <= 0)
      {
        printf("\e[0;93mTodos se murieron, na que hacerle \e[0m\n");
        send_txt_all(game, "\e[1;91m - FIN DE LA PARTIDA -\n");
        *_game = game;
        play_again(_game);
        return;
        // for(int numero_jugador = 0; numero_jugador < game.n_alive + 1; numero_jugador++){
        //   send_signal(game.players[numero_jugador].socket, 6);
        //   close(game.players[numero_jugador].socket);
        //   game.players[numero_jugador].socket = 0;
        // }
        // for(int numero_jugador = 0; numero_jugador < game.n_dead + 1; numero_jugador++){
        //   send_signal(game.players[numero_jugador].socket, 6);
        //   close(game.players[numero_jugador].socket);
        //   game.players[numero_jugador].socket = 0;
        // }
        //return;
      }
    
  }
  *_game = game;
  return;
}

/*Devuleve el buffer para envíar las habilidades para su impresión en el cliente*/
char *objetivos_habilidad(Game game, int pos_jugador, int movimiento, char* buffer_aux)
{
  
  switch (game.players[pos_jugador].class)
  {
  case Cazador:
    sprintf(buffer_aux, "\e[0;93mEjercerás tu habilidad sobre el monstruo.\e[0m\nEnvía un 1 para atacar.");
    return buffer_aux;
  case Medico:
    if (movimiento == 0) // Va a curar a alguno de los aliados
    {
      send_txt(game.players[pos_jugador].socket, "\e[0;93mEjercerás tu habilidad de curación ¿Qué jugador deseas sanar?\n");
      int i;
      for(i = 0; i < game.n_alive - 1; i++)
      {
        sprintf(buffer_aux, "[%i]> %s\n", i, game.players[i].playername);
        send_txt(game.players[pos_jugador].socket, buffer_aux);
      }
      sprintf(buffer_aux, "[%i]> %s\n", i, game.players[i].playername);
      return buffer_aux;
    }
    sprintf(buffer_aux, "\e[0;93mEjercerás tu habilidad sobre el monstruo.\e[0m\nEnvía un 1 para atacar.");
    return buffer_aux;
  case Hacker:
    if (movimiento == 0) // Duplicar la habilidad de alguno de los aliados
    {
      send_txt(game.players[pos_jugador].socket, "\e[0;93mEjercerás tu habilidad de boostear a un jugador ¿A quién deseas ayudar?\n");
      int i;
      for(i = 0; i < game.n_alive - 1; i++)
      {
        sprintf(buffer_aux, "[%i]> %s\n", i, game.players[i].playername);
        send_txt(game.players[pos_jugador].socket, buffer_aux);
      }
      sprintf(buffer_aux, "[%i]> %s\n", i, game.players[i].playername);
      return buffer_aux;
    }
    if (movimiento == 1) {
      return buffer_aux = "\e[0;93mEjercerás tu habilidad sobre el monstruo.\e[0m\nEnvía un 1 para atacar.";
    }
    return buffer_aux = "\e[0;93mHas ocupado FUERZA BRUTA.\e[0m\nEnvía un 1 para sumar una carga de esta habilidad.";
  default:
    return NULL;
  }
}

char *movimientos_jugador(int class, char *buffer_aux)
{
  switch (class)
  {
  case Cazador:
    sprintf(buffer_aux, "\e[0;93m\nSelecciona una habilidad para utilizar\n\e[0m[1] Estocada\n[2] Corte Cruzado\n[3] Distraer\n[0] Rendirse\n");
    return buffer_aux;
  case Medico:
    sprintf(buffer_aux, "\e[0;93m\nSelecciona una habilidad para utilizar\n\e[0m[1] Curar\n[2] Destello\n[3] Descarga\n[0] Rendirse\n");
    return buffer_aux;
  case Hacker:
    sprintf(buffer_aux, "\e[0;93m\nSelecciona una habilidad para utilizar\n\e[0m[1] Sql Injection\n[2] DDos Attack\n[3] Bruteforce\n[0] Rendirse\n");
    return buffer_aux;
  default:
    sprintf(buffer_aux, "Esta clase no existe?\n");
    return buffer_aux;
  }
};

/*Función para inicializar las entidades del juego. Las variables se setean a 0*/
void game_entities_init(Game game, int clientes_maximos)
{
  for (int player = 0; player < clientes_maximos; player++){
    game.players[player] = (Entity){
      .hp = 0, 
      .max_hp = 0,
      .n_funciones = 3, //3 tipos de movimientos
      .class = -1,
      .has_name = false,
      .is_monster = false,
      .socket = 0,
      .jumped = false,
      .alive = true,
    };
  }
}

void kill_player(Game *_game, int player_id, int mode)
{
  Game game = *_game;
  char buffer[253];
  if (mode){
    send_txt(game.players[player_id].socket, "\e[0;93mHas muerto, ahora solo te queda mirar sentado uwu.\e[0m\n");
    sprintf(buffer, "\e[0;93mEl jugador %s ha muerto!\n", game.players[player_id].playername);
  }
  else{
    send_txt(game.players[player_id].socket, "\e[0;93mTe has rendido, ahora solo te queda mirar sentado uwu.\e[0m\n");
    sprintf(buffer, "\e[0;93mEl jugador %s se ha rendido!\e[0m\n", game.players[player_id].playername);
  }
  game.players[player_id].hp = 0;
  game.players[player_id].alive = false;
  game.dead_players[game.n_dead] = game.players[player_id];
  game.n_dead += 1;
  int i_aux = player_id;
  for (int j = player_id; j < game.n_alive; j++)
    if (game.players[j].alive){
      game.players[i_aux] = game.players[j];
      i_aux += 1;
    }
  game.n_alive -= 1;
  // game.jugadores_inicializados_totalmente -= 1;
  send_txt_all(game, buffer);
  *_game = game;
}

void play_again(Game *_game){
  Game game = *_game;
  char mensaje[100];
  sprintf(mensaje, "\e[0;93m\nAhora que el enemigo está muerto, ¿quieres jugar otra partida?\e[0m\n");
  send_txt_all(game, mensaje);
  int contador = 0;
  //Por comodidad, vamos a pasar los jugadores muertos a los jugadores vivos
  for (int j = 0; j < game.n_dead; j++)
  {//Si el jugador estaba muerto (debería), lo pasamos a la lista de los vivos
    // if (!game.dead_players[j].alive){
    game.players[game.n_alive] = game.dead_players[j];
    game.players[game.n_alive].alive = true;
    contador++;
    game.n_alive++;
    //}
  }
  game.n_dead -= contador;
  
  // Ahora le preguntamos a cada jugador si quiere seguir jugando, y si sí le damos a escoger su clase
  for (int jugador = 0; jugador < game.n_alive; jugador++){
    if(game.players[jugador].socket != 0)
    {
      sprintf(mensaje, "\n[0] Jugar otra partida\n[1] Desconectarme\n");
      send_txt(game.players[jugador].socket, mensaje);
      int eleccion = request_int(game.players[jugador].socket, 0, 1);
      if (eleccion == 0) 
      { //Si juega otra partida, le preguntamos por su clase
        send_txt(game.players[jugador].socket, "\e[0;93m¿Qué clases deseas ser?\n \e[0m[0]>Cazador\n [1]>Médico\n [2]>Hacker\n");
        eleccion = request_int(game.players[jugador].socket, 0, 2);
        game.players[jugador].class = eleccion; //Clase seteada
        //Ahora inicializamos la clase correspondiente para el jugador
        class_def(eleccion, &game.players[jugador]);
      }else{
        //Si no, lo desconectamos
        char buffer_xd[100];
        sprintf(buffer_xd, "Shao %s, gracias por jugar!\n", game.players[jugador].playername);
        send_txt(game.players[jugador].socket, buffer_xd);
        send_signal(game.players[jugador].socket, 6);
        close(game.players[jugador].socket);
        game.players[jugador].socket = 0;
      }
    }
  }
  //Ahora verificamos si el admin salió, y en caso de que eso haya sucedido, le damos el admin a otro
  // int contador_aux = 0;
  // for (int jugador = 1; jugador < game.jugadores_inicializados_totalmente; jugador++)
  // {
  //   if (game.players[jugador].socket > 0)
  //   {
  //     game.players[contador_aux] = game.players[jugador]; //Lo cambiamos de lugar, ahora falta reacomodar los espacios
  //     contador_aux += 1;
  //   }
  // }

  // game.jugadores_inicializados_totalmente = contador_aux;
  int contador_aux;
  for (int jugador = 0; jugador < game.jugadores_inicializados_totalmente; jugador++){
    if (game.players[jugador].socket == 0)
    {
      game.n_alive -= 1;
      game.players[jugador].alive = false;
      contador_aux = 0;
      for (int juga2 = jugador; juga2 < game.jugadores_inicializados_totalmente; juga2++)
      {
        if(game.players[juga2].socket > 0){
          game.players[jugador+contador_aux] = game.players[juga2]; //Lo cambiamos de lugar, ahora falta reacomodar los espacios
          contador_aux++;
        }
      }
      game.jugadores_inicializados_totalmente--;
    }
  }
  //Ahora preguntamos al admin por el nuevo monstruo
  if(game.n_alive > 0)
  {
    char buffer_monstruo[100];
    sprintf(buffer_monstruo, "Escoge un mounstro:\n[3] GreatJagRuz\n[4] Ruzalos\n[5] Ruiz\n");
    send_txt(game.players[0].socket, buffer_monstruo);
    int opcion_monstruo = request_int(game.players[0].socket, 3, 5);
    class_def(opcion_monstruo, &game.monsters[0]);
    game.monsters->alive = true;
  }
  *_game = game;
}