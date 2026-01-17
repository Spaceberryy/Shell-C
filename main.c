#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void read_input(char *input, size_t size) {
    fgets(input, size, stdin);   
    size_t n = strlen(input);
    if (n > 0 && input[n - 1] == '\n'){
        input[n - 1] = '\0';
    }
}

void handle_echo(char *input) {
    printf("%s\n", input + 5);
}

void handle_type(char *input) {
    if (strncmp(input + 5, "echo", strlen("echo")) == 0) {
        printf("echo is a shell builtin\n");
    }
    else if (strncmp(input + 5, "exit", strlen("exit")) == 0) {
        printf("exit is a shell builtin\n");
    }
    else if (strncmp(input + 5, "type", strlen("type")) == 0) {
        printf("type is a shell builtin\n");
    } 
    else {
        printf("%s: command not found\n", input + 5);
    }
}

void print_path(char *input) {
    char *path = getenv("PATH");
    printf("%s\n", path);
}

void handle_input(char *input) {
    if (!strcmp(input, "exit 0")){
        exit(0);
    } else if (strncmp(input, "echo", strlen("echo")) == 0) {
        handle_echo(input);
	print_path(input + 5);
    } else if (strncmp(input, "type", strlen("type")) == 0) {
        handle_type(input);
    } else {
	printf("%s: command not found\n", input);
    }
}



int main() {
    char input[100];

    while (1) {
        printf("$ ");
        fflush(stdout);
        read_input(input, sizeof(input));
        handle_input(input);
    }

    return 0;
}
