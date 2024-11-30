// strace.c
#include "types.h"
#include "user.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf(2, "Usage: strace on|off\n");
    exit();
  }

  if (strcmp(argv[1], "on") == 0) {
    trace(1); // Enable tracing
  } else if (strcmp(argv[1], "off") == 0) {
    trace(0); // Disable tracing
  } else {
    printf(2, "Usage: strace on|off\n");
  }

  exit();
}