#include <stdio.h>
#include <string.h>

void vulnerable_function(char *input) {
    char buffer[1000];
    strcpy(buffer, input); // Vulnerabilidad de Stack Buffer Overflow
    printf("Has introducido: %s\n", buffer);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <input>\n", argv[0]);
        return 1;
    }
    vulnerable_function(argv[1]);
    printf("Programa finalizado correctamente.\n");
    return 0;
}