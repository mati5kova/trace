// examples/comprehensive_test.c
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static volatile sig_atomic_t got_sigusr1 = 0;
static volatile sig_atomic_t got_sigchld = 0;

static void safe_write(const char *msg)
{
    write(STDOUT_FILENO, msg, strlen(msg));
}

static void die(const char *msg)
{
    perror(msg);
    exit(1);
}

static void sigusr1_handler(int signo)
{
    (void)signo;
    got_sigusr1 = 1;

    // async-signal-safe output
    const char msg[] = "handler: received SIGUSR1\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

static void sigchld_handler(int signo)
{
    (void)signo;
    got_sigchld = 1;

    const char msg[] = "handler: received SIGCHLD\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

static void install_signal_handlers(void)
{
    struct sigaction sa_usr1;
    memset(&sa_usr1, 0, sizeof(sa_usr1));
    sa_usr1.sa_handler = sigusr1_handler;
    sigemptyset(&sa_usr1.sa_mask);

    if (sigaction(SIGUSR1, &sa_usr1, NULL) == -1) {
        die("sigaction SIGUSR1");
    }

    struct sigaction sa_chld;
    memset(&sa_chld, 0, sizeof(sa_chld));
    sa_chld.sa_handler = sigchld_handler;
    sigemptyset(&sa_chld.sa_mask);

    if (sigaction(SIGCHLD, &sa_chld, NULL) == -1) {
        die("sigaction SIGCHLD");
    }
}

static void test_basic_syscalls(void)
{
    safe_write("\n== basic syscalls ==\n");

    pid_t pid = getpid();
    pid_t ppid = getppid();

    printf("main: pid=%ld ppid=%ld\n", (long)pid, (long)ppid);

    int fd = open("trace_test.tmp", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        die("open trace_test.tmp");
    }

    const char file_msg[] = "hello from trace_test.tmp\n";
    ssize_t written = write(fd, file_msg, sizeof(file_msg) - 1);
    printf("main: wrote %ld bytes to file\n", (long)written);

    close(fd);

    fd = open("trace_test.tmp", O_RDONLY);
    if (fd == -1) {
        die("open trace_test.tmp for reading");
    }

    char buffer[128];
    ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
    if (n == -1) {
        die("read trace_test.tmp");
    }

    buffer[n] = '\0';
    printf("main: read from file: %s", buffer);

    close(fd);

    // Intentionally failed syscall.
    int missing = open("definitely_missing_file_12345.txt", O_RDONLY);
    printf("main: intentionally failed open returned %d errno=%d (%s)\n",
           missing,
           errno,
           strerror(errno));
}

static void test_pipe(void)
{
    safe_write("\n== pipe ==\n");

    int pipefd[2];

    if (pipe(pipefd) == -1) {
        die("pipe");
    }

    const char msg[] = "message through pipe";

    if (write(pipefd[1], msg, sizeof(msg)) == -1) {
        die("write pipe");
    }

    char buffer[64];

    ssize_t n = read(pipefd[0], buffer, sizeof(buffer) - 1);
    if (n == -1) {
        die("read pipe");
    }

    buffer[n] = '\0';
    printf("main: pipe read: %s\n", buffer);

    close(pipefd[0]);
    close(pipefd[1]);
}

static void test_mmap(void)
{
    safe_write("\n== mmap ==\n");

    size_t length = 4096;

    void *mem = mmap(NULL,
                     length,
                     PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS,
                     -1,
                     0);

    if (mem == MAP_FAILED) {
        die("mmap");
    }

    char *text = (char *)mem;
    strcpy(text, "hello from mmap memory");

    printf("main: mmap address=%p content=\"%s\"\n", mem, text);

    if (munmap(mem, length) == -1) {
        die("munmap");
    }
}

static void test_signal_to_self(void)
{
    safe_write("\n== signal to self ==\n");

    printf("main: sending SIGUSR1 to myself\n");

    if (kill(getpid(), SIGUSR1) == -1) {
        die("kill self SIGUSR1");
    }

    printf("main: got_sigusr1=%d\n", got_sigusr1);
}

static void test_fork_and_wait(void)
{
    safe_write("\n== fork and wait ==\n");

    pid_t child = fork();

    if (child == -1) {
        die("fork");
    }

    if (child == 0) {
        printf("child1: pid=%ld ppid=%ld\n", (long)getpid(), (long)getppid());

        const char msg[] = "child1: writing using write syscall\n";
        write(STDOUT_FILENO, msg, sizeof(msg) - 1);

        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 100 * 1000 * 1000;
        nanosleep(&ts, NULL);

        _exit(42);
    }

    int status = 0;
    pid_t waited = waitpid(child, &status, 0);

    if (waited == -1) {
        die("waitpid child1");
    }

    if (WIFEXITED(status)) {
        printf("main: child1 exited with code %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("main: child1 killed by signal %d\n", WTERMSIG(status));
    }

    printf("main: got_sigchld=%d\n", got_sigchld);
}

static void test_fork_exec(void)
{
    safe_write("\n== fork and exec ==\n");

    pid_t child = fork();

    if (child == -1) {
        die("fork exec child");
    }

    if (child == 0) {
        printf("child2: about to exec /bin/echo\n");

        execlp("echo",
               "echo",
               "child2: hello from exec",
               NULL);

        perror("execlp echo");
        _exit(127);
    }

    int status = 0;

    if (waitpid(child, &status, 0) == -1) {
        die("waitpid child2");
    }

    if (WIFEXITED(status)) {
        printf("main: child2 exited with code %d\n", WEXITSTATUS(status));
    }
}

static void test_signal_killed_child(void)
{
    safe_write("\n== child killed by signal ==\n");

    pid_t child = fork();

    if (child == -1) {
        die("fork signal child");
    }

    if (child == 0) {
        printf("child3: going to sleep until parent kills me\n");

        for (;;) {
            pause();
        }
    }

    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 100 * 1000 * 1000;
    nanosleep(&ts, NULL);

    printf("main: sending SIGTERM to child3 pid=%ld\n", (long)child);

    if (kill(child, SIGTERM) == -1) {
        die("kill child3 SIGTERM");
    }

    int status = 0;

    if (waitpid(child, &status, 0) == -1) {
        die("waitpid child3");
    }

    if (WIFSIGNALED(status)) {
        printf("main: child3 killed by signal %d\n", WTERMSIG(status));
    }
}

static void test_multiple_children(void)
{
    safe_write("\n== multiple children ==\n");

    for (int i = 0; i < 3; i++) {
        pid_t child = fork();

        if (child == -1) {
            die("fork multiple children");
        }

        if (child == 0) {
            printf("child-loop-%d: pid=%ld\n", i, (long)getpid());
            _exit(10 + i);
        }
    }

    for (int i = 0; i < 3; i++) {
        int status = 0;
        pid_t waited = wait(&status);

        if (waited == -1) {
            die("wait multiple children");
        }

        if (WIFEXITED(status)) {
            printf("main: reaped child pid=%ld exit=%d\n",
                   (long)waited,
                   WEXITSTATUS(status));
        }
    }
}

int main(void)
{
    safe_write("trace_test: starting\n");

    install_signal_handlers();

    test_basic_syscalls();
    test_pipe();
    test_mmap();
    test_signal_to_self();
    test_fork_and_wait();
    test_fork_exec();
    test_signal_killed_child();
    test_multiple_children();

    unlink("trace_test.tmp");

    safe_write("\ntrace_test: done\n");

    return 0;
}