#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <string.h>
#include "os_API.h"
#include "structs.h"
#include "../osfs/main.h"
#include "../util/bits.h"

char* path_disk;
int partition;
Mbt* mbt;

/** monta el os
 * @param
 * @returns Mbt*
*/
void os_mount(char* diskname, int partition_id){
  if (mbt){
    destroy_mbt();
  }
  mbt = malloc(sizeof(Mbt));

  path_disk = diskname;

  // OJO: rb+ para leer y escribir en binario (?)
  FILE* disk = fopen(diskname, "r+b");  // intentar con rb+
  init_mbt(disk);
  if (mbt->entry_container[partition_id] && mbt->entry_container[partition_id]->is_valid){
    partition = partition_id;
  } else{
    fprintf(stderr, "\e[1;91m [WHY] \e[0m: Partición inválida \n");
  }
  fclose(disk);
}

void os_bitmap(unsigned num){
  // instanciar bitmap
  Bitmap* bitmap = init_bitmap();

  if (num > bitmap->n_blocks || num < 0){
    os_strerror(invalid_input_range_bitmap);
    return;
  }

  fprintf(stderr, "\e[0;94m    Bitmap\e[0m\n\n");
  if (num == 0){
    uint8_t buffer[32];
    int counter = 0;
    for (int byte = 0; byte < bitmap->n_blocks * 2048; byte++){
      if (counter == 32){
        for (int i = 0; i < 32; i++){
          fprintf(stderr, "0x%02hhX ", buffer[i]);
        }
        fprintf(stderr, "\n");
        counter = 0;
      }
      buffer[counter] = bitmap->bytes[byte];
      counter++;
      uint8_t buff[1];
      buff[0] = bitmap->bytes[byte];
      for (int bit = 0; bit < 8; bit++){
        if (bt_get(buff, bit)){
          bitmap->full_blocks++;
        } else{
          bitmap->empty_blocks++;
        }
      }
    }
  } else{
    uint8_t buffer[32];
    int counter = 0;
    for(int byte = 2048 * (num - 1); byte < 2048 * (num - 1) + 2048; byte++){
      if (counter == 32){
        for (int i = 0; i < 32; i++){
          fprintf(stderr, "0x%02hhX ", buffer[i]);
        }
        fprintf(stderr, "\n");
        counter = 0;
      }
      buffer[counter] = bitmap->bytes[byte];
      counter++;
       // Contar bits ocupados y libres para cada byte
      uint8_t buff[1];
      buff[0] = bitmap->bytes[byte];
      for (int bit = 0; bit < 8; bit++){
        if (bt_get(buff, bit)){
          bitmap->full_blocks++;
        } else{
          bitmap->empty_blocks++;
        }
      }
    }
  }
  fprintf(stderr, "\n");
  fprintf(stderr, "\e[0;35mBloques Ocupados: \e[0m%d\n", bitmap->full_blocks);
  fprintf(stderr, "\e[1;35mBloques Libres: \e[0m%d\n", bitmap->empty_blocks);
  fprintf(stderr, "--------------------------\n\n");


  close_bitmap(bitmap);
}

bool os_exists(char* filename){
  FILE* disk = fopen(path_disk, "r+b");
  
  int inicio_directorio = 1024 + (mbt->entry_container[partition]->location)*2048 ;
  Directory *bloque_directory = directory_init(disk, inicio_directorio);
  for (int directory_entry = 0; directory_entry < bloque_directory->entry_quantity; directory_entry++)
  {
    if(bloque_directory->entries[directory_entry].is_valid)
    {
      if (!strcmp(bloque_directory->entries[directory_entry].filename, filename))
      {
        printf("directory_entry %i\n",directory_entry);
        printf("Archivo <%s> encontrado!\n",bloque_directory->entries[directory_entry].filename);
        fclose(disk);
        destroy_directory(bloque_directory);
        return true;
      }
    }
  }
  destroy_directory(bloque_directory);
  fclose(disk);
  return false;
}

