#include "../kernel/types.h"
#include "user.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(2, "Usage: unblock <syscall_id>\n");
        exit(1);
    }

    int syscall_id = atoi(argv[1]);

    if (unblock(syscall_id) < 0) {
        fprintf(2, "Failed to unblock syscall %d\n", syscall_id);
        exit(1);
    } else {
        printf("Unblocked syscall %d\n", syscall_id);
        exit(0);
    }
}