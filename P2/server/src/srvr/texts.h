#pragma once
// enum texts {
//   t_exit,
//   t_admin_start,
//   t_admin_req_monster,
//   t_req_nombre,
//   t_req_class,
//   t_req_class_ex,
//   t_req_add_or_start,
//   t_start_game
// };


static char
  *x_none = "\0",
  *x_input = "$> ",
  *x_exit = " [0]>Desconectarse\n",
  *x_admin_start = "Bienvenido! Tu eres admin\n",
  *x_member_start = "Bienvenido! Tu eres admin\n",
  *x_admin_req_monster = "¿Deseas que haya un mounstro?\n [1]>Si\n [2]>No\n",
  *x_admin_req_monster_succ = "Se decidió que haya mounstro\n",
  *x_admin_req_monster_fail = "Se decidió que NO haya mounstro\n",
  *x_req_nombre = "A continuación, ingresa tu nombre. Se truncará a 12 caracteres\n",
  *x_req_class = "¿Qué clases deseas ser?\n [1]>Cazador\n [2]>Médico\n [3]>Hacker\n",
  *x_req_class_ex = " [4]>Monstruo\n",
  *x_req_add_or_start = "¿Qué deseas hacer?\n [1]>Agregar un cliente\n [2]>Iniciar partida\n",
  *x_member_joined = "Se conectó el jugador %d\n",
  *x_member_nameclass = "El jugador %d se quedó con nombre %s y clase %s\n",
  *x_start_game = "Iniciando Partida...\n",
  *x_err = "No se pudo encontrar mensaje\n",
  *x_c_cazador = "Cazador",
  *x_c_medico = "Medico",
  *x_c_hacker = "Hacker",
  *x_c_monstruo = "Monstruo"
;

// char *get_predefined_txt(enum texts txt){
//   switch (txt)
//   {
//   case t_admin_start:
//     return x_admin_start;
//   case t_admin_req_monster:
//     return x_admin_req_monster;
//   case t_req_nombre:
//     return x_req_nombre;
//   case t_req_class:
//     return x_req_class;
//   case t_req_class_ex:
//     return x_req_class_ex;
//   case t_req_add_or_start:
//     return x_req_add_or_start;
//   case t_start_game:
//     return x_start_game;
//   default:
//     return x_err;
//   }
// }