void os_ls(){
  printf("Listando elementos de la partición actual\n");
  FILE* disk = fopen(path_disk, "r+b");

  int inicio_directorio = 1024 + (mbt->entry_container[partition]->location)*2048 ; // Dirección para todas las particiones
  Directory *bloque_directory = directory_init(disk, inicio_directorio /* REVISAREIS*/);
  
  // Tengo que ir a la particion y rescatar el identificador relativo
  for (int directory_entry = 0; directory_entry < bloque_directory->entry_quantity; directory_entry++)
  {
    if(bloque_directory->entries[directory_entry].is_valid)
    {
      printf("NOMBRE DEL ARCHIVO: %s\n", bloque_directory->entries[directory_entry].filename);
    }
  }
  printf("\n");
  destroy_directory(bloque_directory);
  fclose(disk);
}

void os_mbt(){
  printf("Particiones válidas\n");
  for (int entry_id = 0; entry_id < mbt->entry_quantity; entry_id++){
    if (mbt->entry_container[entry_id] && mbt->entry_container[entry_id]->is_valid){
      printf("    \e[0;35m id: \e[0m %d\n", mbt->entry_container[entry_id]->id);
      printf("    \e[0;94m location: \e[0m %d\n", mbt->entry_container[entry_id]->location);
      printf("    \e[1;35m size: \e[0m %d\n", mbt->entry_container[entry_id]->size);
      printf("    --------------------------\n\n");
    }
  }
}


void os_create_partition(int id, int size){
  printf("Creando partición %d de tamaño %d\n\n", id, size);
  if (size < 16384 || size > 131072)
  {
    os_strerror(invalid_create_partition);
    fprintf(stderr, "\e[1;91m [WHY] \e[0m: Wrong size as argument. Size must be between 16384 and 131072.\n\n \n");
  }
  else if (id < 0 || id > 127)
  {
    os_strerror(invalid_create_partition);
    fprintf(stderr, "\e[1;91m [WHY] \e[0m: wrong id as argument. Id must be between 0 and 128.\n\n");
  }
  else if (mbt->entry_container[id] && mbt->entry_container[id]->is_valid)
  {
    os_strerror(invalid_create_partition);
    fprintf(stderr, "\e[1;91m [WHY] \e[0m: Partition already created. If aditional partition is needed, please try another id.\n\n");
  }
  else
  {
    TEntry* tentry = NULL;
    for (int entry_id = 0; entry_id < mbt->entry_quantity; entry_id++){
      if (mbt->entry_container[entry_id] && mbt->entry_container[entry_id]->is_valid){
        tentry = init_tentry(tentry, mbt->entry_container[entry_id]->location, mbt->entry_container[entry_id]->size);
      }
    }

    if (!tentry || size < tentry->location)
    {
      Entry* entry = init_entry(true, id, 0, size);
      mbt->entry_container[id] = entry;
      write_new_partition(id, 0, size);
      printf("Partition created successfully.\n\n");
      destroy_tentry(tentry);
      return;
    }

    TEntry* current = tentry;
    while (current)
    {
      uint32_t posible_location = current->location + current->size;
      if (!current->next)
      {
        if (2097151 - posible_location + 1 >= size)
        {
          Entry* entry = init_entry(true, id, posible_location, size);
          mbt->entry_container[id] = entry;
          write_new_partition(id, posible_location, size);
          printf("Partition created successfully.\n\n");
        }
        else
        {
          os_strerror(invalid_create_partition);
          fprintf(stderr, "\e[1;91m [WHY] \e[0m: Partition cannot be allocated, please try a smaller size.\n\n");
        }
        destroy_tentry(tentry);
        return;
      }
      else if (current->next->location - posible_location >= size)
      {
        Entry* entry = init_entry(true, id, posible_location, size);
        mbt->entry_container[id] = entry;
        write_new_partition(id, posible_location, size);
        printf("Partition created successfully.\n\n");
        destroy_tentry(tentry);
        return;
      }
      current = current->next;
    }    
  }
}


