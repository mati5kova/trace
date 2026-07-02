// examples/simple_fork.c
#include <stdio.h>
#include <unistd.h>

int main() {
    if (fork() == 0)
    {
        syscall(64, 1, "otrok\n", 6);
    } else
    {
        syscall(64, 1, "stars\n", 6);
    }
    return 42;
}