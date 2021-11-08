// Itamar Laredo 311547087
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h>

int compare_output(char *path1, char *path2) {
    char *file_args[] = {"comp", path1, path2, NULL};
    int status;
    pid_t pid;
    pid = fork();

    if (pid == 0) {
        if (execlp("./comp.out", "comp.out", path1, path2, NULL) < 0) {
            const char *s = "Error in: execlp\n";
            if (write(1, s, strlen(s)) < 0)
                exit(-1);
            exit(-1);
        }
    }

    if (waitpid(pid, &status, 0) < 0) {
        const char *s = "Error in: waitpid\n";
        if (write(1, s, strlen(s)) < 0)
            exit(-1);
        exit(-1);
    }

    if (WIFEXITED(status)) {
        const int res = WEXITSTATUS(status);
        return res;
    }
    return 0;
}

void readConfigFile(int fd) {

    char configFile[3][150];
    int i = 0, j = 0;
    int status, compare_res;
    size_t len;
    const char *extention;
    struct dirent *entry;
    struct dirent *entry2;
    char object[150];
    char grade[4];
    char reason[150];
    char new_path[150];
    char csv_line[150];
    char dir_path[150];
    char input_path[150];
    char output_path[150];
    char ch[1];
    int in, out, errors, opencsv;
    int hadCfile, retVal, offset = 0;

    opencsv = open("results.csv", O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (opencsv < 0) {
        const char *s = "Error in: open\n";
        if (write(1, s, strlen(s)) < 0)
            exit(-1);
        exit(-1);
    }

    retVal = read(fd, ch, 1);
    if (retVal < 0) {
        const char *s = "Error in: read\n";
        if (write(1, s, strlen(s)) < 0)
            exit(-1);
        exit(-1);
    }
    offset++;
    status = lseek(fd, offset, SEEK_SET);
    if (status < 0) {
        const char *s = "Error in: lseek\n";
        if (write(1, s, strlen(s)) < 0)
            exit(-1);
        exit(-1);
    }
    while (retVal == 1) {
        if (ch[0] == '\n') {
            configFile[i][j] = '\0';
            i++;
            j = 0;
            retVal = read(fd, ch, 1);
            if (retVal < 0) {
                const char *s = "Error in: read\n";
                if (write(1, s, strlen(s)) < 0)
                    exit(-1);
                exit(-1);
            }
            offset++;
            status = lseek(fd, offset, SEEK_SET);
            if (status < 0) {
                const char *s = "Error in: lseek\n";
                if (write(1, s, strlen(s)) < 0)
                    exit(-1);
                exit(-1);
            }
        }

        configFile[i][j] = ch[0];

        retVal = read(fd, ch, 1);
        if (retVal < 0) {
            const char *s = "Error in: read\n";
            if (write(1, s, strlen(s)) < 0)
                exit(-1);
            exit(-1);
        }
        offset++;
        status = lseek(fd, offset, SEEK_SET);
        if (status < 0) {
            const char *s = "Error in: lseek\n";
            if (write(1, s, strlen(s)) < 0)
                exit(-1);
            exit(-1);
        }
        j++;
    }

    strcpy(dir_path, "./");
    strcat(dir_path, configFile[0]);
    strcpy(input_path, "./");
    strcat(input_path, configFile[1]);
    strcpy(output_path, "./");
    strcat(output_path, configFile[2]);

    char *file_args[] = {"file", NULL};

    errors = open("errors.txt", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    if (errors < 0) {
        const char *s = "Error in: open\n";
        if (write(1, s, strlen(s)) < 0)
            exit(-1);
        exit(-1);
    }

    status = dup2(errors, 2);
    if (status < 0) {
        const char *s = "Error in: dup2\n";
        if (write(1, s, strlen(s)) < 0)
            exit(-1);
        exit(-1);
    }

    DIR *dir;
    DIR *dir2;
    if ((dir = opendir(dir_path)) == NULL) {
        const char *s = "Not a valid directory\n";
        if (write(1, s, strlen(s)) < 0)
            exit(-1);
        exit(-1);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            strcpy(object, entry->d_name);
            strcpy(new_path, dir_path);
            strcat(new_path, "/");
            strcat(new_path, entry->d_name);

            if ((dir2 = opendir(new_path)) == NULL) {
                const char *s = "Error in: opendir\n";
                if (write(1, s, strlen(s)) < 0)
                    exit(-1);
                exit(-1);
            }

            hadCfile = 0;
            while ((entry2 = readdir(dir2)) != NULL) {
                if (strcmp(entry2->d_name, ".") == 0 || strcmp(entry2->d_name, "..") == 0)
                    continue;

                len = strlen(entry2->d_name);
                extention = &entry2->d_name[len - 2];
                if (strcmp(extention, ".c") == 0 && entry2->d_type != DT_DIR) {
                    hadCfile = 1;

                    in = open(input_path, O_RDONLY);
                    if (in < 0) {
                        const char *s = "Input file not exist\n";
                        if (write(1, s, strlen(s)) < 0)
                            exit(-1);
                        exit(-1);
                    }
                    out = open("output.txt", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                    if (out < 0) {
                        const char *s = "Error in: open\n";
                        if (write(1, s, strlen(s)) < 0)
                            exit(-1);
                        exit(-1);
                    }

                    status = dup2(in, 0);
                    if (status < 0) {
                        const char *s = "Error in: dup2\n";
                        if (write(1, s, strlen(s)) < 0)
                            exit(-1);
                        exit(-1);
                    }
                    status = dup2(out, 1);
                    if (status < 0) {
                        const char *s = "Error in: dup2\n";
                        if (write(1, s, strlen(s)) < 0)
                            exit(-1);
                        exit(-1);
                    }
                    status = close(in);
                    if (status < 0) {
                        const char *s = "Error in: close\n";
                        if (write(1, s, strlen(s)) < 0)
                            exit(-1);
                        exit(-1);
                    }
                    status = close(out);
                    if (status < 0) {
                        const char *s = "Error in: close\n";
                        if (write(1, s, strlen(s)) < 0)
                            exit(-1);
                        exit(-1);
                    }

                    strcat(new_path, "/");
                    strcat(new_path, entry2->d_name);

                    if (fork() == 0) {
                        if (execlp("gcc", "gcc", new_path, "-o" , "file.out", NULL) < 0) {
                            const char *s = "Error in: execlp\n";
                            if (write(1, s, strlen(s)) < 0)
                                exit(-1);
                            exit(-1);
                        }
                    }
                    wait(&status);
                    if (WEXITSTATUS(status) != 0) {
                        strcpy(grade, "10");
                        strcpy(reason, "COMPILATION_ERROR");
                        continue;
                    }

                    if (fork() == 0) {
                        if (execvp("./file.out", file_args) < 0) {
                            const char *s = "Error in: execvp\n";
                            if (write(1, s, strlen(s)) < 0)
                                exit(-1);
                            exit(-1);
                        }
                    }
                    time_t begin = time(NULL);
                    wait(&status);
                    time_t end = time(NULL);
                    if ((end - begin) > 5) {
                        strcpy(grade, "20");
                        strcpy(reason, "TIMEOUT");
                        continue;
                    }

                    compare_res = compare_output(output_path, "./output.txt");

                    if (compare_res == 1) {
                        strcpy(grade, "100");
                        strcpy(reason, "EXCELLENT");
                    } else if (compare_res == 2) {
                        strcpy(grade, "50");
                        strcpy(reason, "WRONG");
                    } else if (compare_res == 3) {
                        strcpy(grade, "75");
                        strcpy(reason, "SIMILAR");
                    }
                }
                if (hadCfile == 0) {
                    strcpy(grade, "0");
                    strcpy(reason, "NO_C_FILE");
                }
            }

            strcpy(csv_line, object);
            strcat(csv_line, ",");
            strcat(csv_line, grade);
            strcat(csv_line, ",");
            strcat(csv_line, reason);
            strcat(csv_line, "\n");
            status = write(opencsv, csv_line, strlen(csv_line));
            if (status < 0) {
                exit(-1);
            }

            status = closedir(dir2);
            if (status < 0) {
                const char *s = "Error in: closedir\n";
                if (write(1, s, strlen(s)) < 0)
                    exit(-1);
                exit(-1);
            }
        }
    }

    status = remove("output.txt");
    if (status < 0) {
        const char *s = "Error in: remove\n";
        if (write(1, s, strlen(s)) < 0)
            exit(-1);
        exit(-1);
    }
    status = close(errors);
    if (status < 0) {
        const char *s = "Error in: close\n";
        if (write(1, s, strlen(s)) < 0)
            exit(-1);
        exit(-1);
    }
    status = closedir(dir);
    if (status < 0) {
        const char *s = "Error in: closedir\n";
        if (write(1, s, strlen(s)) < 0)
            exit(-1);
        exit(-1);
    }
    status = close(opencsv);
    if (status < 0) {
        const char *s = "Error in: close\n";
        if (write(1, s, strlen(s)) < 0)
            exit(-1);
        exit(-1);
    }
}

int main(int argc, char *argv[]) {
    int fd, status;

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        const char *s = "Error in: open\n";
        if (write(1, s, strlen(s)) < 0)
            exit(-1);
        exit(-1);
    }

    readConfigFile(fd);
    status = close(fd);
    if (status < 0) {
        const char *s = "Error in: close\n";
        if (write(1, s, strlen(s)) < 0)
            exit(-1);
        exit(-1);
    }

    return 0;
}
