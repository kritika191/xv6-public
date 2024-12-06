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
    } else if(strcmp(argv[1], "run") == 0){
        if(argc <3){
            printf(2, "Usage: strace run [command]\n");
            exit();
        }
        int pid = fork();
        if (pid<0)
        {
            printf(2, "fork failed\n");
            exit();
        }
        else if(pid == 0) //child
        {
            trace(1);
            if(exec(argv[2], &argv[2]) < 0)
            {
                printf(2, "exec failed for %s\n", argv[2]);
                exit();
            }
        }
        else{ //parent
            wait();
        }
    } else if(strcmp(argv[1], "dump") == 0){
        dumptrace();
    }
    else {
        printf(2, "Invalid argument. Use [on|off|dump|run] <command>\n");
    }
    exit();
}
