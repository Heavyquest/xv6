#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define SEPARATORS " -\r\t\n./,"

static int
sixfive(int fd)
{
  char c;
  int n;
  int val = 0;       
  int digits = 0;    
  int valid = 1;     

  for(;;){
    n = read(fd, &c, 1);
    if(n < 0)
      return -1;
    int end = (n == 0);                     
    if(end || strchr(SEPARATORS, c)){
      if(digits > 0 && valid && (val % 5 == 0 || val % 6 == 0))
        printf("%d\n", val);
      val = 0; digits = 0; valid = 1;
      if(end)
        return 0;
    } else if(c >= '0' && c <= '9'){
      val = val * 10 + (c - '0');
      digits++;
    } else {
      valid = 0;                             
    }
  }
}

int
main(int argc, char *argv[])
{
  int status = 0;

  if(argc < 2){
    if(sixfive(0) < 0){
      fprintf(2, "sixfive: read error\n");
      exit(1);
    }
    exit(0);
  }
  for(int i = 1; i < argc; i++){
    int fd = open(argv[i], O_RDONLY);
    if(fd < 0){
      printf("sixfive: cannot open %s\n", argv[i]);
      status = 1;
      continue;
    }
    if(sixfive(fd) < 0){
      fprintf(2, "sixfive: read error on %s\n", argv[i]);
      status = 1;
    }
    close(fd);
  }
  exit(status);
}
