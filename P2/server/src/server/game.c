#include "game.h"
#include "comunication.h"
/** Envia un mensaje a todos los jugadores instanciados
 * 
 * 
 */
void send_txt_all(Game game, char* msg){
  for(int i = 0; i < game.n_alive; i++)
    send_txt(game.players[i].socket, msg);
  for(int i = 0; i < game.n_dead; i++)
    send_txt(game.dead_players[i].socket, msg);
  printf("Mensajes enviados!\n");
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

/*
//TODO:
- Darle opciones para monstruo al admin en inicio.
- Interfaz que muestre el estado de los jugadores

- opciones de ataque de admin, luego por turno opciones opara demás jugadores
- Seleccionar ataque para monstruo
- actualizar todos los estados

*/

/*El líder del grupo es el primero en jugar*/
void game_start(Game *_game){
  Game game = *_game;
  // Rondas
  for(;;)
  {
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
      char buffer_interfaz[100]; 
      int movimiento;
      // printf("ESTOY VIVO %d\n", game.players[i].alive);
      if(game.players[i].alive && game.players[i].hp > 0)
      {
        // Ahora le damos al jugador la opción de escoger su ataque y su objetivo
        char buffer_aux[500];
        send_txt(game.players[i].socket, movimientos_jugador(game.players[i].class, buffer_aux));
        //Ahora le pedimos la opción al jugador
        movimiento = request_int(game.players[i].socket, 0, 3);

        //TODO: Colocar interacción con funciones de cada clase!
        
        if(movimiento == 0)
        {
          kill_player(&game, i, 0);
          i--;
        }else{
          game.players[i].func[movimiento-1](game.players, game.n_alive, i, game.monsters, 1, 1);
        }
      }
      if (!game.monsters[0].alive)
      {
        send_txt_all(game, "\e[0;93mFelicidades, vencieron al monstruo!\e[0m\n");
        printf("Ganaron uwu.\n");
        send_txt_all(game, "\e[1;91m - FIN DE LA PARTIDA1 -\e[0m\n");
        for(int numero_jugador = 0; numero_jugador < game.n_alive + 1; numero_jugador++){
          send_signal(game.players[numero_jugador].socket, 6);
          close(game.players[numero_jugador].socket);
          game.players[numero_jugador].socket = 0;
        }
        for(int numero_jugador = 0; numero_jugador < game.n_dead + 1; numero_jugador++){
          send_signal(game.players[numero_jugador].socket, 6);
          close(game.players[numero_jugador].socket);
          game.players[numero_jugador].socket = 0;
        }
        *_game = game;
        return;
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
      game.monsters[0].func[movimiento_enemigo](game.monsters, 1, 0, game.players, game.n_alive, game.rondas);
      if (movimiento_enemigo == 2)
        game.rondas = 0;
      //Para aliados
      for (int numero_jugador = 0; numero_jugador < game.n_alive; numero_jugador++)
        if (game.players[numero_jugador].hp <= 0)
          kill_player(&game, numero_jugador, 1);
      extras_handler(game.players, game.n_alive);
      extras_handler(game.monsters, 1);
      if (!game.monsters[0].alive)
      {
        send_txt_all(game, "\e[0;93mFelicidades, vencieron al monstruo!\e[0m\n");
        printf("Ganaron uwu.\n");
        send_txt_all(game, "\e[1;91m - FIN DE LA PARTIDA2 -\e[0m\n");
        for(int numero_jugador = 0; numero_jugador < game.n_alive + 1; numero_jugador++){
          send_signal(game.players[numero_jugador].socket, 6);
          close(game.players[numero_jugador].socket);
          game.players[numero_jugador].socket = 0;
        }
        for(int numero_jugador = 0; numero_jugador < game.n_dead + 1; numero_jugador++){
          send_signal(game.players[numero_jugador].socket, 6);
          close(game.players[numero_jugador].socket);
          game.players[numero_jugador].socket = 0;
        }
        *_game = game;
        return;
      }
      for (int numero_jugador = 0; numero_jugador < game.n_alive; numero_jugador++)
        if (game.players[numero_jugador].hp <= 0)
          kill_player(&game, numero_jugador, 1);
    }
    
    if(game.n_alive <= 0)
      {
        printf("\e[0;93mTodos se murieron, na que hacerle \e[0m\n");
        send_txt_all(game, "\e[1;91m - FIN DE LA PARTIDA3 -\n");
        for(int numero_jugador = 0; numero_jugador < game.n_alive + 1; numero_jugador++){
          send_signal(game.players[numero_jugador].socket, 6);
          close(game.players[numero_jugador].socket);
          game.players[numero_jugador].socket = 0;
        }
        for(int numero_jugador = 0; numero_jugador < game.n_dead + 1; numero_jugador++){
          send_signal(game.players[numero_jugador].socket, 6);
          close(game.players[numero_jugador].socket);
          game.players[numero_jugador].socket = 0;
        }
        *_game = game;
        return;
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
    sprintf(buffer_aux, "[1] Estocada\n[2] Corte Cruzado\n[3] Distraer\n[0] Rendirse\n");
    return buffer_aux;
  case Medico:
    sprintf(buffer_aux, "[1] Curar\n[2] Destello\n[3] Descarga\n[0] Rendirse\n");
    return buffer_aux;
  case Hacker:
    sprintf(buffer_aux, "[1] Sql Injection\n[2] DDos Attack\n[3] Bruteforce\n[0] Rendirse\n");
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
  game.players[player_id].hp = 0;
  game.players[player_id].alive = false;
  game.dead_players[game.n_dead] = game.players[player_id];
  game.n_dead += 1;
  int i_aux = 0;
  for (int j = 0; j < game.n_alive; j++)
    if (game.players[j].alive){
      game.players[i_aux] = game.players[j];
      i_aux += 1;
    }
  game.n_alive -= 1;
  // game.jugadores_inicializados_totalmente -= 1;
  char buffer[80];
  if (mode){
    send_txt(game.players[player_id].socket, "Has muerto, ahora solo te queda mirar sentado uwu.\n");
    sprintf(buffer, "\e[0;93mEl jugador %s ha muerto!\n", game.players[player_id].playername);
  }
  else{
    send_txt(game.players[player_id].socket, "\e[0;93mTe has rendido, ahora solo te queda mirar sentado uwu.\e[0m\n");
    sprintf(buffer, "\e[0;93mEl jugador %s se ha rendido!\e[0m\n", game.players[player_id].playername);
  }
  send_txt_all(game, buffer);
  *_game = game;
}