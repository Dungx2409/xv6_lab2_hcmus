#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    printf("Printing page table...\n");
    ptableprint();
    exit(0);
}
