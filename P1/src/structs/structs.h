
enum os_error{
    invalid_mount_partition,
    invalid_delete_partition,
    invalid_read_file,
    invalid_delete_file,
		full_disk
    // TODO: completar
};

void os_strerror(enum os_error error){
  switch (error)
  {
  case invalid_partition:
    printf("La partición es inválida");
    break;
  default:
    break;
  }
}

typedef struct mbt
{
  /* data */
} Mbt;


typedef struct partition
{
	/* data */
} Partition;
