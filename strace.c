#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[]) 
{
    if (argc < 2) {
        printf(2, "Usage: strace [on|off]\n");
        exit();
    }
    if (strcmp(argv[1], "on") == 0) {
        trace(1);
    } else if (strcmp(argv[1], "off") == 0) {
        trace(0);
    } else if(strcmp(argv[1], "run") == 0) {
        if(argc < 3) {
            printf(2, "Usage: strace run <command>\n");
            exit();
        }
        int pid = fork();
        if (pid < 0) {
            // Error
            printf(2, "Fork failed\n");
            exit();
        } else if (pid == 0) {
            // We are in a child process
            trace(1);
            if(exec(argv[2], &argv[2]) < 0) {
                printf(2, "exec failed for %s\n", argv[2]);
                exit();
            }
        } else {
            // We are in a parent process
            wait(); 
        }
    } else {
        printf(2, "Invalid argument. Use 'on' or 'off'.\n");
    }
    exit();
}
