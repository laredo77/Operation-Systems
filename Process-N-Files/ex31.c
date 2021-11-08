// Itamar Laredo 311547087
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int compareFiles(int fd_1, int fd_2) {

    char ch1[1], ch2[1];
    int offset1 = 0, offset2 = 0;
    int maybe_similar = 0;
    int retVal1, retVal2, status;

    retVal1 = read(fd_1, ch1, 1);
    if (retVal1 < 0) {
        const char *s = "Error in: read\n";
        if (write(1, s, strlen(s)) < 0)
            exit(-1);
        exit(-1);
    }

    retVal2 = read(fd_2, ch2, 1);
    if (retVal2 < 0) {
        const char *s = "Error in: read\n";
        if (write(1, s, strlen(s)) < 0)
            exit(-1);
        exit(-1);
    }

    offset1++;
    offset2++;

    status = lseek(fd_1, offset1, SEEK_SET);
    if (status < 0) {
        const char *s = "Error in: lseek\n";
        if (write(1, s, strlen(s)) < 0)
            exit(-1);
        exit(-1);
    }

    lseek(fd_2, offset2, SEEK_SET);
    if (status < 0) {
        const char *s = "Error in: lseek\n";
        if (write(1, s, strlen(s)) < 0)
            exit(-1);
        exit(-1);
    }

    while (retVal1 == 1 && retVal2 == 1) {
        if (ch1[0] != ch2[0]) {
            if (ch1[0] == tolower(ch2[0]) || ch2[0] == tolower(ch1[0]))
                maybe_similar = 1;
            else if(isspace(ch1[0])) {
                retVal1 = read(fd_1, ch1, 1);
                if (retVal1 < 0) {
                    const char *s = "Error in: read\n";
                    if (write(1, s, strlen(s)) < 0)
                        exit(-1);
                    exit(-1);
                }
                offset1++;
                status = lseek(fd_1, offset1, SEEK_SET);
                if (status < 0) {
                    const char *s = "Error in: lseek\n";
                    if (write(1, s, strlen(s)) < 0)
                        exit(-1);
                    exit(-1);
                }
                maybe_similar = 1;
                continue;
            }
            else if(isspace(ch2[0])) {
                retVal2 = read(fd_2, ch2, 1);
                if (retVal2 < 0) {
                    const char *s = "Error in: read\n";
                    if (write(1, s, strlen(s)) < 0)
                        exit(-1);
                    exit(-1);
                }
                offset2++;
                status = lseek(fd_2, offset2, SEEK_SET);
                if (status < 0) {
                    const char *s = "Error in: lseek\n";
                    if (write(1, s, strlen(s)) < 0)
                        exit(-1);
                    exit(-1);
                }
                maybe_similar = 1;
                continue;
            }
            else if(ch1[0] != tolower(ch2[0]) || ch2[0] != tolower(ch1[0]))
                return 2;
        }

        retVal1 = read(fd_1, ch1, 1);
        if (retVal1 < 0) {
            const char *s = "Error in: read\n";
            if (write(1, s, strlen(s)) < 0)
                exit(-1);
            exit(-1);
        }

        retVal2 = read(fd_2, ch2, 1);
        if (retVal2 < 0) {
            const char *s = "Error in: read\n";
            if (write(1, s, strlen(s)) < 0)
                exit(-1);
            exit(-1);
        }

        offset1++;
        offset2++;
        status = lseek(fd_1, offset1, SEEK_SET);
        if (status < 0) {
            const char *s = "Error in: lseek\n";
            if (write(1, s, strlen(s)) < 0)
                exit(-1);
            exit(-1);
        }
        status = lseek(fd_2, offset2, SEEK_SET);
        if (status < 0) {
            const char *s = "Error in: lseek\n";
            if (write(1, s, strlen(s)) < 0)
                exit(-1);
            exit(-1);
        }
    }

    if (retVal1 == 1) {
        while (1) {
            if (retVal1 == 0)
                break;
            if (!(isspace(ch1[0])))
                return 2;
            retVal1 = read(fd_1, ch1, 1);
            if (retVal1 < 0) {
                const char *s = "Error in: read\n";
                if (write(1, s, strlen(s)) < 0)
                    exit(-1);
                exit(-1);
            }
            offset1++;
            status = lseek(fd_1, offset1, SEEK_SET);
            if (status < 0) {
                const char *s = "Error in: lseek\n";
                if (write(1, s, strlen(s)) < 0)
                    exit(-1);
                exit(-1);
            }
        }
    }

    else if (retVal2 == 1) {
        while (1) {
            if (retVal2 == 0)
                break;
            if (!(isspace(ch2[0])))
                return 2;
            retVal2 = read(fd_2, ch2, 1);
            if (retVal2 < 0) {
                const char *s = "Error in: read\n";
                if (write(1, s, strlen(s)) < 0)
                    exit(-1);
                exit(-1);
            }
            offset2++;
            status = lseek(fd_2, offset2, SEEK_SET);
            if (status < 0) {
                const char *s = "Error in: lseek\n";
                if (write(1, s, strlen(s)) < 0)
                    exit(-1);
                exit(-1);
            }
        }
    }

    if (maybe_similar)
        return 3;
    else if (offset1 != offset2)
        return 3;
    return 1;
}

int main(int argc, char *argv[]) {
    int result, status;
    int fd_1, fd_2;

    fd_1 = open(argv[1], O_RDONLY);
    if (fd_1 < 0) {
        const char *s = "Error in: open\n";
        if (write(1, s, strlen(s)) < 0)
            exit(-1);
        exit(-1);
    }

    fd_2 = open(argv[2], O_RDONLY);
    if (fd_2 < 0) {
        const char *s = "Error in: open\n";
        if (write(1, s, strlen(s)) < 0)
            exit(-1);
        exit(-1);
    }

    result = compareFiles(fd_1, fd_2);

    status = close(fd_1);
    if (status < 0) {
        const char *s = "Error in: close\n";
        if (write(1, s, strlen(s)) < 0)
            exit(-1);
        exit(-1);
    }
    status = close(fd_2);
    if (status < 0) {
        const char *s = "Error in: close\n";
        if (write(1, s, strlen(s)) < 0)
            exit(-1);
        exit(-1);
    }

    return result;
}