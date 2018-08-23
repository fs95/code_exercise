// Function: Recceive a character file as a parameter(in English and Chinese),
// sort and output each line according to pinyin.

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

#define MAX_LINE 1024
#define MAX_LENTH 1024

int StrcollWrapper(const void *v1, const void *v2)
{
    return strcoll(*(const char **)v1, *(const char **)v2);
}

int main(int argc, char const *argv[])
{
    char buffer[MAX_LENTH]; // File read buffer
    char *line[MAX_LINE]; // All lines of the file
    int linesN = 0; // The number of lines

    if (argc < 2) {
        fprintf(stderr, "Usage: %s [file]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *inputFile = fopen(argv[1], "r");
    if (inputFile == NULL) {
        perror("Error:"); // Output error information
        exit(EXIT_FAILURE);
    }

    // Locale set: The locale setting will affect the use of the "strcoll()"
    char *localeBackup = setlocale(LC_COLLATE, NULL);
    char *localeSet = setlocale(LC_COLLATE, "zh_CN.UTF-8");
    if (localeSet == NULL) {
        cout << "Locale setting failed!" << endl;
        exit(EXIT_FAILURE);
    }

    // The number of lines is calculated from zero
    for (linesN = 0; linesN < MAX_LINE && fgets(buffer, MAX_LENTH, inputFile) != NULL; linesN++) {
        line[linesN] = (char *)malloc(strlen(buffer) + 1); // Get memory
        strncpy(line[linesN], buffer, strlen(buffer) + 1); // Copy
    }

    // Sort and output
    qsort(line, linesN, sizeof(line[0]), StrcollWrapper);
    for (int i = 0; i < linesN; i++) {
        cout << line[i];
    }

    // Free
    setlocale(LC_COLLATE, localeBackup);
    fclose(inputFile);
    for (int i = 0; i < linesN; i++) {
        free(line[i]);
        line[i] = NULL;
    }

    return 0;
}
