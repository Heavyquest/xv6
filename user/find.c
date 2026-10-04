#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "user/user.h"

static char **cmdv = 0;  
static int cmdc = 0;

static char *
basename(char *path)
{
  char *p = path + strlen(path);
  while(p > path && p[-1] != '/')
    p--;
  return p;
}

static void
report(char *path)
{
  if(cmdc == 0){
    printf("%s\n", path);
    return;
  }
  char *args[MAXARG];
  int i;
  for(i = 0; i < cmdc; i++)
    args[i] = cmdv[i];
  args[i++] = path;
  args[i] = 0;

  int pid = fork();
  if(pid < 0){
    fprintf(2, "find: fork failed\n");
    return;
  }
  if(pid == 0){
    exec(args[0], args);
    fprintf(2, "find: exec %s failed\n", args[0]);
    exit(1);
  }
  if(wait(0) < 0)
    fprintf(2, "find: wait failed\n");
}

static void
find(char *path, char *name)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, O_RDONLY)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if(strcmp(basename(path), name) == 0)
    report(path);

  if(st.type == T_DIR){
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
      fprintf(2, "find: path too long: %s\n", path);
      close(fd);
      return;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    if(p == buf || p[-1] != '/')
      *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      find(buf, name);
    }
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 3 || argc == 4 ||
     (argc > 3 && strcmp(argv[3], "-exec") != 0)){
    fprintf(2, "usage: find dir name [-exec cmd [args...]]\n");
    exit(1);
  }
  if(argc > 4){
    cmdv = &argv[4];
    cmdc = argc - 4;
    if(cmdc + 2 > MAXARG){
      fprintf(2, "find: too many arguments for -exec\n");
      exit(1);
    }
  }
  find(argv[1], argv[2]);
  exit(0);
}
