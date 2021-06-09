enum classname {
  Cazador, Medico, Hacker, GreatJagRuz, Ruzalos, Ruiz
};

typedef struct ente{
  int hp;
  int max_hp;
  enum classname cls;
  int usos_estocada;
  int usos_fza_bruta;
  int atq_buf;
  int turns_atq_buf_left;

} Ente;