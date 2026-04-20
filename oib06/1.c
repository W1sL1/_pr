#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PASSWORD_FILE "password.txt"
#define MAX_PASSWORD_LEN 100

// Функция для проверки введённого пароля
int check_password(const char *input) {
    char correct_password[MAX_PASSWORD_LEN];
    FILE *file = fopen(PASSWORD_FILE, "r");
    if (file == NULL) {
        printf("Failed to open password file.\n");
        return 0;
    }
    if (fgets(correct_password, MAX_PASSWORD_LEN, file) == NULL) {
        fclose(file);
        printf("Error reading password.\n");
        return 0;
    }
    fclose(file);

    // Удалим символ новой строки, если он есть
    size_t len = strlen(correct_password);
    if (len > 0 && correct_password[len-1] == '\n') {
        correct_password[len-1] = '\0';
    }

    return strcmp(input, correct_password) == 0;
}

int main() {
    char input[MAX_PASSWORD_LEN];

    printf("Enter password: ");
    if (fgets(input, MAX_PASSWORD_LEN, stdin) == NULL) {
        printf("Input error.\n");
        return 1;
    }
    // Удалим символ новой строки, если он есть
    size_t len = strlen(input);
    if (len > 0 && input[len-1] == '\n') {
        input[len-1] = '\0';
    }

    if (check_password(input)) {
        printf("Password is correct!\n");
    } else {
        printf("Incorrect password.\n");
        exit(1);
    }


    printf("Welcome to the system!\n");

    return 0;
}