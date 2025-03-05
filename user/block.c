#include "../kernel/types.h"
#include "user.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(2, "Usage: block <syscall_id>\n");
        exit(1);
    }

    int syscall_id = atoi(argv[1]);

    if (block(syscall_id) < 0) {
        fprintf(2, "Failed to block syscall %d\n", syscall_id);
        exit(1);
    } else {
        printf("Blocked syscall %d\n", syscall_id);
        exit(0);
    }
}