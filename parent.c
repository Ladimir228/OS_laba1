#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int p1[2];
    int mid[2];
    int p2[2];

    if (pipe(p1) == -1 || pipe(mid) == -1 || pipe(p2) == -1) {
        write(2, "pipe error\n", 11);
        return 1;
    }

    pid_t c1 = fork();
    if (c1 == -1) {
        write(2, "fork error\n", 11);
        return 1;
    }
    if (c1 == 0) {
        dup2(p1[0], 0);
        dup2(mid[1], 1);
        close(p1[0]); close(p1[1]);
        close(mid[0]); close(mid[1]);
        close(p2[0]); close(p2[1]);
        execl("./child1", "child1", (char *)NULL);
        write(2, "exec error\n", 11);
        _exit(1);
    }

    pid_t c2 = fork();
    if (c2 == -1) {
        write(2, "fork error\n", 11);
        return 1;
    }
    if (c2 == 0) {
        dup2(mid[0], 0);
        dup2(p2[1], 1);
        close(p1[0]); close(p1[1]);
        close(mid[0]); close(mid[1]);
        close(p2[0]); close(p2[1]);
        execl("./child2", "child2", (char *)NULL);
        write(2, "exec error\n", 11);
        _exit(1);
    }

    close(p1[0]);
    close(mid[0]);
    close(mid[1]);
    close(p2[1]);

    char buf[256];
    ssize_t n;

    while ((n = read(0, buf, sizeof(buf))) > 0) {
        if (write(p1[1], buf, n) == -1) {
            write(2, "write pipe1 error\n", 18);
            return 1;
        }
    }
    close(p1[1]);

    while ((n = read(p2[0], buf, sizeof(buf))) > 0) {
        if (write(1, buf, n) == -1) {
            write(2, "write stdout error\n", 19);
            return 1;
        }
    }
    close(p2[0]);

    waitpid(c1, NULL, 0);
    waitpid(c2, NULL, 0);
    return 0;
}