void write_new_partition(int id, int location, int size) 
{
  FILE* disk = fopen(path_disk, "r+b");
  fseek(disk, 0, SEEK_SET);
  uint8_t buffer[8];

  for (int entry_id = 0; entry_id < mbt->entry_quantity; entry_id++)
  {
    fread(buffer, sizeof(uint8_t), (size_t)8, disk);
    bool is_valid = bt_get(buffer, 0);
    if (!is_valid)
    {
      fseek(disk, -8, SEEK_CUR);
      uint8_t new_buffer[8];
      new_buffer[0] = bit_and_id_to_byte(1, (uint8_t)id);
      insert_location_to_buffer(new_buffer, (uint32_t)location);
      insert_size_to_buffer(new_buffer, (uint32_t)size);
      fwrite(new_buffer, sizeof(uint8_t), 8, disk);
      create_bitmap(id);
      break;
    }
  }
  int inicio_directiory = (mbt->entry_container[partition]->location)*2048 + 1024;
  uint8_t buffer_validez2[1] = {0};
  uint8_t buffer_identificador[3] = {0, 0, 0};
  uint8_t buffer_nombre[28] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  
  fseek(disk, inicio_directiory, SEEK_SET);
  // El bloque directorio posee (2048/32) entradas
  for (int entrada_directory = 0; entrada_directory < 64; entrada_directory++)
  {
    // 1er byte de validez
    fwrite(buffer_validez2, sizeof(uint8_t), 1, disk);
    // 3 Bytes para el identificador relativo
    fwrite(buffer_identificador, sizeof(uint8_t), 3, disk);
    // 28 Bytes para el nombre del archivo
    fwrite(buffer_nombre, sizeof(uint8_t), 28, disk);
    // Ahora cambiamos la posición del puntero en el archivo para seguir escribiendo
    inicio_directiory = (mbt->entry_container[partition]->location)*2048 + 1024 + 32;
    fseek(disk, inicio_directiory, SEEK_SET);
    
  }
  
  // Sabemos que el primer bloque si o si es directory
  
  fclose(disk);
}

void create_bitmap(int id){
  int old_partition = partition;
  partition = id;
  Bitmap* bitmap = init_bitmap();
  // modificar
  for (int i = 0; i < bitmap->n_blocks * 2048; i++)
  {
    bitmap->bytes[i] = 0;
  }

  for(int i = 0; i < bitmap->n_blocks + 1; i++){
    bt_set(bitmap->bytes, i, true);
  }

  close_bitmap(bitmap);
  partition = old_partition;
}

// TODO: lanzar OS_ERROR invalid_delete_partition
void os_delete_partition(int delete_id){
  printf("Eliminando partición %d de la MBT\n\n", delete_id);
  if (partition == delete_id)
  {
    //os_strerror
    printf("ERROR: No se puede borrar una partición ya montada");
    return;
  }
  
  // Crea el puntero al archivo disk, en modo read write 
  FILE* disk = fopen(path_disk, "r+b");
  fseek(disk, 0, SEEK_SET);
  bool raise_error = true;
  uint8_t buffer[8];

  for (int entry_id = 0; entry_id < mbt->entry_quantity; entry_id++){

    // Obtenemos el id de partición y el bit de validez
    fread(buffer, sizeof(uint8_t), (size_t)1, disk);
    bool is_valid = bt_get(buffer, 0);
    int partition_id = get_partition_id(buffer[0]);

    if (is_valid && (partition_id == delete_id)){
      // si la partición es válida e id es correcto, cambia el primer byte de la entrada a 0.
      // es decir, el bit de validez y el id pasa a ser 0.
      uint8_t zero_buffer[1] = {0};
      fseek(disk, -1, SEEK_CUR);
      fwrite(zero_buffer, sizeof(uint8_t), 1, disk);
      mbt->entry_container[entry_id]->is_valid = 0;

      raise_error = false;
      break;
    }
    // Avanzamos hacia el siguiente trozo de 8 bytes, solución pobre
    fseek(disk, 8 * (entry_id + 1), SEEK_SET);
  }

  if (raise_error){
    // RAISE ERROR
  }
  fclose(disk);
}

