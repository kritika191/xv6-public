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
    } else {
        printf(2, "Invalid argument. Use 'on' or 'off'.\n");
    }
    exit();
}
