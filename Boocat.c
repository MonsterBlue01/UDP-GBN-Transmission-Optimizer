#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <err.h>

int main(int argc, char *argv[]) {
    if (argc == 1) {
        // Infinite loop to read and print input until EOF
        char c;
        while (read(STDIN_FILENO, &c, 1) == 1) {
            write(STDOUT_FILENO, &c, 1);
        }
    } else {
        int i;
        for (i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-") == 0) {
                // Read from stdin until EOF
                char c;
                while (read(STDIN_FILENO, &c, 1) == 1) {
                    write(STDOUT_FILENO, &c, 1);
                }
            } else {
                // Read from file and print its contents
                int fd = open(argv[i], O_RDONLY);
                if (fd == -1) {
                    warn("%s", argv[i]);
                    continue;
                }
                char buffer[BUFSIZ];
                ssize_t nread;
                while ((nread = read(fd, buffer, sizeof(buffer))) > 0) {
                    char *p = buffer;
                    while (nread > 0) {
                        ssize_t nwrite = write(STDOUT_FILENO, p, nread);
                        if (nwrite == -1) {
                            if (errno == EINTR) {
                                continue;
                            } else {
                                warn("write error");
                                break;
                            }
                        }
                        nread -= nwrite;
                        p += nwrite;
                    }
                    if (nread == -1) {
                        warn("read error");
                        break;
                    }
                }
                close(fd);
            }
            write(STDOUT_FILENO, " ", 1);
        }
        write(STDOUT_FILENO, "\n", 1);  // This line is optional (Not so sure about the format...)
    }
    return 0;
}