void os_reset_mbt(){
  printf("Eliminando particiones de la MBT\n\n");

  // Crea el puntero al archivo disk, en modo read write 
  FILE* disk = fopen(path_disk, "r+b");
  fseek(disk, 0, SEEK_SET);
  uint8_t zero_buffer[1] = {0};

  for (int entry_id = 0; entry_id < mbt->entry_quantity; entry_id++){
    // Cambia el primer byte de la entrada a 0.
    // es decir, el bit de validez y el id pasa a ser 0.
    fwrite(zero_buffer, sizeof(uint8_t), 1, disk);
    // Basta con hacer la entrada inválida, ya que en ese caso se sobreescribe lo demás

    /* if (mbt->entry_container){
      if (mbt->entry_container[entry_id]){
        mbt->entry_container[entry_id]->is_valid = 0;
      }
    } */

    // No encontré si fwrite mueve el puntero en ningún lado :( p.d mi intuición dice que sí lo mueve
    // asi que tenemos la solución pobre:
    fseek(disk, 8 * (entry_id + 1), SEEK_SET);
  }

  // tal vez un raise_error pero no se me ocurre qué.
  fclose(disk);
}

void os_write(osFile* file_desc, void* buffer, int nbytes){
  printf("Escribiendo archivo\n\n");
}

// Debe retornar si el archivo cerró correctamente:
// 0: sin errores
// 1: error
int os_close(osFile* file_desc){
  printf("Cerrando archivo\n\n");

  int cantidad_bloques = (file_desc->index_block->file_size % 2048 ? 1 : 0) + file_desc->index_block->file_size / 2048 ; //Cada bloque es de 2048 bytes
  for (int bloque_actual = 0; bloque_actual < cantidad_bloques; bloque_actual++){
    free(file_desc->data_blocks[bloque_actual].array_bytes);
  }
  free(file_desc->data_blocks);

  destroy_directory(file_desc->relative_index);

  free(file_desc->index_block->punteros);
  free(file_desc->index_block);

  free(file_desc);
  return 0;
}



