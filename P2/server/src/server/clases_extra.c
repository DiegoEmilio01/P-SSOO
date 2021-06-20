#include <assert.h>
#include <stdlib.h>

#include "clases.h"
#include "comunication.h"
/** Aplica funciones necesarias al final del turno, se debe llamar una vez por cada lista de entidades (una aliada y una enemigia)
 * @param entes Lista de entes, puede ser aliado o enemigo
 * @param len_estes Largo de la lista de entes
*/
void extras_handler(Entity* entes, int len_entes){

  for (int n_ente = 0; n_ente < len_entes; n_ente++){
    Entity* ente = &entes[n_ente];
    // bleed por estocada
    if (ente->bleed == 's'){
      
      ente->hp -= (ente->bleed_counter) * 500;
      if (ente->hp <= 0){
        ente->alive = false;
        ente->hp = 0;
      }
    }
    // bleed por espina
    if(ente->bleed == 'e' && ente->bleed_counter > 0){
      ente->hp -= 400;
      ente->bleed_counter--;
      if (ente->hp <= 0){
        ente->alive = false;
        ente->hp = 0;
      }
    }
    // disminuir la duplicación de daño por sql
    if (ente->buffed) ente->buffed--;
    // disminuir el debuff por reprobado
    if (ente->reprobado) ente->reprobado--;
  }
}

int enemy_selector(Entity* yo, int len_enemigos){
  if (!len_enemigos) printf("Va a fallar, no hay enemigos\n");
  int enemy_pos;
  if (len_enemigos == 1)
  {
    printf("Se ataca al único enemigo disponible\n");
    enemy_pos = 0; // Solo hay un enemigo disponible
  }else if (yo->distracted){ // en el caso donde haya más opciones
    enemy_pos = yo->pos_focused;
    yo->distracted = false;
    printf("Monstruo distraído! Se atacará a %i \n", enemy_pos);
  }else{ // en el caso donde hayan muchos enemigos y no esté distraido
    enemy_pos = rand() % len_enemigos;
    printf("Se eligió atacar a %i\n", enemy_pos);
  }

  return enemy_pos;
}

int ally_selector(Entity* yo, int len_aliados, Entity* aliados){
  if (!len_aliados) printf("Va a fallar, no hay aliados\n");
  int ally_pos;
  if (len_aliados == 1)
  {
    printf("Se selecciona al único aliado disponible\n");
    ally_pos = 1; // Solo hay un enemigo disponible
  }else { // en el caso donde haya más opciones
    char *str1 = malloc(100*sizeof(char));
    char *str_atqs = calloc(450, sizeof(char));
    strcat(str_atqs, "\e[0;93mEscoge un aliado sobre el cual aplicar el efecto:\e[0m\n");
    for (int i=0; i<len_aliados; i++){
      sprintf(str1, "[%d] %s\n", i+1, aliados[i].playername);
      strcat(str_atqs, str1);
    }
    free(str1);
    send_txt(yo->socket, str_atqs);
    free(str_atqs);
    ally_pos = request_int(yo->socket, 1, len_aliados);
    printf("Se eligió ayudar a %i\n", ally_pos);
  }

  return ally_pos-1;
}


/** Realiza ataque
 * @param dano_base es el daño que se haría sin ningun cambio de status
 * @return cantidad de daño realizado
 */
int attack(Entity* atacante, Entity* objetivo, int dano_base){
  int dano_final = dano_base;
  if (atacante->buffed)
    dano_final *= 2; // duplica daño
  if (atacante->reprobado)
    dano_final /= 2; // inflige mitad de daño
  if (objetivo->reprobado)
    dano_final *= 2; // duplica daño
  // ver si se pasa del daño
  if (objetivo->hp < dano_final)
    dano_final = objetivo->hp;
  objetivo->hp -= dano_final;
  if(objetivo->hp <= 0)
    objetivo->alive = false;
  return dano_final;
}

/** Realiza curación
 * @param heal_base es el daño que se haría sin ningun cambio de status
 * @return cantidad de daño realizado
 */
int heal(Entity* healer, Entity* objetivo, int heal_base){
  int heal_final = heal_base;
  assert (objetivo->hp > 0);
  int heal_max = objetivo->max_hp - objetivo->hp;
  if (healer->reprobado)
    heal_final /= 2; // inflige mitad de heal
  if (objetivo->reprobado)
    heal_final /= 2; // recibe mitad heal
  if (heal_final > heal_max)
    heal_final = heal_max;
  objetivo->hp += heal_final;
  return heal_final;
}