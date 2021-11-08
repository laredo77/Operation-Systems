#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

char OLD_PATH[150];

/*
 * Job definition.
 * Using this struct, I will define what is a job.
 * consists of the job_ID and the name of the command.
 */
typedef struct {
    int job_id;
    char *command;
} Job;

/*
 * This function receives a string from the user and turns it into a valid command.
 * If the user add the & sign at the end of the string,
 * the function updates that the command should run
 * as a background command.
 */
void getCommand(char *buffer, int *background_flag) {

    printf("$ ");
    fflush(stdout);
    fgets(buffer, 100, stdin);
    buffer[strlen(buffer) - 1] = 0;

    if (buffer[strlen(buffer) - 1] == '&') {
        *background_flag = 1;
        buffer[strlen(buffer) - 2] = 0;
    }
}

/*
 * This function serves as helper function,
 * in purpose to count the words in the command entered by the user.
 */
int count_words(char *buffer) {

    int flag = 0;
    int words = 0;
    while (*buffer) {
        if (*buffer == ' ' || *buffer == '\n' || *buffer == '\t')
            flag = 0;
        else if (flag == 0) {
            flag = 1;
            words++;
        }
        buffer++;
    }
    return words;
}

/*
 * This function handles a command entered by the user and turns it into tokens.
 * In addition, if the command is echo command,
 * the function handles the edge case where '"' is added to the string,
 * removed it in purpose to print the echo without it.
 */
void parseInput(char **args, char *buffer) {
    char delimiter[2] = " ";
    int k, i = 0;
	
    char *token = strtok(buffer, delimiter);

    while (token != NULL) {
        args[i] = (char *) malloc((sizeof(char) * strlen(token)) + 1);
        if (!args[i]) {
            free(args[i]);
            args[i] = NULL;
            printf("An error occurred\n");
            break;
        }

        strcpy(args[i], token);
        i++;
        token = strtok(NULL, delimiter);
    }
    args[i - 1] = strtok(args[i - 1], "\n");
    args[i] = NULL;

    i = 0;
    if (strcmp(args[0], "echo") == 0) {
        while (args[i] != NULL) {
            if (args[i][0] == '"') {
                char temp[strlen(args[i]) - 2];
                for (k = 0; k < strlen(args[i]) - 2; k++) {
                    temp[k] = args[i][k + 1];
                }
                temp[strlen(args[i]) - 2] = '\0';
                strcpy(args[i], temp);
            }
            i++;
        }
    }
}

/*
 * The following function handles a shell command that is not builtin commands.
 * The function creates a child process that will run the command.
 * If the background flag is on, the command will run in parallel to the running process.
 */
void exeCommand(char **args, char *buffer, Job *jobs_array,
                int *amount_of_jobs, int background_flag, Job *history,
                int *history_counter) {

    int stat, ret_code;
    pid_t pid;
    pid = fork();

    Job add_to_history;
    add_to_history.job_id = pid;
    add_to_history.command = buffer;
    history[*history_counter] = add_to_history;
    *history_counter += 1;

    if (pid < 0) {
        printf("fork failed\n");
    } else if (pid == 0) {
        ret_code = execvp(args[0], args);
        if (ret_code == -1) {
            printf("exec failed\n");
        }
        exit(0);
    } else {
        if (background_flag) {
            Job new_job;
            new_job.job_id = pid;
            new_job.command = buffer;
            jobs_array[*amount_of_jobs] = new_job;
            *amount_of_jobs += 1;
        } else
            waitpid(pid, &stat, WUNTRACED);
    }
}

/*
 * A function that change the current working directory
 * to the home path working directory.
 */
void executeTildeSign() {
    getcwd(OLD_PATH, 150);
    if (chdir(getenv("HOME")) != 0) {
        printf("chdir failed\n");
        return;
    }
}

/*
 * Implementation of the command "-" which returns the user to
 * the last path of working directory where he\she worked.
 */
void executeMinuseSign() {
    char current_path[150];
    getcwd(current_path, 150);
    if (chdir(OLD_PATH) != 0) {
        printf("chdir failed\n");
        return;
    }
    strcpy(OLD_PATH, current_path);
}

/*
 * Implementation of the command ".." which is exactly
 * the same to the shell implementation of chdir("..").
 */
