#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

void write_strace_output(const char *filename) {
    char *buffer;
    int size = 4096; // Default buffer size
    int fd;

    // Dynamically allocate buffer
    buffer = malloc(size);
    if (!buffer) {
        printf(2, "Error: Unable to allocate buffer for strace output\n");
        exit();
    }

    // Call the syscall to fill the buffer with strace information
    if (writebuffer(buffer, size) < 0) {
        printf(2, "Error: Unable to retrieve strace output\n");
        free(buffer);
        exit();
    }

    // Open the file for writing
    fd = open(filename, O_CREATE | O_WRONLY);
    if (fd < 0) {
        printf(2, "Error: Unable to open file %s\n", filename);
        free(buffer);
        exit();
    }

    // Write the buffer content to the file
    if (write(fd, buffer, strlen(buffer)) < 0) {
        printf(2, "Error: Unable to write to file %s\n", filename);
        close(fd);
        free(buffer);
        exit();
    }

    printf(1, "Strace output successfully written to %s\n", filename);

    // Clean up
    close(fd);
    free(buffer);
}

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
    } else if (strcmp(argv[1], "-e") == 0) {
        // Enable tracing for a specific syscall
        if (argc != 3) {
            printf(2, "Usage: strace -e <syscall>\n");
            exit();
        }
        if (handleflags(1, argv[2]) < 0) {
            printf(2, "Error enabling -e flag for syscall: %s\n", argv[2]);
        }
    } else if (strcmp(argv[1], "-s") == 0) {
        // Enable tracing for successful syscalls only
        if (handleflags(2, "") < 0) {
            printf(2, "Error enabling -s flag\n");
        }
    } else if (strcmp(argv[1], "-f") == 0) {
        // Enable tracing for failed syscalls only
        if (handleflags(3, "") < 0) {
            printf(2, "Error enabling -f flag\n");
        }
    } else if (strcmp(argv[1], "-o") == 0) {
        if (argc != 3) {
            printf(2, "Usage: strace -o <filename>\n");
            exit();
        }
        write_strace_output(argv[2]); // Write output to file
    }
    else {
        printf(2, "Invalid argument. Use [on|off|dump|run] <command>\n");
    }
    exit();
}
