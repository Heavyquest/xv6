// user/sleep.c - pause for a number of ticks
#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc != 2){
    fprintf(2, "usage: sleep ticks\n");
    exit(1);
  }
  // reject anything that is not a plain non-negative integer
  for(char *s = argv[1]; *s; s++){
    if(*s < '0' || *s > '9'){
      fprintf(2, "sleep: invalid number of ticks '%s'\n", argv[1]);
      exit(1);
    }
  }
  if(pause(atoi(argv[1])) < 0){
    fprintf(2, "sleep: pause failed\n");
    exit(1);
  }
  exit(0);
}
