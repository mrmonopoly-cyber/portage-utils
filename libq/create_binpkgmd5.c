#include "config.h"

#include <fcntl.h>

#include "create_binpkgmd5.h"
#include "safe_io.h"
#include "hash.h"

#define HASH_SIZE 32

void create_binpkgmd5_file(void *hash_to_copy_to_conv)
{
  int fd_to_create;
  char * hash_to_copy = (char *) hash_to_copy_to_conv;

  fd_to_create= creat("BINPKGMD5",O_WRONLY|O_CREAT);
  safe_write(fd_to_create, hash_to_copy, HASH_SIZE);
  close(fd_to_create);
}
