#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>

#define MAX_CMD_LEN 1024
#define MAX_PATH_LEN 2048

// Helper: Check if a file is executable
int is_executable(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        return (st.st_mode & S_IXUSR) != 0;
    }
    return 0;
}

// Helper: Find executable in PATH
char *find_in_path(const char *cmd) {
    char *path_env = getenv("PATH");
    if (!path_env) return NULL;

    // Copy PATH since strtok modifies the string
    char *path_copy = malloc(strlen(path_env) + 1);
    strcpy(path_copy, path_env);

    char *dir = strtok(path_copy, ":");
    static char full_path[MAX_PATH_LEN];

    while (dir) {
        snprintf(full_path, MAX_PATH_LEN, "%s/%s", dir, cmd);
        if (is_executable(full_path)) {
            free(path_copy);
            return full_path;
        }
        dir = strtok(NULL, ":");
    }

    free(path_copy);
    return NULL;
}

// Task: Locate executable files
void getPath(char *input) {
    char *path = getenv("PATH");
    printf("%s\n", path);
}

// Task: pwd builtin
void handle_pwd(void) {
    char cwd[MAX_PATH_LEN];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        printf("pwd: error retrieving current directory\n");
    }
}

// Task: cd builtin (absolute paths, relative paths, home directory)
void handle_cd(const char *path) {
    char target_path[MAX_PATH_LEN];

    // Handle home directory (~)
    if (path[0] == '~') {
        const char *home = getenv("HOME");
        if (!home) {
            printf("cd: HOME not set\n");
            return;
        }
        if (path[1] == '\0') {
            strcpy(target_path, home);
        } else if (path[1] == '/') {
            snprintf(target_path, MAX_PATH_LEN, "%s%s", home, path + 1);
        } else {
            printf("cd: invalid home directory syntax\n");
            return;
        }
    } else {
        strcpy(target_path, path);
    }

    if (chdir(target_path) != 0) {
        printf("cd: %s: No such file or directory\n", path);
    }
}

// Task: Run a program (with fork/exec)
void run_program(const char *cmd, char *args[]) {
    pid_t pid = fork();

    if (pid < 0) {
        printf("fork failed\n");
    } else if (pid == 0) {
        // Child process
        execvp(cmd, args);
        // execvp only returns on error
        printf("%s: command not found\n", cmd);
        exit(1);
    } else {
        // Parent process: wait for child
        int status;
        waitpid(pid, &status, 0);
    }
}

void handle_type(const char *input) {
    // Skip "type " and process the command
    while (*input && *input == ' ') input++;

    if (strlen(input) == 0) {
        printf("type: missing operand\n");
        return;
    }

    // Check builtins first
    if (strcmp(input, "type") == 0) {
        printf("type is a shell builtin\n");
    } else if (strcmp(input, "exit") == 0) {
        printf("exit is a shell builtin\n");
    } else if (strcmp(input, "echo") == 0) {
        printf("echo is a shell builtin\n");
    } else if (strcmp(input, "pwd") == 0) {
        printf("pwd is a shell builtin\n");
    } else if (strcmp(input, "cd") == 0) {
        printf("cd is a shell builtin\n");
    } else {
        // Check if it's in PATH
        char *executable = find_in_path(input);
        if (executable) {
            printf("%s is %s\n", input, executable);
        } else {
            printf("%s: not found\n", input);
        }
    }
}

void handle_input(char *input) {
    // Skip leading whitespace
    while (*input && *input == ' ') input++;

    if (strlen(input) == 0) {
        return;
    }

    // Parse input: split into command and arguments
    char input_copy[MAX_CMD_LEN];
    strcpy(input_copy, input);

    char *cmd = strtok(input_copy, " ");
    if (!cmd) return;

    // Handle builtins
    if (strcmp(cmd, "type") == 0) {
        char *arg = strtok(NULL, " ");
        if (arg) {
            handle_type(arg);
        } else {
            printf("type: missing operand\n");
        }
    } else if (strcmp(cmd, "exit") == 0) {
        exit(0);
    } else if (strcmp(cmd, "echo") == 0) {
        // Echo everything after "echo "
        char *echo_start = strchr(input, ' ');
        if (echo_start) {
            printf("%s\n", echo_start + 1);
        } else {
            printf("\n");
        }
    } else if (strcmp(cmd, "pwd") == 0) {
        handle_pwd();
    } else if (strcmp(cmd, "cd") == 0) {
        char *path = strtok(NULL, " ");
        if (path) {
            handle_cd(path);
        } else {
            // cd with no arguments goes to home
            const char *home = getenv("HOME");
            if (home) {
                handle_cd(home);
            }
        }
    } else {
        // External command: build args array for execvp
        char *args[MAX_CMD_LEN / 2];
        args[0] = cmd;
        int arg_count = 1;

        char input_copy2[MAX_CMD_LEN];
        strcpy(input_copy2, input);
        strtok(input_copy2, " "); // skip command

        char *arg = strtok(NULL, " ");
        while (arg && arg_count < MAX_CMD_LEN / 2 - 1) {
            args[arg_count++] = arg;
            arg = strtok(NULL, " ");
        }
        args[arg_count] = NULL;

        run_program(cmd, args);
    }
}

int main(int argc, char *argv[]) {
    while (1) {
        // Flush after every printf
        setbuf(stdout, NULL);

        printf("$ ");
        // Wait for user input
        char input[MAX_CMD_LEN];
        if (fgets(input, MAX_CMD_LEN, stdin) == NULL) {
            break; // EOF
        }
        input[strcspn(input, "\n")] = '\0';
        handle_input(input);
    }

    return 0;
}