/*En la partición 2, el archivo nene.txt le encuentro su bloque índice en 25841 y después leo su tamaño de archivo que es 3521 Bytes*/
osFile* os_open(char* filename, char mode){ //TODO: Falta rellenar bloque de data, y probar la inicializacion del Directory
  // printf("Abriendo archivo %s\n\n", filename);
  osFile *ret_file = malloc(sizeof(osFile));
  ret_file->read_index = 0;
  FILE* disk = fopen(path_disk, "r+b");
  int inicio_directorio = 1024 + (mbt->entry_container[partition]->location)*2048 ; // Dirección para todas las particiones
  Directory *bloque_directory = directory_init(disk, inicio_directorio /* REVISAREIS*/);
  
  ret_file->relative_index = bloque_directory;
  // Tengo que ir a la particion y rescatar el identificador relativo
  
  if (mode == 'r' && os_exists(filename))
  {
    for (int directory_entry = 0; directory_entry < bloque_directory->entry_quantity; directory_entry++)
    {
      if(!strcmp(bloque_directory->entries[directory_entry].filename, filename)){
        ret_file->filename = bloque_directory->entries[directory_entry].filename;
        //Si el archivo era el que buscaba, procedo a rellenar sus bloques de datos y retorno
        uint64_t posicion_bloque_indice = 1024 + (bloque_directory->entries[directory_entry].relative_index)*2048 + (mbt->entry_container[partition]->location)*2048; // MBT + Particion + relative
        // Ahora rellenamos el IndexBlock para obtener punteros
        IndexBlock *bloque_index = indexblock_init(disk, posicion_bloque_indice /* REVISAREIS*/);
        ret_file->index_block = bloque_index;
        // Ahora rellenamos los DataBlocks
        datablocks_init(disk, ret_file);
        ret_file->mode = 'r';
        fclose(disk);
        return ret_file;
      }
    }
  }
  else if (mode == 'r')
  {
    fclose(disk);
    os_strerror(invalid_read_file);
    ret_file->mode = 'r';
    return ret_file;
  }
  else if (mode == 'w' && !os_exists(filename))
  {
    for (int directory_entry = 0; directory_entry < bloque_directory->entry_quantity; directory_entry++)
    {
      if(!strcmp(bloque_directory->entries[directory_entry].filename, filename)){
        ret_file->filename = filename;
        //Si el archivo era el que buscaba, procedo a rellenar sus bloques de datos y retorno
        uint64_t posicion_bloque_indice = 1024 + (bloque_directory->entries[directory_entry].relative_index)*2048 + (mbt->entry_container[partition]->location)*2048; // MBT + Particion + relative
        // Ahora rellenamos el IndexBlock para obtener punteros
        IndexBlock *bloque_index = indexblock_init(disk, posicion_bloque_indice /* REVISAREIS*/);
        ret_file->index_block = bloque_index;
        ret_file->mode = 'w';
        // Ahora rellenamos los DataBlocks
        datablocks_init(disk, ret_file);
        fclose(disk);
        return ret_file;
      }
    }
  }
  else if (mode == 'w')
  { //Acá ya existe
  for (int directory_entry = 0; directory_entry < bloque_directory->entry_quantity; directory_entry++)
    {
      if(!strcmp(bloque_directory->entries[directory_entry].filename, filename)){
        ret_file->filename = filename;
        //Si el archivo era el que buscaba, procedo a rellenar sus bloques de datos y retorno
        uint64_t posicion_bloque_indice = 1024 + (bloque_directory->entries[directory_entry].relative_index)*2048 + (mbt->entry_container[partition]->location)*2048; // MBT + Particion + relative
        // Ahora rellenamos el IndexBlock para obtener punteros
        IndexBlock *bloque_index = indexblock_init(disk, posicion_bloque_indice /* REVISAREIS*/);
        ret_file->index_block = bloque_index;
        // Ahora rellenamos los DataBlocks
        datablocks_init(disk, ret_file);
        ret_file->mode = 'w';
        fclose(disk);
        os_strerror(invalid_write_file);
        return ret_file;
      }
    }
  }
  else
  {
    for (int directory_entry = 0; directory_entry < bloque_directory->entry_quantity; directory_entry++)
    {
      if(!strcmp(bloque_directory->entries[directory_entry].filename, filename)){
        ret_file->filename = filename;
        //Si el archivo era el que buscaba, procedo a rellenar sus bloques de datos y retorno
        uint64_t posicion_bloque_indice = 1024 + (bloque_directory->entries[directory_entry].relative_index)*2048 + (mbt->entry_container[partition]->location)*2048; // MBT + Particion + relative
        // Ahora rellenamos el IndexBlock para obtener punteros
        IndexBlock *bloque_index = indexblock_init(disk, posicion_bloque_indice /* REVISAREIS*/);
        ret_file->index_block = bloque_index;
        // Ahora rellenamos los DataBlocks
        datablocks_init(disk, ret_file);
        fclose(disk);
        os_strerror(invalid_open_mode);
        ret_file->mode = 'r';
        return ret_file;
      }
    }
  }
  return NULL;
}