void executeDoubleDotSign() {
    getcwd(OLD_PATH, 150);
    if (chdir("..") != 0) {
        printf("chdir failed\n");
        return;
    }
}

/*
 * This function handles a cd command that is argument length is longer than 1.
 * If the start of the command has ~ or - signs, it call the appropriate functions.
 * Otherwise it will use the chdir command for the path, and if it fail,
 * it will print failed message.
 */
void executeCD(char *command) {

    if (command[0] == '~' || command[0] == '-') {
        if (command[1] == '~' || command[1] == '-') {
            printf("chdir failed\n");
            return;
        } else if (command[0] == '~') {
            executeTildeSign();
        } else
            executeMinuseSign();
        command = command + 2;
    }

    if (strlen(command) >= 1) {
        if (chdir(command) != 0) {
            printf("chdir failed\n");
            executeMinuseSign();
            return;
        }
    }
}

/*
 * This function execute the shell's builtin commands.
 * Whether it's a job, history or cd command, it execute the appropriate functions.
 */
void exeBuiltInCommand(char **args, char *buffer, Job *jobs_array,
                       int *amount_of_jobs, Job *history, int *history_counter, int size_of_args) {
	int i;
    Job add_to_history;
    add_to_history.job_id = getpid();
    add_to_history.command = buffer;
    history[*history_counter] = add_to_history;
    *history_counter += 1;

    if (strcmp(buffer, "jobs") == 0) {
        for (i = 0; i < (*amount_of_jobs); i++) {
            if (waitpid(jobs_array[i].job_id, NULL, WNOHANG) == 0)
                printf("%s\n", jobs_array[i].command);
        }
    } else if (strcmp(buffer, "history") == 0) {
        if (*history_counter == 1)
            printf("%s RUNNING\n", history[0].command);
        else {
            for (i = 0; i < (*history_counter) - 1; i++) {
                if (waitpid(history[i].job_id, NULL, WNOHANG) == 0) {

                    printf("%s RUNNING\n", history[i].command);
                } else {
                    printf("%s DONE\n", history[i].command);
                }
            }
            printf("%s RUNNING\n", history[*history_counter - 1].command);
        }
    } else if (strcmp(args[0], "cd") == 0) {
        if (args[1] != NULL && args[2] != NULL && strcmp(args[2], " ") != 0) {
            printf("Too many arguments\n");
            return;
        } else if (size_of_args == 1 || strcmp(args[1], "~") == 0) {
            executeTildeSign();
        } else if (strcmp(args[1], "..") == 0) {
            executeDoubleDotSign();
        } else if (strcmp(args[1], "-") == 0) {
            executeMinuseSign();
        } else if (strcmp(args[1], "/") == 0) {
            getcwd(OLD_PATH, 150);
            if (chdir("/") != 0) {
                printf("chdir failed\n");
                return;
            }
        } else if (args[1] != NULL && args[2] == NULL) {
            executeCD(args[1]);
        }
    } else if (strcmp(buffer, "exit") == 0)
        exit(0);
}

/*
 * main to run the program.
 */
int main() {
    Job jobs_array[100];
    Job history[100];
    char *built_in_commands[] = {"jobs", "history", "cd", "exit"};
    int words, amount_of_jobs = 0, background_flag;
    char buffer[100];
    int history_counter = 0;
    getcwd(OLD_PATH, 150);
    while (1) {
        background_flag = 0;
        getCommand(buffer, &background_flag);
        words = count_words(buffer);
        char *args[words + 1];
        char *copy_buffer = (char *) malloc(sizeof(char) * strlen(buffer) + 1);
        strcpy(copy_buffer, buffer);
        parseInput(args, buffer);

        if (strcmp(args[0], built_in_commands[0]) == 0 || strcmp(args[0], built_in_commands[1]) == 0
            || strcmp(args[0], built_in_commands[2]) == 0 || strcmp(args[0], built_in_commands[3]) == 0) {
            exeBuiltInCommand(args, copy_buffer, jobs_array, &amount_of_jobs, history, &history_counter, words);
        } else {
            exeCommand(args, copy_buffer, jobs_array, &amount_of_jobs,
                       background_flag, history, &history_counter);
        }
    }
}
