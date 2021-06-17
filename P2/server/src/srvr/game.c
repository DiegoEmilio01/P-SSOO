#include "game.h"
#include "comunication.h"
/** Envia un mensaje a todos los jugadores instanciados
 * 
 * 
 */
void send_txt_all(Game game, char* msg, int maximo_clientes){
  int counter_guerreros = 0;
  for(int i = 0; i < maximo_clientes; i++)
  {
      send_txt(game.players[i].socket, msg);
  }
  printf("Mensajes enviados!\n");
//   for (int i=0; i<4; i++){
//     switch (game.active_players[i])
//     {
//     case 1:
//       send_txt(game.players[counter_guerreros].socket, msg);
//       counter_guerreros++;
//       break;
//     default:
//       send_txt(game.monsters[0].socket, msg);
//       break;
//     }
//   }
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
int game_start(Game game, int maximo_clientes){
  //Ahora decidimos el monstruo
  for(;;)
  {
    char buffer[100];
    sprintf(buffer, "\e[0;93mNuevo turno, piensa cuidadosamente tu jugada...\n");
    send_txt_all(game, buffer, game.jugadores_inicializados_totalmente);
    for(int i = 0; i < game.jugadores_inicializados_totalmente; i++)
    {
      //Acá se muestra el estado a todos los jugadores, luego el admin hace el primer movimiento
      
      for(int ply = 0; ply < game.jugadores_inicializados_totalmente; ply++ )
      {
        char buffer_interfaz[100]; 
        send_txt_all(game, "[ESTADOS JUGADORES]\n", game.jugadores_inicializados_totalmente);
        sprintf(buffer_interfaz, "- [%s] HP:%d/%d ",game.players[ply].playername, game.players[ply].hp, game.players[ply].max_hp);
        send_txt_all(game, buffer_interfaz, game.jugadores_inicializados_totalmente);
      }
      if (game.players[i].hp <= 0)
      {
        send_txt(game.players[i].socket, "Tas muerto, ahora solo te queda mirar sentado uwu.\n");
      }else{
        // Ahora le damos al jugador la opción de escoger su ataque y su objetivo
        char buffer_aux[500];
        send_txt(game.players[i].socket, movimientos_jugador(game.players[i].class, buffer_aux));
        //Ahora le pedimos la opción al jugador
        int movimiento = request_int(game.players[i].socket, 0, 2);
        //Ahora preguntamos por el objetivo del ataque (Se asume que se puede atacar todos?)
        
        send_txt(game.players[i].socket, objetivos_habilidad(game, i, movimiento, buffer_aux));

        int objetivo = request_int(game.players[i].socket, 0, 2);
        //TODO: Colocar interacción con funciones de cada clase!
        //ejecutar_movimiento()
      }
      if (!game.monsters[0].alive)
      {
        send_txt_all(game, "Felicidades, vencieron al monstruo!\n", game.jugadores_inicializados_totalmente);
        printf("Ganaron uwu.\n");
        send_txt_all(game, "\e[1;91m - FIN DE LA PARTIDA -\n", game.jugadores_inicializados_totalmente);
        for (int numero_jugador = 0; i < game.jugadores_inicializados_totalmente; numero_jugador++)
        {
          //Desconectamos a los jugadores y seteamos su socket en 0
          close(game.players[numero_jugador].socket);
          game.players[numero_jugador].socket = 0;
        }
      }
      //Revisamos si ya no quedan jugadores y si es así, se termina el juego
      if(game.n_alive <= 0)
      {
        printf("Todos se murieron, na que hacerle \n");
        send_txt_all(game, "\e[1;91m - FIN DE LA PARTIDA -\n", game.jugadores_inicializados_totalmente);
        for (int numero_jugador = 0; i < game.jugadores_inicializados_totalmente; numero_jugador++)
        {
          //Desconectamos a los jugadores y seteamos su socket en 0
          close(game.players[numero_jugador].socket);
          game.players[numero_jugador].socket = 0;
        }
        
      }
      

    }
  }
}

/*Devuleve el buffer para envíar las habilidades para su impresión en el cliente*/
char *objetivos_habilidad(Game game, int pos_jugador, int movimiento, char* buffer_aux)
{
  
  switch (game.players[pos_jugador].class)
  {
  case Cazador:
    sprintf(buffer_aux, "\e[0;93mEjercerás tu habilidad sobre el monstruo.\nEnvía un 1 para atacar.");
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
    sprintf(buffer_aux, "\e[0;93mEjercerás tu habilidad sobre el monstruo.\nEnvía un 1 para atacar.");
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
      return buffer_aux = "\e[0;93mEjercerás tu habilidad sobre el monstruo.\nEnvía un 1 para atacar.";
    }
    return buffer_aux = "\e[0;93mHas ocupado FUERZA BRUTA.\nEnvía un 1 para sumar una carga de esta habilidad.";
  default:
    return NULL;
  }
}

char *movimientos_jugador(int class, char *buffer_aux)
{
  switch (class)
  {
  
  case Cazador:
    sprintf(buffer_aux, "[0] Estocada\n[1] Corte Cruzado\n[2]Distraer\n");
    return buffer_aux;
  case Medico:
    sprintf(buffer_aux, "[0] Curar\n[1] Destello\n[2] Descarga\n");
    return buffer_aux;
  case Hacker:
    sprintf(buffer_aux, "[0] Sql Injection\n[1] DDos Attack\n[2] Bruteforce\n");
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
      .self_contador = 0,
      .effect_contador = 0,
      .effect_value = 0,
      .effect_type = 0,
      .n_funciones = 3, //3 tipos de movimientos
      .class = -1,
      .has_name = false,
      .is_monster = false,
      .socket = 0,
      .jumped = false,
      .alive = true,
      .multiplier = 1
    };
  }
}

