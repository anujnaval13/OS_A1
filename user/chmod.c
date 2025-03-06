#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: chmod <file> <mode>\n");
        exit(1);
    }

    int mode = atoi(argv[2]);  // Convert mode to int

    if (chmod(argv[1], mode) < 0) {
        printf("chmod failed\n");
        exit(1);
    }

    exit(0);
}