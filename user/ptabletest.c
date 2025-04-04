#include "kernel/types.h"
#include "user.h"

int main(int argc, char *argv[]) {
  if(argc != 2) {
    printf("Usage: %s [0|1]\n", argv[0]);
    exit(1);
  }
  
  // Gọi syscall để bật/tắt tính năng
  if(signalPTE(atoi(argv[1])) < 0) {
    printf("Failed\n");
    exit(1);
  }
  // Chạy shell mới
  char *sh_argv[] = {"sh", 0};
  exec("sh", sh_argv);
  
  printf("exec failed\n");
  exit(1);
}