int os_rm(char* filename){
  if(os_exists(filename) == false){
    os_strerror(phantom_file_error);
    return 1;
  }else{
    /* Para matar un archivo en disco tengo que:
        - MATAR su entrada del bloque directorio
        - Borrar sus bloques de datos y escribirlos en disco (todos en 0)
        - Matar el index
    */
    printf("BORRANDO: %s\n", filename);
    FILE* disk = fopen(path_disk, "r+b");
    int inicio_directorio = 1024 + (mbt->entry_container[partition]->location)*2048 ; // Dirección para todas las particiones
    Directory *bloque_directory = directory_init(disk, inicio_directorio /* REVISAREIS*/);
    fseek(disk, inicio_directorio, SEEK_SET);
    uint8_t zero_bytes[1] = {0};
    uint8_t buffer[4];
    char buffer_name[29];
    int bloque_n;

    for (bloque_n = 0; bloque_n < bloque_directory->entry_quantity; bloque_n++){
      fread(buffer, sizeof(uint8_t), (size_t)4, disk);
      fread(buffer_name, sizeof(char), (size_t)28, disk);
      //if (bloque_n == 51) printf("[¿]%ld\n", ftell(disk));
      if (!buffer[0]) continue;
      //buffer_name[28] = '\0';
      if (!strcmp(buffer_name, filename)){  // igual nombre
        fseek(disk, (long int)-32, SEEK_CUR);
        fwrite(zero_bytes, sizeof(uint8_t), (size_t)1, disk);
        break;
      }
    }
    if (bloque_n == 64) return 1;

    uint32_t bloques_a_liberar[681];
    uint32_t relative_index = last_3_bytes_of_4(buffer);
    fseek(disk, 5 + relative_index * 2048 + 1024 * (mbt->entry_container[partition]->location) * 2048, SEEK_SET);
    uint8_t pointer[3];
    for (int i=0; i<681; i++){
      fread(pointer, sizeof(uint8_t), (size_t)3, disk);
      bloques_a_liberar[i] = three_bytes_to_int(pointer);
    }
    fclose(disk);
    // borrar en bitmap
    // cargar bitmap a memoria
    Bitmap* le_bitmap = init_bitmap();
    for (int i=0; i<681; i++){
      // no borrar el directorio por error
      if (!bloques_a_liberar[i]) continue;
      bt_set(le_bitmap->bytes, bloques_a_liberar[i], 0);
    }
    
    close_bitmap(le_bitmap);
    return 0;
  }
};
// Ahora que tenemos el IndexBlock procedemos a llenar los DataBlocks usando los punteros que este guarda

/*Lee los siguientes nbytes del archivo y los guarda en la dirección apuntada por buffer*/
int os_read(osFile* file_desc, void* buffer, int nbytes){
  if (file_desc->mode == 'r')
  {
    int read_now=0;
    int cantidad_bloques = (file_desc->index_block->file_size % 2048 ? 1 : 0) + file_desc->index_block->file_size / 2048 ; //Cada bloque es de 2048 bytes
    nbytes += file_desc->read_index;
    // leer extra hasta alcanzar el punto a seguir
    int over_read = file_desc->read_index;
    int to_read;
    if (file_desc->index_block->file_size > nbytes)
      to_read = nbytes;
    else
      to_read = file_desc->index_block->file_size;
    buffer -= over_read;
    for(int bloque_actual=0; bloque_actual < cantidad_bloques; bloque_actual++)
    {
      for(int i=0; i<to_read && i<2048; i++){
        if (over_read <= 0){
          *(uint8_t*)(buffer + i) = file_desc->data_blocks[bloque_actual].array_bytes[file_desc->read_index % 2048]; // A continuación llamamos cada bit del array
          file_desc->read_index += 1; // Sumamos uno al contador del archivo
          read_now++;
        }
        else
          over_read--;
      }
      to_read -= 2048;
      if (to_read <= 0) break;
      buffer += 2048;
    }
    return read_now;
  }
  os_strerror(invalid_read_file);
  return 0;
}


void extract_file(osFile* osfile)
{
  FILE* file = fopen(osfile->filename, "wb");
  int cantidad_bloques = (osfile->index_block->file_size % 2048 ? 1 : 0) + osfile->index_block->file_size / 2048;
  uint8_t buffer[2048];
  int read_bytes;
  for (int bloque = 0; bloque < cantidad_bloques; bloque++)
  {
    read_bytes = os_read(osfile, buffer, 2048);
    fwrite(buffer, sizeof(uint8_t), (size_t)(read_bytes), file);
  }
  osfile->read_index = 0;
  fclose(file);
}
