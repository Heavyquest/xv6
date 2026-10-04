#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data, int len);

int
main(int argc, char *argv[])
{
  if (argc == 1) {
    printf("Example 1:\n");
    int a[2] = {61810, 2026};
    memdump("ii", (char *)a, sizeof(a));

    printf("Example 2:\n");
    memdump("S", "a string", sizeof("a string"));

    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *)&s, sizeof(s));

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;

    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");

    printf("Example 4:\n");
    memdump("pihcS", (char *)&example, sizeof(example));

    printf("Example 5:\n");
    memdump("sccccc", (char *)&example, sizeof(example));
  } else if (argc == 2) {
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while (n < sizeof(data)) {
      int nn = read(0, data + n, sizeof(data) - n);
      if (nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data, n);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data, int len)
{
  int off = 0;

  for(; *fmt; fmt++){
    char f = *fmt;
    int need;

    switch(f){
    case 'i': need = 4; break;
    case 'p': need = 8; break;
    case 'h': need = 2; break;
    case 'c': need = 1; break;
    case 's': need = 8; break;
    case 'S': need = 0; break;
    default:
      fprintf(2, "memdump: unknown format '%c'\n", f);
      continue;
    }

    if(len - off < need){
      printf("memdump: not enough data for '%c'\n", f);
      return;
    }

    // memmove into locals avoids unaligned loads
    switch(f){
    case 'i': { int v;      memmove(&v, data + off, 4); printf("%d\n", v);  break; }
    case 'p': { uint64 v;   memmove(&v, data + off, 8); printf("%lx\n", v); break; }
    case 'h': { short v;    memmove(&v, data + off, 2); printf("%d\n", v);  break; }
    case 'c': printf("%c\n", data[off]); break;
    case 's': { char *s;    memmove(&s, data + off, 8); printf("%s\n", s);  break; }
    case 'S': {
      int n = 0;
      while(off + n < len && data[off + n] != 0)
        n++;
      write(1, data + off, n);
      write(1, "\n", 1);
      off += n;
      if(off < len)
        off++;           
      break;
    }
    }
    off += need;
  }
}
