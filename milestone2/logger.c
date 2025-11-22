#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include "logger.h"

static int pipe_fd[2];
static pid_t logger_pid = 0;

void logger_loop(int read_fd) {
    FILE *log_file = fopen("gateway.log", "a");
    if (log_file == NULL) {
        perror("Logger: Failed to open gateway.log");
        exit(EXIT_FAILURE);
    }

    FILE *pipe_stream = fdopen(read_fd, "r");
    if (pipe_stream == NULL) {
        perror("Logger: fdopen failed");
        exit(EXIT_FAILURE);
    }

    char buffer[256];
    //ssize_t bytes_read;
    int sequence_num = 0;

    //while ((bytes_read = read(read_fd, buffer, sizeof(buffer) - 1)) > 0) {
    //    buffer[bytes_read] = '\0';
    // use fgets instead of read to avoid log overlap
    while (fgets(buffer, sizeof(buffer), pipe_stream) != NULL) {
        // avoid double newline
        buffer[strcspn(buffer, "\n")] = 0;

        time_t now;
        time(&now);
        struct tm *local = localtime(&now);
        char time_str[30];
        strftime(time_str, sizeof(time_str), "%a %b %d %H:%M:%S %Y", local);

        fprintf(log_file, "%d - %s - %s\n", sequence_num++, time_str, buffer);
        fflush(log_file);
    }

    fclose(pipe_stream);
    fclose(log_file);
    exit(EXIT_SUCCESS);
}

int create_log_process() {
    if (pipe(pipe_fd) == -1) {
        perror("Pipe creation failed");
        return -1;
    }
    logger_pid = fork();
    if (logger_pid < 0) {
        perror("Fork failed");
        return -1;
    }
    if (logger_pid == 0) {
        close(pipe_fd[1]);
        logger_loop(pipe_fd[0]);
        exit(0);
    } else {
        close(pipe_fd[0]);
        return 0;
    }
}

int write_to_log_process(char *msg) {
    if (logger_pid <= 0) return -1;

    char buffer[300];
    // add mandatory \n as the sign of ending
    snprintf(buffer, sizeof(buffer), "%s\n", msg);

    if (write(pipe_fd[1], buffer, strlen(buffer)) == -1) {
        return -1;
    }
    return 0;
}

int end_log_process() {
    if (logger_pid <= 0) return -1;
    close(pipe_fd[1]);
    waitpid(logger_pid, NULL, 0);
    logger_pid = 0;
    return 0;
}