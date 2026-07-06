// examples/bad_open.c
#include <fcntl.h>
#include <unistd.h>

int main() {

    const int ret = open("/invalid/path", O_RDONLY);
    if (ret == -1)
    {
        write(1, "invalid path\n", 13);
        return 42;
    }

    close(ret);
    return 0;
}
