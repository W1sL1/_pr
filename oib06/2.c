#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PASSWORD_FILE "password.txt"
#define MAX_PASSWORD_LEN 100

unsigned char get_xor_key(size_t i) {
    return (unsigned char)(0x37 + ((i * 13) % 93));
}


void xor_crypt(char *data) {
    for (size_t i = 0; data[i] != '\0'; ++i) {
        data[i] ^= get_xor_key(i);
    }
}


int check_password(const char *input) {
    char correct_password[MAX_PASSWORD_LEN];
    FILE *file = fopen(PASSWORD_FILE, "r");
    if (file == NULL) {
        puts("Failed to open password file.");
        return 0;
    }
    if (fgets(correct_password, MAX_PASSWORD_LEN, file) == NULL) {
        fclose(file);
        puts("Error reading password.");
        return 0;
    }
    fclose(file);

    size_t len = strlen(correct_password);
    if (len > 0 && correct_password[len-1] == '\n') {
        correct_password[len-1] = '\0';
    }

    return strcmp(input, correct_password) == 0;
}

int main() {
    char input[MAX_PASSWORD_LEN];

    puts("Enter password: ");
    if (fgets(input, MAX_PASSWORD_LEN, stdin) == NULL) {
        puts("Input error.");
        return 1;
    }
    size_t len = strlen(input);
    if (len > 0 && input[len-1] == '\n') {
        input[len-1] = '\0';
    }

    if (check_password(input)) {
        puts("Password is correct!");
    } else {
        puts("Incorrect password.");
        puts("Press Enter to exit...");
        (void)getchar();
        exit(1);
    }

    puts("Welcome to the system!");
    puts("Press Enter to exit...");
    (void)getchar();

    return 0;
}