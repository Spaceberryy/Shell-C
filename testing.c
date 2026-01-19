
#include <stdlib.h>
#include <string.h>

int main() {
    char *path = getenv("PATH");
    const char *delim = {":"};

    int delimCount = 0;
    for (int i = 0; i < strlen(path); i++) {
	if (path[i] == ':') {
	    delimCount++;
	}
    }


    char *tokens[delimCount];

    char *token = strtok(path, delim);

    int index = 0;
    while (token != NULL) {
	token = strtok(NULL, delim);
	tokens[index++] = token;
	// printf("%s\n", token);
    }

    for (int i = 0; i < delimCount; i++) {
	printf("token %d: %s\n", i, tokens[i]);
    }

    return 0;
}
