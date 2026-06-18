// examples/write_hello.c
#include <unistd.h>

int main() {
    write(1, "hello\n", 6);
}