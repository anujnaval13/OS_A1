#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "inttypes.h"  // Include for PRIu64

#define HISTORY_SIZE 100 // Define based on kernel-side buffer size

struct history_entry {
    int pid;
    char name[16];
    uint64 mem_usage;
};

int main() {
    struct history_entry hist[HISTORY_SIZE];  // Use struct array instead of char buffer
    int count = history((void *)hist);  // Fetch process history

    if (count < 0) {
        printf("history failed\n");
        exit(1);
    }

    for (int i = 0; i < count; i++) {
        printf("%d %s %" PRIu64 "\n", hist[i].pid, hist[i].name, hist[i].mem_usage);
    }
    

    exit(0);
}
