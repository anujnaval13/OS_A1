// init: The initial user-level program

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define MAX_ATTEMPTS 3

char *argv[] = { "sh", 0 };

// Function to remove the trailing newline from input
void trim_newline(char *str) {
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            str[i] = '\0';
            break;
        }
    }
}

int main(void) {
    int pid, wpid;
    char user[32], pass[32];
    int attempts = 0;

    // Ensure console is open
    if (open("console", O_RDWR) < 0) {
        mknod("console", CONSOLE, 0);
        open("console", O_RDWR);
    }
    dup(0);  // stdout
    dup(0);  // stderr

    // Authentication loop
    while (attempts < MAX_ATTEMPTS) {
        printf("Enter Username: ");
        gets(user, sizeof(user));
        trim_newline(user);  // Remove trailing newline

        if (strcmp(user, USERNAME) != 0) {
            printf("Invalid Username\n");
            attempts++;
            continue;
        }

        printf("Enter Password: ");
        gets(pass, sizeof(pass));
        trim_newline(pass);  // Remove trailing newline

        if (strcmp(pass, PASSWORD) == 0) {
            printf("Login successful\n");
            break;
        } else {
            printf("Invalid Password\n");
            attempts++;
        }
    }

    if (attempts >= MAX_ATTEMPTS) {
        printf("Too many failed attempts. Access denied.\n");
        exit(1);
    }

    // Start shell loop
    for (;;) {
        printf("init: starting sh\n");
        pid = fork();
        if (pid < 0) {
            printf("init: fork failed\n");
            exit(1);
        }
        if (pid == 0) {
            exec("sh", argv);
            printf("init: exec sh failed\n");
            exit(1);
        }

        // Wait for shell to exit and restart it
        for (;;) {
            wpid = wait((int *) 0);
            if (wpid == pid) {
                break;
            } else if (wpid < 0) {
                printf("init: wait returned an error\n");
                exit(1);
            }
        }
    }
}
