// examples/simple_fork.c
#include <unistd.h>

int main() {
    if (fork() == 0)
    {
        syscall(64, 1, "child\n", 6);
    } else
    {
        syscall(64, 1, "parent\n", 7);
    }
    return 42;
}